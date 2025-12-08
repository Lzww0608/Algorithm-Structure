"""
Storage Module - 存储模块

功能:
1. 内容存储 - 保存网页内容
2. URL 存储 - 保存访问过的 URL 历史
3. 元数据存储 - 保存爬取元数据
"""

import os
import json
import time
import hashlib
import threading
from typing import Optional, Dict, Any, List
from dataclasses import dataclass, asdict
from datetime import datetime
from pathlib import Path


@dataclass
class StoredPage:
    """存储的页面数据"""
    url: str                        # 原始 URL
    final_url: str                  # 最终 URL
    title: str                      # 页面标题
    content_hash: str               # 内容哈希
    content_file: str               # 内容文件路径
    crawl_time: str                 # 爬取时间
    status_code: int                # HTTP 状态码
    content_type: str               # 内容类型
    content_length: int             # 内容长度
    depth: int                      # 爬取深度
    parent_url: str = ""            # 来源 URL
    word_count: int = 0             # 字数
    links_count: int = 0            # 链接数量
    images_count: int = 0           # 图片数量


class Storage:
    """
    统一存储管理器
    
    管理内容存储、URL 历史和元数据
    """
    
    def __init__(self, base_dir: str = "./crawled_data"):
        """
        初始化存储管理器
        
        Args:
            base_dir: 存储基础目录
        """
        self._base_dir = Path(base_dir)
        self._content_dir = self._base_dir / "content"
        self._image_dir = self._base_dir / "images"
        self._meta_dir = self._base_dir / "meta"
        
        # 创建目录
        self._content_dir.mkdir(parents=True, exist_ok=True)
        self._image_dir.mkdir(parents=True, exist_ok=True)
        self._meta_dir.mkdir(parents=True, exist_ok=True)
        
        # URL 历史文件
        self._url_history_file = self._meta_dir / "url_history.jsonl"
        self._crawl_log_file = self._meta_dir / "crawl_log.jsonl"
        
        # 统计数据
        self._lock = threading.Lock()
        self._pages_stored = 0
        self._images_stored = 0
        self._total_content_size = 0
    
    def save_page(
        self,
        url: str,
        content: str,
        title: str = "",
        final_url: str = "",
        status_code: int = 200,
        content_type: str = "",
        depth: int = 0,
        parent_url: str = "",
        word_count: int = 0,
        links_count: int = 0,
        images_count: int = 0
    ) -> StoredPage:
        """
        保存网页内容
        
        Args:
            url: 原始 URL
            content: 网页内容
            title: 页面标题
            final_url: 最终 URL
            status_code: HTTP 状态码
            content_type: 内容类型
            depth: 爬取深度
            parent_url: 来源 URL
            word_count: 字数
            links_count: 链接数量
            images_count: 图片数量
            
        Returns:
            StoredPage 对象
        """
        # 计算内容哈希
        content_hash = hashlib.md5(content.encode('utf-8')).hexdigest()
        
        # 生成文件名
        safe_filename = self._make_safe_filename(url)
        content_file = self._content_dir / f"{safe_filename}_{content_hash[:8]}.html"
        
        # 保存内容
        with open(content_file, 'w', encoding='utf-8') as f:
            f.write(content)
        
        # 创建存储记录
        stored_page = StoredPage(
            url=url,
            final_url=final_url or url,
            title=title,
            content_hash=content_hash,
            content_file=str(content_file),
            crawl_time=datetime.now().isoformat(),
            status_code=status_code,
            content_type=content_type,
            content_length=len(content),
            depth=depth,
            parent_url=parent_url,
            word_count=word_count,
            links_count=links_count,
            images_count=images_count
        )
        
        # 保存元数据
        self._save_metadata(stored_page)
        
        # 更新统计
        with self._lock:
            self._pages_stored += 1
            self._total_content_size += len(content)
        
        return stored_page
    
    def save_image(self, url: str, content: bytes, filename: str = None) -> str:
        """
        保存图片
        
        Args:
            url: 图片 URL
            content: 图片二进制内容
            filename: 可选的文件名
            
        Returns:
            保存的文件路径
        """
        if not filename:
            # 从 URL 提取文件名
            from urllib.parse import urlparse
            parsed = urlparse(url)
            filename = os.path.basename(parsed.path)
            if not filename or '.' not in filename:
                # 使用哈希作为文件名
                content_hash = hashlib.md5(content).hexdigest()[:12]
                ext = self._guess_image_extension(url, content)
                filename = f"{content_hash}{ext}"
        
        # 确保文件名唯一
        file_path = self._image_dir / filename
        if file_path.exists():
            name, ext = os.path.splitext(filename)
            content_hash = hashlib.md5(content).hexdigest()[:8]
            filename = f"{name}_{content_hash}{ext}"
            file_path = self._image_dir / filename
        
        # 保存图片
        with open(file_path, 'wb') as f:
            f.write(content)
        
        # 更新统计
        with self._lock:
            self._images_stored += 1
            self._total_content_size += len(content)
        
        return str(file_path)
    
    def _make_safe_filename(self, url: str) -> str:
        """生成安全的文件名"""
        from urllib.parse import urlparse
        
        parsed = urlparse(url)
        
        # 组合域名和路径
        safe_name = f"{parsed.netloc}{parsed.path}"
        
        # 移除或替换不安全字符
        safe_chars = []
        for char in safe_name:
            if char.isalnum() or char in '-_.':
                safe_chars.append(char)
            elif char == '/':
                safe_chars.append('_')
        
        result = ''.join(safe_chars)
        
        # 限制长度
        if len(result) > 100:
            result = result[:100]
        
        return result or "page"
    
    def _guess_image_extension(self, url: str, content: bytes) -> str:
        """猜测图片扩展名"""
        # 优先从 URL 判断
        url_lower = url.lower()
        for ext in ['.png', '.jpg', '.jpeg', '.gif', '.webp', '.svg']:
            if ext in url_lower:
                return ext
        
        # 从内容魔数判断
        if content[:8] == b'\x89PNG\r\n\x1a\n':
            return '.png'
        elif content[:2] == b'\xff\xd8':
            return '.jpg'
        elif content[:6] in (b'GIF87a', b'GIF89a'):
            return '.gif'
        elif content[:4] == b'RIFF' and content[8:12] == b'WEBP':
            return '.webp'
        
        return '.unknown'
    
    def _save_metadata(self, stored_page: StoredPage):
        """保存元数据到 JSONL 文件"""
        with self._lock:
            with open(self._url_history_file, 'a', encoding='utf-8') as f:
                f.write(json.dumps(asdict(stored_page), ensure_ascii=False) + '\n')
    
    def log_crawl_event(self, event_type: str, data: dict):
        """
        记录爬取事件日志
        
        Args:
            event_type: 事件类型
            data: 事件数据
        """
        log_entry = {
            "timestamp": datetime.now().isoformat(),
            "event_type": event_type,
            "data": data
        }
        
        with self._lock:
            with open(self._crawl_log_file, 'a', encoding='utf-8') as f:
                f.write(json.dumps(log_entry, ensure_ascii=False) + '\n')
    
    def get_crawled_urls(self) -> List[str]:
        """获取所有已爬取的 URL"""
        urls = []
        if self._url_history_file.exists():
            with open(self._url_history_file, 'r', encoding='utf-8') as f:
                for line in f:
                    try:
                        data = json.loads(line.strip())
                        urls.append(data['url'])
                    except Exception:
                        pass
        return urls
    
    def stats(self) -> dict:
        """获取存储统计"""
        with self._lock:
            return {
                "pages_stored": self._pages_stored,
                "images_stored": self._images_stored,
                "total_content_size": self._total_content_size,
                "total_content_size_formatted": self._format_bytes(self._total_content_size),
                "storage_directory": str(self._base_dir)
            }
    
    def _format_bytes(self, size: int) -> str:
        """格式化字节大小"""
        for unit in ["B", "KB", "MB", "GB"]:
            if size < 1024:
                return f"{size:.2f} {unit}"
            size /= 1024
        return f"{size:.2f} TB"


