"""
DNS Resolver - DNS 解析器（带缓存）

功能:
1. 将域名解析为 IP 地址
2. 实现 DNS 缓存以减少查询延迟
3. 支持 TTL 过期机制
"""

import socket
import time
import threading
from typing import Optional, Dict, Tuple
from collections import OrderedDict


class DNSCache:
    """
    LRU DNS 缓存
    
    使用 OrderedDict 实现 LRU 缓存策略
    """
    
    def __init__(self, max_size: int = 1000, ttl: int = 3600):
        """
        初始化 DNS 缓存
        
        Args:
            max_size: 最大缓存条目数
            ttl: 缓存过期时间（秒）
        """
        self._cache: OrderedDict[str, Tuple[str, float]] = OrderedDict()
        self._max_size = max_size
        self._ttl = ttl
        self._lock = threading.Lock()
        self._hits = 0
        self._misses = 0
    
    def get(self, domain: str) -> Optional[str]:
        """
        从缓存获取 IP 地址
        
        Args:
            domain: 域名
            
        Returns:
            IP 地址或 None（如果未缓存或已过期）
        """
        with self._lock:
            if domain not in self._cache:
                self._misses += 1
                return None
            
            ip, timestamp = self._cache[domain]
            
            # 检查是否过期
            if time.time() - timestamp > self._ttl:
                del self._cache[domain]
                self._misses += 1
                return None
            
            # 移动到末尾（LRU）
            self._cache.move_to_end(domain)
            self._hits += 1
            return ip
    
    def set(self, domain: str, ip: str):
        """
        设置缓存条目
        
        Args:
            domain: 域名
            ip: IP 地址
        """
        with self._lock:
            # 如果已存在，更新并移到末尾
            if domain in self._cache:
                self._cache.move_to_end(domain)
            
            self._cache[domain] = (ip, time.time())
            
            # 检查缓存大小，淘汰最旧的条目
            while len(self._cache) > self._max_size:
                self._cache.popitem(last=False)
    
    def clear(self):
        """清空缓存"""
        with self._lock:
            self._cache.clear()
    
    def stats(self) -> dict:
        """获取缓存统计"""
        with self._lock:
            total = self._hits + self._misses
            hit_rate = self._hits / total if total > 0 else 0
            return {
                "size": len(self._cache),
                "max_size": self._max_size,
                "hits": self._hits,
                "misses": self._misses,
                "hit_rate": f"{hit_rate:.2%}"
            }


class DNSResolver:
    """
    DNS 解析器
    
    支持带缓存的 DNS 解析
    """
    
    def __init__(self, cache_size: int = 1000, cache_ttl: int = 3600):
        """
        初始化 DNS 解析器
        
        Args:
            cache_size: DNS 缓存大小
            cache_ttl: DNS 缓存 TTL（秒）
        """
        self._cache = DNSCache(max_size=cache_size, ttl=cache_ttl)
        self._resolve_lock = threading.Lock()
        self._total_resolves = 0
    
    def resolve(self, domain: str) -> Optional[str]:
        """
        解析域名为 IP 地址
        
        Args:
            domain: 域名
            
        Returns:
            IP 地址或 None（如果解析失败）
        """
        # 先尝试从缓存获取
        cached_ip = self._cache.get(domain)
        if cached_ip:
            return cached_ip
        
        # 缓存未命中，进行 DNS 查询
        with self._resolve_lock:
            # 双重检查（可能其他线程已经解析）
            cached_ip = self._cache.get(domain)
            if cached_ip:
                return cached_ip
            
            try:
                # 执行 DNS 解析
                ip = socket.gethostbyname(domain)
                self._cache.set(domain, ip)
                self._total_resolves += 1
                return ip
            except socket.gaierror as e:
                print(f"[DNS] 解析失败: {domain} - {e}")
                return None
            except Exception as e:
                print(f"[DNS] 未知错误: {domain} - {e}")
                return None
    
    def resolve_url(self, url: str) -> Optional[str]:
        """
        从 URL 中提取域名并解析
        
        Args:
            url: 完整 URL
            
        Returns:
            IP 地址或 None
        """
        from urllib.parse import urlparse
        try:
            parsed = urlparse(url)
            domain = parsed.netloc
            # 移除端口号
            if ':' in domain:
                domain = domain.split(':')[0]
            return self.resolve(domain)
        except Exception:
            return None
    
    def stats(self) -> dict:
        """获取解析器统计"""
        cache_stats = self._cache.stats()
        cache_stats["total_resolves"] = self._total_resolves
        return cache_stats
    
    def clear_cache(self):
        """清空 DNS 缓存"""
        self._cache.clear()

