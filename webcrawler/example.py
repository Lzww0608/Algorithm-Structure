#!/usr/bin/env python3
"""
网络爬虫使用示例

这个示例展示了如何使用爬虫的各个模块。
"""

from config import CrawlerConfig
from crawler import WebCrawler


def example_basic():
    """基础使用示例"""
    print("=" * 60)
    print("示例 1: 基础使用")
    print("=" * 60)
    
    # 使用默认配置
    crawler = WebCrawler()
    
    # 添加种子 URL
    crawler.add_seed_urls([
        "https://httpbin.org/html",  # 简单的测试页面
    ])
    
    # 开始爬取（限制为 5 个页面）
    stats = crawler.crawl(max_pages=5)
    
    print(f"\n爬取结果: {stats.pages_crawled} 个页面")


def example_custom_config():
    """自定义配置示例"""
    print("=" * 60)
    print("示例 2: 自定义配置")
    print("=" * 60)
    
    # 创建自定义配置
    config = CrawlerConfig(
        max_workers=3,              # 3 个并发线程
        max_depth=2,                # 最大深度 2
        max_pages=10,               # 最多 10 个页面
        request_delay=0.5,          # 请求间隔 0.5 秒
        enable_image_download=False,# 不下载图片
        storage_dir="./test_data"   # 自定义存储目录
    )
    
    # 限制只爬取特定域名
    config.allowed_domains = {"httpbin.org"}
    
    crawler = WebCrawler(config)
    crawler.add_seed_urls(["https://httpbin.org/links/5"])
    
    stats = crawler.crawl()
    print(f"\n爬取结果: {stats.pages_crawled} 个页面")


def example_module_usage():
    """单独使用各模块示例"""
    print("=" * 60)
    print("示例 3: 单独使用模块")
    print("=" * 60)
    
    # 1. 使用 URL Frontier
    from frontier import URLFrontier
    
    frontier = URLFrontier(politeness_delay=1.0)
    frontier.add("https://example.com/page1", depth=0, priority=1)
    frontier.add("https://example.com/page2", depth=0, priority=2)
    
    print(f"URL Frontier 队列大小: {frontier.size()}")
    
    # 2. 使用下载器
    from downloader import HTMLDownloader
    
    downloader = HTMLDownloader(timeout=10)
    result = downloader.download("https://httpbin.org/html")
    
    print(f"下载状态: {'成功' if result.success else '失败'}")
    print(f"状态码: {result.status_code}")
    print(f"内容长度: {len(result.content)} 字符")
    
    # 3. 使用解析器
    from parser import ContentParser
    
    parser = ContentParser()
    parsed = parser.parse(result.content, result.final_url)
    
    print(f"页面标题: {parsed.title}")
    print(f"链接数量: {len(parsed.links)}")
    
    # 4. 使用去重器
    from dedup import Deduplicator
    
    dedup = Deduplicator()
    
    is_new = dedup.url_dedup.check_and_mark("https://example.com/page1")
    print(f"URL 是否为新: {is_new}")
    
    is_new = dedup.url_dedup.check_and_mark("https://example.com/page1")
    print(f"同一 URL 再次检查: {is_new}")
    
    # 5. 使用 URL 过滤器
    from url_filter import URLFilter
    
    url_filter = URLFilter()
    
    test_urls = [
        "https://example.com/page",
        "https://facebook.com/page",  # 默认黑名单
        "https://example.com/file.pdf",  # 被阻止的扩展名
        "javascript:void(0)",  # 无效 URL
    ]
    
    print("\nURL 过滤测试:")
    for url in test_urls:
        result = url_filter.filter(url)
        status = "✓ 通过" if result.passed else f"✗ 阻止 ({result.reason})"
        print(f"  {url[:40]:40} {status}")


def main():
    """运行示例"""
    import sys
    
    examples = {
        "1": ("基础使用", example_basic),
        "2": ("自定义配置", example_custom_config),
        "3": ("模块使用", example_module_usage),
    }
    
    if len(sys.argv) > 1:
        choice = sys.argv[1]
    else:
        print("网络爬虫示例")
        print("-" * 40)
        for key, (name, _) in examples.items():
            print(f"  {key}. {name}")
        print("-" * 40)
        choice = input("请选择示例 (1-3): ").strip()
    
    if choice in examples:
        name, func = examples[choice]
        print(f"\n运行: {name}\n")
        func()
    else:
        print("无效选择")


if __name__ == "__main__":
    main()

