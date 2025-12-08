"""
URL Filter - URL 过滤器

功能:
1. 过滤黑名单域名
2. 过滤不需要的文件类型
3. 验证 URL 格式
4. 支持自定义过滤规则
"""

import re
from typing import Set, List, Callable, Optional
from urllib.parse import urlparse, urljoin
from dataclasses import dataclass


@dataclass
class FilterResult:
    """过滤结果"""
    url: str
    passed: bool
    reason: str = ""


class URLFilter:
    """
    URL 过滤器
    
    用于过滤不需要爬取的 URL
    """
    
    def __init__(
        self,
        allowed_domains: Set[str] = None,
        blocked_domains: Set[str] = None,
        blocked_extensions: Set[str] = None,
        max_url_length: int = 2048
    ):
        """
        初始化 URL 过滤器
        
        Args:
            allowed_domains: 允许的域名（空集合=允许所有）
            blocked_domains: 黑名单域名
            blocked_extensions: 不允许的文件扩展名
            max_url_length: 最大 URL 长度
        """
        self._allowed_domains = allowed_domains or set()
        self._blocked_domains = blocked_domains or {
            "facebook.com", "twitter.com", "instagram.com",
            "linkedin.com", "pinterest.com",
            "ads.google.com", "doubleclick.net", "googleadservices.com",
            "googlesyndication.com", "google-analytics.com"
        }
        self._blocked_extensions = blocked_extensions or {
            ".pdf", ".doc", ".docx", ".xls", ".xlsx", ".ppt", ".pptx",
            ".zip", ".rar", ".tar", ".gz", ".7z",
            ".mp3", ".mp4", ".avi", ".mov", ".wmv", ".flv",
            ".exe", ".dmg", ".apk", ".msi",
            ".css", ".js"  # 通常不需要爬取的资源
        }
        self._max_url_length = max_url_length
        
        # 自定义过滤器
        self._custom_filters: List[Callable[[str], Optional[str]]] = []
        
        # 统计
        self._total_checked = 0
        self._total_passed = 0
        self._total_blocked = 0
        self._block_reasons = {}
    
    def add_custom_filter(self, filter_func: Callable[[str], Optional[str]]):
        """
        添加自定义过滤器
        
        Args:
            filter_func: 过滤函数，接收 URL，返回 None（通过）或拒绝原因字符串
        """
        self._custom_filters.append(filter_func)
    
    def filter(self, url: str) -> FilterResult:
        """
        过滤单个 URL
        
        Args:
            url: 要过滤的 URL
            
        Returns:
            FilterResult 对象
        """
        self._total_checked += 1
        
        # 1. 基础验证
        if not url or not isinstance(url, str):
            return self._block(url, "空 URL")
        
        url = url.strip()
        
        # 2. 长度检查
        if len(url) > self._max_url_length:
            return self._block(url, "URL 过长")
        
        # 3. 格式验证
        if not self._is_valid_url(url):
            return self._block(url, "无效 URL 格式")
        
        # 4. 解析 URL
        try:
            parsed = urlparse(url)
        except Exception:
            return self._block(url, "URL 解析失败")
        
        # 5. 协议检查
        if parsed.scheme not in ('http', 'https'):
            return self._block(url, f"不支持的协议: {parsed.scheme}")
        
        # 6. 域名检查
        domain = parsed.netloc.lower()
        
        # 检查允许列表
        if self._allowed_domains:
            if not self._domain_matches(domain, self._allowed_domains):
                return self._block(url, "域名不在允许列表")
        
        # 检查黑名单
        if self._domain_matches(domain, self._blocked_domains):
            return self._block(url, "域名在黑名单")
        
        # 7. 扩展名检查
        path = parsed.path.lower()
        for ext in self._blocked_extensions:
            if path.endswith(ext):
                return self._block(url, f"扩展名被阻止: {ext}")
        
        # 8. 特殊路径过滤
        blocked_patterns = [
            r'/wp-admin/',
            r'/admin/',
            r'/login',
            r'/logout',
            r'/signup',
            r'/register',
            r'#',           # 锚点
            r'\?.*=.*&.*=.*&.*=',  # 过多参数
        ]
        for pattern in blocked_patterns:
            if re.search(pattern, url, re.IGNORECASE):
                return self._block(url, f"匹配阻止模式: {pattern}")
        
        # 9. 自定义过滤器
        for custom_filter in self._custom_filters:
            reason = custom_filter(url)
            if reason:
                return self._block(url, f"自定义过滤: {reason}")
        
        # 通过所有检查
        self._total_passed += 1
        return FilterResult(url=url, passed=True)
    
    def filter_batch(self, urls: List[str]) -> List[str]:
        """
        批量过滤 URL
        
        Args:
            urls: URL 列表
            
        Returns:
            通过过滤的 URL 列表
        """
        return [url for url in urls if self.filter(url).passed]
    
    def _is_valid_url(self, url: str) -> bool:
        """验证 URL 格式"""
        pattern = re.compile(
            r'^https?://'  # http:// 或 https://
            r'(?:(?:[A-Z0-9](?:[A-Z0-9-]{0,61}[A-Z0-9])?\.)+[A-Z]{2,6}\.?|'  # 域名
            r'localhost|'  # localhost
            r'\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3})'  # IP
            r'(?::\d+)?'  # 可选端口
            r'(?:/?|[/?]\S+)$', re.IGNORECASE)
        return bool(pattern.match(url))
    
    def _domain_matches(self, domain: str, domain_set: Set[str]) -> bool:
        """检查域名是否匹配集合中的任一项"""
        # 移除 www 前缀
        if domain.startswith('www.'):
            domain = domain[4:]
        
        for blocked in domain_set:
            blocked = blocked.lower()
            if blocked.startswith('www.'):
                blocked = blocked[4:]
            
            # 精确匹配或子域名匹配
            if domain == blocked or domain.endswith('.' + blocked):
                return True
        
        return False
    
    def _block(self, url: str, reason: str) -> FilterResult:
        """记录阻止并返回结果"""
        self._total_blocked += 1
        self._block_reasons[reason] = self._block_reasons.get(reason, 0) + 1
        return FilterResult(url=url, passed=False, reason=reason)
    
    def add_blocked_domain(self, domain: str):
        """添加黑名单域名"""
        self._blocked_domains.add(domain.lower())
    
    def add_allowed_domain(self, domain: str):
        """添加允许域名"""
        self._allowed_domains.add(domain.lower())
    
    def stats(self) -> dict:
        """获取统计信息"""
        return {
            "total_checked": self._total_checked,
            "total_passed": self._total_passed,
            "total_blocked": self._total_blocked,
            "pass_rate": f"{self._total_passed / self._total_checked:.2%}" if self._total_checked > 0 else "N/A",
            "block_reasons": dict(sorted(
                self._block_reasons.items(), 
                key=lambda x: x[1], 
                reverse=True
            )[:10])  # Top 10 阻止原因
        }


