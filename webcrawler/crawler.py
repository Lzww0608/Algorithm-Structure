"""
Crawler - 爬虫主控制器

整合所有模块，实现完整的爬虫流程:
1. 从 URL Frontier 获取 URL
2. 下载页面内容
3. 解析内容
4. 去重检查
5. 存储内容
6. 提取新链接
7. 过滤并加入队列
"""

import time
import threading
from typing import List, Optional, Set
from concurrent.futures import ThreadPoolExecutor, as_completed
from dataclasses import dataclass

from .config import CrawlerConfig, default_config
from .frontier import URLFrontier, URLItem
from .downloader import HTMLDownloader, DownloadResult
from .dns_resolver import DNSResolver
from .parser import ContentParser, ParsedContent
from .dedup import Deduplicator
from .storage import Storage
from .url_filter import URLFilter
from .extensions import ExtensionManager, LinkExtractor, ImageDownloader, WebMonitor


@dataclass
class CrawlStats:
    """爬虫统计数据"""
    start_time: float = 0.0
    end_time: float = 0.0
    pages_crawled: int = 0
    pages_failed: int = 0
    pages_skipped: int = 0
    total_bytes: int = 0
    urls_discovered: int = 0
    urls_filtered: int = 0


class WebCrawler:
    """
    网络爬虫主控制器
    
    协调所有模块完成爬取任务
    """
    
    def __init__(self, config: CrawlerConfig = None):
        """
        初始化爬虫
        
        Args:
            config: 爬虫配置
        """
        self._config = config or default_config
        
        # 初始化核心组件
        self._dns_resolver = DNSResolver(
            cache_size=self._config.dns_cache_size,
            cache_ttl=self._config.dns_cache_ttl
        )
        
        self._frontier = URLFrontier(
            politeness_delay=self._config.request_delay
        )
        
        self._downloader = HTMLDownloader(
            user_agent=self._config.user_agent,
            timeout=self._config.request_timeout,
            dns_resolver=self._dns_resolver
        )
        
        self._parser = ContentParser()
        
        self._dedup = Deduplicator(
            capacity=self._config.bloom_filter_capacity,
            error_rate=self._config.bloom_filter_error_rate
        )
        
        self._storage = Storage(base_dir=self._config.storage_dir)
        
        self._url_filter = URLFilter(
            allowed_domains=self._config.allowed_domains,
            blocked_domains=self._config.blocked_domains,
            blocked_extensions=self._config.blocked_extensions
        )
        
        # 初始化扩展模块
        self._extension_manager = ExtensionManager()
        self._setup_extensions()
        
        # 状态
        self._running = False
        self._stats = CrawlStats()
        self._lock = threading.Lock()
    
    def _setup_extensions(self):
        """设置扩展模块"""
        # 链接提取器（始终启用）
        link_extractor = LinkExtractor()
        self._extension_manager.register(link_extractor)
        
        # 图片下载器
        if self._config.enable_image_download:
            image_downloader = ImageDownloader(
                save_dir=self._config.image_dir
            )
            self._extension_manager.register(image_downloader)
        
        # 网页监控
        if self._config.enable_web_monitor:
            web_monitor = WebMonitor(
                history_dir=f"{self._config.storage_dir}/monitor"
            )
            self._extension_manager.register(web_monitor)
    
    def add_seed_urls(self, urls: List[str]):
        """
        添加种子 URL
        
        Args:
            urls: 种子 URL 列表
        """
        for url in urls:
            # 验证 URL
            if self._url_filter.filter(url).passed:
                self._frontier.add(url, depth=0, priority=1)
                print(f"[Crawler] 添加种子 URL: {url}")
            else:
                print(f"[Crawler] 跳过无效种子 URL: {url}")
    
    def crawl(self, max_pages: int = None) -> CrawlStats:
        """
        开始爬取
        
        Args:
            max_pages: 最大爬取页面数（覆盖配置）
            
        Returns:
            爬取统计
        """
        max_pages = max_pages or self._config.max_pages
        
        self._running = True
        self._stats = CrawlStats(start_time=time.time())
        
        print(f"\n{'='*60}")
        print(f"开始爬取 - 最大页面数: {max_pages}")
        print(f"{'='*60}\n")
        
        # 使用线程池进行并发爬取
        with ThreadPoolExecutor(max_workers=self._config.max_workers) as executor:
            futures = []
            
            while self._running and self._stats.pages_crawled < max_pages:
                # 获取待处理的 URL
                url_item = self._frontier.get(timeout=5)
                
                if url_item is None:
                    # 检查是否有正在处理的任务
                    if not futures:
                        print("[Crawler] 没有更多 URL 可爬取")
                        break
                    
                    # 等待一些任务完成
                    done_futures = []
                    for future in as_completed(futures, timeout=1):
                        done_futures.append(future)
                        if len(done_futures) >= 1:
                            break
                    
                    for future in done_futures:
                        futures.remove(future)
                    
                    continue
                
                # 检查深度限制
                if url_item.depth > self._config.max_depth:
                    continue
                
                # 检查 URL 是否已访问
                if self._dedup.is_url_seen(url_item.url):
                    self._stats.pages_skipped += 1
                    continue
                
                # 提交爬取任务
                future = executor.submit(self._crawl_page, url_item)
                futures.append(future)
                
                # 清理已完成的 futures
                done = [f for f in futures if f.done()]
                for f in done:
                    futures.remove(f)
            
            # 等待所有任务完成
            for future in as_completed(futures):
                pass
        
        self._stats.end_time = time.time()
        self._running = False
        
        self._print_summary()
        
        return self._stats
    
    def _crawl_page(self, url_item: URLItem) -> bool:
        """
        爬取单个页面
        
        Args:
            url_item: URL 项
            
        Returns:
            是否成功
        """
        url = url_item.url
        depth = url_item.depth
        
        print(f"[Crawler] 爬取 [{depth}]: {url}")
        
        # 1. 标记 URL 为已访问
        self._dedup.mark_url_seen(url)
        
        # 2. 下载页面
        download_result = self._downloader.download(url)
        
        if not download_result.success:
            print(f"[Crawler] 下载失败: {url} - {download_result.error_message}")
            with self._lock:
                self._stats.pages_failed += 1
            return False
        
        # 3. 内容去重检查
        is_new_content, content_hash = self._dedup.content_dedup.check_and_mark(
            download_result.content
        )
        
        if not is_new_content:
            print(f"[Crawler] 重复内容: {url}")
            with self._lock:
                self._stats.pages_skipped += 1
            return False
        
        # 4. 解析内容
        parsed = self._parser.parse(download_result.content, url)
        
        if not parsed.success:
            print(f"[Crawler] 解析失败: {url}")
            with self._lock:
                self._stats.pages_failed += 1
            return False
        
        # 5. 存储内容
        stored = self._storage.save_page(
            url=url,
            content=download_result.content,
            title=parsed.title,
            final_url=download_result.final_url,
            status_code=download_result.status_code,
            content_type=download_result.content_type,
            depth=depth,
            parent_url=url_item.parent_url,
            word_count=parsed.word_count,
            links_count=len(parsed.links),
            images_count=len(parsed.images)
        )
        
        print(f"[Crawler] 存储: {stored.title or url}")
        
        # 6. 运行扩展模块
        extension_data = {
            'links': parsed.links,
            'images': parsed.images,
            'title': parsed.title,
            'status_code': download_result.status_code,
            'download_time': download_result.download_time
        }
        
        ext_results = self._extension_manager.process(
            url, download_result.content, extension_data
        )
        
        # 7. 处理提取的链接
        new_links = parsed.links
        for ext_result in ext_results:
            if ext_result.extension_name == "LinkExtractor" and ext_result.success:
                new_links = ext_result.data.get('internal_links', parsed.links)
                break
        
        # 8. 过滤并添加新 URL
        added_count = 0
        for link in new_links:
            filter_result = self._url_filter.filter(link)
            
            if filter_result.passed:
                if not self._dedup.is_url_seen(link):
                    # 计算优先级（深度越深优先级越低）
                    priority = min(depth + 2, 10)
                    
                    if self._frontier.add(link, depth=depth + 1, priority=priority, parent_url=url):
                        added_count += 1
            else:
                with self._lock:
                    self._stats.urls_filtered += 1
        
        # 更新统计
        with self._lock:
            self._stats.pages_crawled += 1
            self._stats.total_bytes += len(download_result.content)
            self._stats.urls_discovered += added_count
        
        return True
    
    def stop(self):
        """停止爬取"""
        print("[Crawler] 正在停止...")
        self._running = False
    
    def _print_summary(self):
        """打印爬取总结"""
        duration = self._stats.end_time - self._stats.start_time
        
        print(f"\n{'='*60}")
        print("爬取完成 - 统计摘要")
        print(f"{'='*60}")
        print(f"耗时: {duration:.2f} 秒")
        print(f"成功爬取: {self._stats.pages_crawled} 页")
        print(f"失败: {self._stats.pages_failed} 页")
        print(f"跳过（重复）: {self._stats.pages_skipped} 页")
        print(f"发现新 URL: {self._stats.urls_discovered}")
        print(f"过滤 URL: {self._stats.urls_filtered}")
        print(f"下载总量: {self._format_bytes(self._stats.total_bytes)}")
        print(f"平均速度: {self._stats.pages_crawled / duration:.2f} 页/秒" if duration > 0 else "")
        print(f"{'='*60}")
        
        # 打印各模块统计
        print("\n模块统计:")
        print(f"  - DNS: {self._dns_resolver.stats()}")
        print(f"  - 下载器: {self._downloader.stats()}")
        print(f"  - 去重: {self._dedup.stats()}")
        print(f"  - 存储: {self._storage.stats()}")
        print(f"  - URL 过滤: {self._url_filter.stats()}")
        print(f"  - 扩展: {self._extension_manager.stats()}")
    
    def _format_bytes(self, size: int) -> str:
        """格式化字节大小"""
        for unit in ["B", "KB", "MB", "GB"]:
            if size < 1024:
                return f"{size:.2f} {unit}"
            size /= 1024
        return f"{size:.2f} TB"
    
    def get_stats(self) -> dict:
        """获取当前统计"""
        return {
            "crawler": {
                "pages_crawled": self._stats.pages_crawled,
                "pages_failed": self._stats.pages_failed,
                "pages_skipped": self._stats.pages_skipped,
                "urls_discovered": self._stats.urls_discovered,
                "urls_filtered": self._stats.urls_filtered,
                "total_bytes": self._stats.total_bytes
            },
            "frontier": self._frontier.stats(),
            "downloader": self._downloader.stats(),
            "dedup": self._dedup.stats(),
            "storage": self._storage.stats(),
            "url_filter": self._url_filter.stats(),
            "extensions": self._extension_manager.stats()
        }

