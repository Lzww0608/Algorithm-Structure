"""
扩展模块包

包含可插拔的扩展模块：
- LinkExtractor: 链接提取器
- ImageDownloader: 图片下载器
- WebMonitor: 网页监控器
"""

from .base import Extension, ExtensionManager
from .link_extractor import LinkExtractor
from .image_downloader import ImageDownloader
from .web_monitor import WebMonitor

__all__ = [
    'Extension',
    'ExtensionManager',
    'LinkExtractor',
    'ImageDownloader',
    'WebMonitor'
]