class RobotsTxtParser:
    """
    robots.txt 解析器
    
    解析和应用 robots.txt 规则
    """
    
    def __init__(self, user_agent: str = "*"):
        """
        初始化解析器
        
        Args:
            user_agent: User-Agent 标识
        """
        self._user_agent = user_agent
        self._rules: dict = {}  # domain -> (disallow_patterns, allow_patterns, crawl_delay)
    
    def parse(self, domain: str, robots_content: str):
        """
        解析 robots.txt 内容
        
        Args:
            domain: 域名
            robots_content: robots.txt 内容
        """
        disallow = []
        allow = []
        crawl_delay = None
        current_agent = None
        
        for line in robots_content.split('\n'):
            line = line.strip()
            
            # 跳过注释和空行
            if not line or line.startswith('#'):
                continue
            
            # 解析指令
            if ':' in line:
                key, value = line.split(':', 1)
                key = key.strip().lower()
                value = value.strip()
                
                if key == 'user-agent':
                    current_agent = value
                elif current_agent in (self._user_agent, '*'):
                    if key == 'disallow' and value:
                        disallow.append(value)
                    elif key == 'allow' and value:
                        allow.append(value)
                    elif key == 'crawl-delay':
                        try:
                            crawl_delay = float(value)
                        except ValueError:
                            pass
        
        self._rules[domain] = (disallow, allow, crawl_delay)
    
    def is_allowed(self, url: str) -> bool:
        """
        检查 URL 是否允许爬取
        
        Args:
            url: URL
            
        Returns:
            是否允许
        """
        parsed = urlparse(url)
        domain = parsed.netloc.lower()
        path = parsed.path
        
        if domain not in self._rules:
            return True  # 没有规则时默认允许
        
        disallow, allow, _ = self._rules[domain]
        
        # 检查 allow（优先级高）
        for pattern in allow:
            if self._path_matches(path, pattern):
                return True
        
        # 检查 disallow
        for pattern in disallow:
            if self._path_matches(path, pattern):
                return False
        
        return True
    
    def get_crawl_delay(self, domain: str) -> Optional[float]:
        """获取爬取延迟"""
        if domain in self._rules:
            return self._rules[domain][2]
        return None
    
    def _path_matches(self, path: str, pattern: str) -> bool:
        """检查路径是否匹配模式"""
        # 简单的前缀匹配
        if pattern.endswith('*'):
            return path.startswith(pattern[:-1])
        elif pattern.endswith('$'):
            return path == pattern[:-1]
        else:
            return path.startswith(pattern)

