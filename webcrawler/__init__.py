"""
简易网络爬虫系统

一个模块化、可扩展的 Python 网络爬虫实现。

模块结构:
- frontier: URL 调度器
- downloader: HTML 下载器
- dns_resolver: DNS 解析器（带缓存）
- parser: 内容解析器
- dedup: 去重模块（URL + 内容）
- storage: 存储模块
- url_filter: URL 过滤器
- extensions: 扩展模块（链接提取、图片下载、网页监控）
"""

__version__ = "1.0.0"
__author__ = "Web Crawler"