class URLStorage:
    """
    URL 存储器
    
    专门用于存储和查询 URL 历史
    """
    
    def __init__(self, storage_file: str = "./crawled_data/meta/urls.jsonl"):
        """
        初始化 URL 存储器
        
        Args:
            storage_file: 存储文件路径
        """
        self._storage_file = Path(storage_file)
        self._storage_file.parent.mkdir(parents=True, exist_ok=True)
        self._lock = threading.Lock()
        self._cached_urls: set = set()
        self._load_existing()
    
    def _load_existing(self):
        """加载已存在的 URL"""
        if self._storage_file.exists():
            with open(self._storage_file, 'r', encoding='utf-8') as f:
                for line in f:
                    try:
                        data = json.loads(line.strip())
                        self._cached_urls.add(data.get('url', ''))
                    except Exception:
                        pass
    
    def add(self, url: str, metadata: dict = None):
        """
        添加 URL 记录
        
        Args:
            url: URL
            metadata: 额外元数据
        """
        record = {
            "url": url,
            "timestamp": datetime.now().isoformat()
        }
        if metadata:
            record.update(metadata)
        
        with self._lock:
            self._cached_urls.add(url)
            with open(self._storage_file, 'a', encoding='utf-8') as f:
                f.write(json.dumps(record, ensure_ascii=False) + '\n')
    
    def contains(self, url: str) -> bool:
        """检查 URL 是否存在"""
        with self._lock:
            return url in self._cached_urls
    
    def count(self) -> int:
        """获取 URL 数量"""
        with self._lock:
            return len(self._cached_urls)

