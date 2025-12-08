"""
HTML Downloader - HTML 下载器

功能:
1. 通过 HTTP/HTTPS 协议下载网页内容
2. 支持自定义请求头
3. 处理各种 HTTP 响应状态
4. 集成 DNS 解析器
"""

import time
import requests
from typing import Optional, Dict, Any
from dataclasses import dataclass
from urllib.parse import urlparse
import threading

from .dns_resolver import DNSResolver


@dataclass
class DownloadResult:
    """下载结果"""
    url: str                            # 原始 URL
    success: bool                       # 是否成功
    status_code: int = 0                # HTTP 状态码
    content: str = ""                   # 网页内容（文本）
    raw_content: bytes = b""            # 原始字节内容
    content_type: str = ""              # 内容类型
    encoding: str = ""                  # 编码
    headers: Dict[str, str] = None      # 响应头
    final_url: str = ""                 # 最终 URL（考虑重定向）
    download_time: float = 0.0          # 下载耗时（秒）
    error_message: str = ""             # 错误信息


class HTMLDownloader:
    """
    HTML 下载器
    
    负责从互联网下载网页内容
    """
    
    def __init__(
        self,
        user_agent: str = "SimpleCrawler/1.0",
        timeout: int = 10,
        dns_resolver: DNSResolver = None
    ):
        """
        初始化下载器
        
        Args:
            user_agent: User-Agent 字符串
            timeout: 请求超时时间（秒）
            dns_resolver: DNS 解析器实例
        """
        self._user_agent = user_agent
        self._timeout = timeout
        self._dns_resolver = dns_resolver or DNSResolver()
        
        # 创建 Session 以复用连接
        self._session = requests.Session()
        self._session.headers.update({
            "User-Agent": user_agent,
            "Accept": "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8",
            "Accept-Language": "zh-CN,zh;q=0.9,en;q=0.8",
            "Accept-Encoding": "gzip, deflate",
            "Connection": "keep-alive"
        })
        
        # 统计数据
        self._lock = threading.Lock()
        self._total_downloads = 0
        self._successful_downloads = 0
        self._failed_downloads = 0
        self._total_bytes = 0
    
    def download(self, url: str, extra_headers: Dict[str, str] = None) -> DownloadResult:
        """
        下载网页
        
        Args:
            url: 要下载的 URL
            extra_headers: 额外的请求头
            
        Returns:
            DownloadResult 对象
        """
        start_time = time.time()
        result = DownloadResult(url=url, success=False, headers={})
        
        try:
            # 预先解析 DNS（用于统计和缓存）
            self._dns_resolver.resolve_url(url)
            
            # 准备请求头
            headers = {}
            if extra_headers:
                headers.update(extra_headers)
            
            # 发送请求
            response = self._session.get(
                url,
                headers=headers,
                timeout=self._timeout,
                allow_redirects=True
            )
            
            # 填充结果
            result.status_code = response.status_code
            result.final_url = response.url
            result.headers = dict(response.headers)
            result.content_type = response.headers.get("Content-Type", "")
            result.encoding = response.encoding or "utf-8"
            result.raw_content = response.content
            
            # 检查状态码
            if response.status_code == 200:
                # 尝试解码内容
                try:
                    result.content = response.text
                except Exception:
                    result.content = response.content.decode("utf-8", errors="ignore")
                
                result.success = True
                
                with self._lock:
                    self._successful_downloads += 1
                    self._total_bytes += len(response.content)
            else:
                result.error_message = f"HTTP 状态码: {response.status_code}"
                with self._lock:
                    self._failed_downloads += 1
        
        except requests.exceptions.Timeout:
            result.error_message = "请求超时"
            with self._lock:
                self._failed_downloads += 1
        
        except requests.exceptions.ConnectionError as e:
            result.error_message = f"连接错误: {str(e)}"
            with self._lock:
                self._failed_downloads += 1
        
        except requests.exceptions.TooManyRedirects:
            result.error_message = "重定向次数过多"
            with self._lock:
                self._failed_downloads += 1
        
        except Exception as e:
            result.error_message = f"下载错误: {str(e)}"
            with self._lock:
                self._failed_downloads += 1
        
        finally:
            result.download_time = time.time() - start_time
            with self._lock:
                self._total_downloads += 1
        
        return result
    
    def download_binary(self, url: str) -> DownloadResult:
        """
        下载二进制内容（如图片）
        
        Args:
            url: 要下载的 URL
            
        Returns:
            DownloadResult 对象（内容在 raw_content 中）
        """
        start_time = time.time()
        result = DownloadResult(url=url, success=False, headers={})
        
        try:
            response = self._session.get(
                url,
                timeout=self._timeout,
                allow_redirects=True,
                stream=True
            )
            
            result.status_code = response.status_code
            result.final_url = response.url
            result.headers = dict(response.headers)
            result.content_type = response.headers.get("Content-Type", "")
            
            if response.status_code == 200:
                result.raw_content = response.content
                result.success = True
                
                with self._lock:
                    self._successful_downloads += 1
                    self._total_bytes += len(response.content)
            else:
                result.error_message = f"HTTP 状态码: {response.status_code}"
                with self._lock:
                    self._failed_downloads += 1
        
        except Exception as e:
            result.error_message = f"下载错误: {str(e)}"
            with self._lock:
                self._failed_downloads += 1
        
        finally:
            result.download_time = time.time() - start_time
            with self._lock:
                self._total_downloads += 1
        
        return result
    
    def stats(self) -> dict:
        """获取下载器统计"""
        with self._lock:
            return {
                "total_downloads": self._total_downloads,
                "successful_downloads": self._successful_downloads,
                "failed_downloads": self._failed_downloads,
                "total_bytes": self._total_bytes,
                "total_bytes_formatted": self._format_bytes(self._total_bytes),
                "success_rate": f"{self._successful_downloads / self._total_downloads:.2%}" if self._total_downloads > 0 else "N/A"
            }
    
    def _format_bytes(self, size: int) -> str:
        """格式化字节大小"""
        for unit in ["B", "KB", "MB", "GB"]:
            if size < 1024:
                return f"{size:.2f} {unit}"
            size /= 1024
        return f"{size:.2f} TB"
    
    def close(self):
        """关闭下载器"""
        self._session.close()

