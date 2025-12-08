"""
爬虫配置模块
"""
from dataclasses import dataclass, field
from typing import List, Set


@dataclass
class CrawlerConfig:
    """爬虫配置类"""
    
    # 基础配置
    max_workers: int = 5                    # 并发下载线程数
    max_depth: int = 3                      # 最大爬取深度
    max_pages: int = 100                    # 最大爬取页面数
    
    # 请求配置
    request_timeout: int = 10               # 请求超时时间（秒）
    request_delay: float = 1.0              # 同一域名请求间隔（秒）
    user_agent: str = "SimpleCrawler/1.0 (Educational Purpose)"
    
    # 存储配置
    storage_dir: str = "./crawled_data"     # 数据存储目录
    content_dir: str = "./crawled_data/content"  # 网页内容存储
    image_dir: str = "./crawled_data/images"     # 图片存储目录
    
    # URL 过滤配置
    allowed_domains: Set[str] = field(default_factory=set)  # 允许的域名（空=全部）
    blocked_domains: Set[str] = field(default_factory=lambda: {
        "facebook.com", "twitter.com", "instagram.com",
        "ads.google.com", "doubleclick.net"
    })
    blocked_extensions: Set[str] = field(default_factory=lambda: {
        ".pdf", ".doc", ".docx", ".xls", ".xlsx",
        ".zip", ".rar", ".tar", ".gz",
        ".mp3", ".mp4", ".avi", ".mov",
        ".exe", ".dmg", ".apk"
    })
    
    # 去重配置
    bloom_filter_capacity: int = 100000     # 布隆过滤器容量
    bloom_filter_error_rate: float = 0.001  # 布隆过滤器错误率
    
    # DNS 缓存配置
    dns_cache_size: int = 1000              # DNS 缓存大小
    dns_cache_ttl: int = 3600               # DNS 缓存过期时间（秒）
    
    # 扩展模块配置
    enable_image_download: bool = True      # 是否下载图片
    enable_web_monitor: bool = True         # 是否启用网页监控
    image_extensions: Set[str] = field(default_factory=lambda: {
        ".png", ".jpg", ".jpeg", ".gif", ".webp", ".svg"
    })


# 默认配置实例
default_config = CrawlerConfig()

