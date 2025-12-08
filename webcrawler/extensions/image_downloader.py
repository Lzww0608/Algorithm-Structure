"""
Image Downloader - 图片下载器扩展

功能:
1. 从页面中提取图片 URL
2. 下载并保存图片
3. 支持异步下载
"""

import os
import hashlib
import threading
import queue
from typing import List, Set, Optional
from urllib.parse import urlparse, urljoin
from concurrent.futures import ThreadPoolExecutor
from .base import Extension, ExtensionResult


class ImageDownloader(Extension):
    """
    图片下载器
    
    下载页面中的图片资源
    """
    
    def __init__(
        self,
        save_dir: str = "./crawled_data/images",
        max_workers: int = 3,
        allowed_extensions: Set[str] = None,
        max_size_mb: float = 10.0
    ):
        """
        初始化图片下载器
        
        Args:
            save_dir: 图片保存目录
            max_workers: 并发下载数
            allowed_extensions: 允许的图片扩展名
            max_size_mb: 最大图片大小（MB）
        """
        super().__init__("ImageDownloader")
        self._save_dir = save_dir
        self._max_workers = max_workers
        self._allowed_extensions = allowed_extensions or {
            '.png', '.jpg', '.jpeg', '.gif', '.webp', '.svg', '.ico'
        }
        self._max_size = int(max_size_mb * 1024 * 1024)
        
        # 创建保存目录
        os.makedirs(self._save_dir, exist_ok=True)
        
        # 统计
        self._downloaded_count = 0
        self._failed_count = 0
        self._total_bytes = 0
        self._downloaded_urls: Set[str] = set()
        
        # 下载队列和线程池
        self._download_queue: queue.Queue = queue.Queue()
        self._executor: Optional[ThreadPoolExecutor] = None
        self._lock = threading.Lock()
    
    def process(self, url: str, content: str, parsed_data: dict) -> ExtensionResult:
        """
        处理页面，提取并下载图片
        
        Args:
            url: 页面 URL
            content: 页面内容
            parsed_data: 解析后的数据
            
        Returns:
            ExtensionResult
        """
        try:
            # 获取图片 URL 列表
            images = parsed_data.get('images', [])
            
            if not images and content:
                # 如果没有预解析的图片，自行提取
                images = self._extract_images_from_html(content, url)
            
            # 过滤和下载
            valid_images = self._filter_images(images)
            downloaded = []
            failed = []
            
            for img_url in valid_images:
                if img_url in self._downloaded_urls:
                    continue
                
                result = self._download_image(img_url)
                if result:
                    downloaded.append(result)
                    with self._lock:
                        self._downloaded_urls.add(img_url)
                else:
                    failed.append(img_url)
            
            return ExtensionResult(
                extension_name=self.name,
                success=True,
                data={
                    "total_found": len(images),
                    "downloaded": downloaded,
                    "failed": failed,
                    "downloaded_count": len(downloaded),
                    "failed_count": len(failed)
                }
            )
        
        except Exception as e:
            return ExtensionResult(
                extension_name=self.name,
                success=False,
                error_message=str(e)
            )
    
    def _extract_images_from_html(self, html: str, base_url: str) -> List[str]:
        """从 HTML 提取图片 URL"""
        from bs4 import BeautifulSoup
        import re
        
        images = []
        soup = BeautifulSoup(html, 'lxml')
        
        # 从 <img> 标签提取
        for img in soup.find_all('img'):
            src = img.get('src') or img.get('data-src') or img.get('data-lazy-src')
            if src:
                absolute_url = urljoin(base_url, src.strip())
                if absolute_url.startswith(('http://', 'https://')):
                    images.append(absolute_url)
        
        # 从 CSS background-image 提取
        for tag in soup.find_all(style=True):
            style = tag['style']
            urls = re.findall(r'url\(["\']?([^"\'()]+)["\']?\)', style)
            for url in urls:
                absolute_url = urljoin(base_url, url)
                if absolute_url.startswith(('http://', 'https://')):
                    images.append(absolute_url)
        
        return list(set(images))
    
    def _filter_images(self, images: List[str]) -> List[str]:
        """过滤图片 URL"""
        valid = []
        for url in images:
            # 检查扩展名
            parsed = urlparse(url)
            path = parsed.path.lower()
            
            has_valid_ext = any(
                path.endswith(ext) for ext in self._allowed_extensions
            )
            
            # 如果 URL 没有明确的图片扩展名，也尝试下载（可能是动态 URL）
            if has_valid_ext or 'image' in url.lower():
                valid.append(url)
        
        return valid
    
    def _download_image(self, url: str) -> Optional[str]:
        """
        下载单个图片
        
        Args:
            url: 图片 URL
            
        Returns:
            保存的文件路径，失败返回 None
        """
        import requests
        
        try:
            # 发送请求
            response = requests.get(
                url,
                timeout=10,
                stream=True,
                headers={
                    "User-Agent": "SimpleCrawler/1.0"
                }
            )
            
            if response.status_code != 200:
                with self._lock:
                    self._failed_count += 1
                return None
            
            # 检查内容类型
            content_type = response.headers.get('Content-Type', '')
            if not ('image' in content_type or 'octet-stream' in content_type):
                with self._lock:
                    self._failed_count += 1
                return None
            
            # 检查大小
            content_length = int(response.headers.get('Content-Length', 0))
            if content_length > self._max_size:
                with self._lock:
                    self._failed_count += 1
                return None
            
            # 读取内容
            content = response.content
            
            # 生成文件名
            filename = self._generate_filename(url, content)
            filepath = os.path.join(self._save_dir, filename)
            
            # 保存文件
            with open(filepath, 'wb') as f:
                f.write(content)
            
            with self._lock:
                self._downloaded_count += 1
                self._total_bytes += len(content)
            
            return filepath
        
        except Exception as e:
            with self._lock:
                self._failed_count += 1
            return None
    
    def _generate_filename(self, url: str, content: bytes) -> str:
        """生成唯一文件名"""
        # 从 URL 提取原始文件名
        parsed = urlparse(url)
        original_name = os.path.basename(parsed.path)
        
        # 计算内容哈希
        content_hash = hashlib.md5(content).hexdigest()[:8]
        
        if original_name and '.' in original_name:
            name, ext = os.path.splitext(original_name)
            # 清理文件名
            name = ''.join(c for c in name if c.isalnum() or c in '-_')[:30]
            return f"{name}_{content_hash}{ext}"
        else:
            # 猜测扩展名
            ext = self._guess_extension(content)
            return f"image_{content_hash}{ext}"
    
    def _guess_extension(self, content: bytes) -> str:
        """根据内容猜测扩展名"""
        if content[:8] == b'\x89PNG\r\n\x1a\n':
            return '.png'
        elif content[:2] == b'\xff\xd8':
            return '.jpg'
        elif content[:6] in (b'GIF87a', b'GIF89a'):
            return '.gif'
        elif content[:4] == b'RIFF' and content[8:12] == b'WEBP':
            return '.webp'
        elif b'<svg' in content[:1000]:
            return '.svg'
        else:
            return '.unknown'
    
    def stats(self) -> dict:
        """获取统计信息"""
        base_stats = super().stats()
        with self._lock:
            base_stats.update({
                "downloaded_count": self._downloaded_count,
                "failed_count": self._failed_count,
                "total_bytes": self._total_bytes,
                "total_bytes_formatted": self._format_bytes(self._total_bytes),
                "unique_images": len(self._downloaded_urls)
            })
        return base_stats
    
    def _format_bytes(self, size: int) -> str:
        """格式化字节大小"""
        for unit in ["B", "KB", "MB", "GB"]:
            if size < 1024:
                return f"{size:.2f} {unit}"
            size /= 1024
        return f"{size:.2f} TB"

