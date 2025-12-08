"""
Deduplication Module - 去重模块

功能:
1. URL 去重 - 检查 URL 是否已访问
2. 内容去重 - 检查内容是否已存储（基于指纹/哈希）
3. 使用布隆过滤器实现高效查找
"""

import hashlib
import threading
from typing import Set, Optional
from abc import ABC, abstractmethod


class SimpleBloomFilter:
    """
    简易布隆过滤器实现
    
    当 pybloom_live 不可用时使用此实现
    """
    
    def __init__(self, capacity: int = 100000, error_rate: float = 0.001):
        """
        初始化布隆过滤器
        
        Args:
            capacity: 预期元素数量
            error_rate: 目标错误率
        """
        import math
        
        self.capacity = capacity
        self.error_rate = error_rate
        
        # 计算最优的位数组大小和哈希函数数量
        self.size = int(-capacity * math.log(error_rate) / (math.log(2) ** 2))
        self.num_hashes = int(self.size / capacity * math.log(2))
        
        # 使用 bytearray 存储位
        self.bit_array = bytearray((self.size + 7) // 8)
        self.count = 0
        self._lock = threading.Lock()
    
    def _get_hash_values(self, item: str) -> list:
        """计算多个哈希值"""
        hash_values = []
        for i in range(self.num_hashes):
            # 使用不同的盐值生成多个哈希
            hash_input = f"{item}_{i}".encode('utf-8')
            hash_value = int(hashlib.md5(hash_input).hexdigest(), 16) % self.size
            hash_values.append(hash_value)
        return hash_values
    
    def _set_bit(self, position: int):
        """设置指定位置的位"""
        byte_index = position // 8
        bit_index = position % 8
        self.bit_array[byte_index] |= (1 << bit_index)
    
    def _get_bit(self, position: int) -> bool:
        """获取指定位置的位"""
        byte_index = position // 8
        bit_index = position % 8
        return bool(self.bit_array[byte_index] & (1 << bit_index))
    
    def add(self, item: str):
        """添加元素"""
        with self._lock:
            hash_values = self._get_hash_values(item)
            for pos in hash_values:
                self._set_bit(pos)
            self.count += 1
    
    def __contains__(self, item: str) -> bool:
        """检查元素是否可能存在"""
        with self._lock:
            hash_values = self._get_hash_values(item)
            return all(self._get_bit(pos) for pos in hash_values)
    
    def __len__(self) -> int:
        return self.count


class BaseDeduplicator(ABC):
    """去重器基类"""
    
    @abstractmethod
    def is_seen(self, item: str) -> bool:
        """检查项目是否已见过"""
        pass
    
    @abstractmethod
    def mark_seen(self, item: str):
        """标记项目为已见"""
        pass
    
    @abstractmethod
    def stats(self) -> dict:
        """获取统计信息"""
        pass


class URLDeduplicator(BaseDeduplicator):
    """
    URL 去重器
    
    使用布隆过滤器实现高效的 URL 去重
    """
    
    def __init__(self, capacity: int = 100000, error_rate: float = 0.001):
        """
        初始化 URL 去重器
        
        Args:
            capacity: 预期 URL 数量
            error_rate: 布隆过滤器错误率
        """
        # 尝试使用 pybloom_live，否则使用简易实现
        try:
            from pybloom_live import BloomFilter
            self._bloom = BloomFilter(capacity=capacity, error_rate=error_rate)
            self._using_pybloom = True
        except ImportError:
            self._bloom = SimpleBloomFilter(capacity=capacity, error_rate=error_rate)
            self._using_pybloom = False
        
        self._lock = threading.Lock()
        self._seen_count = 0
        self._check_count = 0
        
        # 同时维护一个精确集合（用于小规模场景）
        self._exact_set: Set[str] = set()
        self._use_exact = capacity <= 50000  # 小规模时使用精确集合
    
    def _normalize_url(self, url: str) -> str:
        """规范化 URL"""
        url = url.strip().lower()
        
        # 移除尾部斜杠
        if url.endswith('/'):
            url = url[:-1]
        
        # 移除常见的追踪参数
        from urllib.parse import urlparse, parse_qs, urlencode, urlunparse
        try:
            parsed = urlparse(url)
            query_params = parse_qs(parsed.query)
            
            # 移除追踪参数
            tracking_params = {'utm_source', 'utm_medium', 'utm_campaign', 
                             'utm_content', 'utm_term', 'fbclid', 'gclid'}
            filtered_params = {k: v for k, v in query_params.items() 
                             if k.lower() not in tracking_params}
            
            new_query = urlencode(filtered_params, doseq=True)
            url = urlunparse((
                parsed.scheme, parsed.netloc, parsed.path,
                parsed.params, new_query, ''
            ))
        except Exception:
            pass
        
        return url
    
    def is_seen(self, url: str) -> bool:
        """
        检查 URL 是否已访问
        
        Args:
            url: 要检查的 URL
            
        Returns:
            是否已见过
        """
        normalized = self._normalize_url(url)
        
        with self._lock:
            self._check_count += 1
            
            if self._use_exact:
                return normalized in self._exact_set
            else:
                return normalized in self._bloom
    
    def mark_seen(self, url: str):
        """
        标记 URL 为已访问
        
        Args:
            url: 要标记的 URL
        """
        normalized = self._normalize_url(url)
        
        with self._lock:
            if self._use_exact:
                if normalized not in self._exact_set:
                    self._exact_set.add(normalized)
                    self._seen_count += 1
            else:
                if normalized not in self._bloom:
                    self._bloom.add(normalized)
                    self._seen_count += 1
    
    def check_and_mark(self, url: str) -> bool:
        """
        检查并标记 URL（原子操作）
        
        Args:
            url: URL
            
        Returns:
            True 如果是新的 URL，False 如果已存在
        """
        normalized = self._normalize_url(url)
        
        with self._lock:
            self._check_count += 1
            
            if self._use_exact:
                if normalized in self._exact_set:
                    return False
                self._exact_set.add(normalized)
            else:
                if normalized in self._bloom:
                    return False
                self._bloom.add(normalized)
            
            self._seen_count += 1
            return True
    
    def stats(self) -> dict:
        """获取统计信息"""
        with self._lock:
            return {
                "seen_count": self._seen_count,
                "check_count": self._check_count,
                "implementation": "pybloom" if self._using_pybloom else "simple",
                "mode": "exact" if self._use_exact else "bloom_filter"
            }


class ContentDeduplicator(BaseDeduplicator):
    """
    内容去重器
    
    使用内容指纹（哈希）检测重复内容
    """
    
    def __init__(self, capacity: int = 100000, error_rate: float = 0.001):
        """
        初始化内容去重器
        
        Args:
            capacity: 预期内容数量
            error_rate: 布隆过滤器错误率
        """
        try:
            from pybloom_live import BloomFilter
            self._bloom = BloomFilter(capacity=capacity, error_rate=error_rate)
        except ImportError:
            self._bloom = SimpleBloomFilter(capacity=capacity, error_rate=error_rate)
        
        self._lock = threading.Lock()
        self._seen_count = 0
        self._check_count = 0
        self._duplicate_count = 0
    
    def _compute_fingerprint(self, content: str) -> str:
        """
        计算内容指纹
        
        使用 SimHash 或简单哈希生成内容指纹
        """
        # 预处理：移除空白字符，转小写
        normalized = ''.join(content.lower().split())
        
        # 计算 SHA256 哈希作为指纹
        fingerprint = hashlib.sha256(normalized.encode('utf-8')).hexdigest()
        
        return fingerprint
    
    def is_seen(self, content: str) -> bool:
        """
        检查内容是否已存储
        
        Args:
            content: 要检查的内容
            
        Returns:
            是否已见过
        """
        fingerprint = self._compute_fingerprint(content)
        
        with self._lock:
            self._check_count += 1
            return fingerprint in self._bloom
    
    def mark_seen(self, content: str) -> str:
        """
        标记内容为已存储
        
        Args:
            content: 要标记的内容
            
        Returns:
            内容指纹
        """
        fingerprint = self._compute_fingerprint(content)
        
        with self._lock:
            self._bloom.add(fingerprint)
            self._seen_count += 1
        
        return fingerprint
    
    def check_and_mark(self, content: str) -> tuple:
        """
        检查并标记内容（原子操作）
        
        Args:
            content: 内容
            
        Returns:
            (is_new, fingerprint) - 是否为新内容，内容指纹
        """
        fingerprint = self._compute_fingerprint(content)
        
        with self._lock:
            self._check_count += 1
            
            if fingerprint in self._bloom:
                self._duplicate_count += 1
                return False, fingerprint
            
            self._bloom.add(fingerprint)
            self._seen_count += 1
            return True, fingerprint
    
    def stats(self) -> dict:
        """获取统计信息"""
        with self._lock:
            return {
                "seen_count": self._seen_count,
                "check_count": self._check_count,
                "duplicate_count": self._duplicate_count,
                "duplicate_rate": f"{self._duplicate_count / self._check_count:.2%}" if self._check_count > 0 else "N/A"
            }


class Deduplicator:
    """
    综合去重器
    
    整合 URL 去重和内容去重
    """
    
    def __init__(self, capacity: int = 100000, error_rate: float = 0.001):
        self.url_dedup = URLDeduplicator(capacity, error_rate)
        self.content_dedup = ContentDeduplicator(capacity, error_rate)
    
    def is_url_seen(self, url: str) -> bool:
        """检查 URL 是否已见"""
        return self.url_dedup.is_seen(url)
    
    def mark_url_seen(self, url: str):
        """标记 URL 为已见"""
        self.url_dedup.mark_seen(url)
    
    def is_content_seen(self, content: str) -> bool:
        """检查内容是否已见"""
        return self.content_dedup.is_seen(content)
    
    def mark_content_seen(self, content: str) -> str:
        """标记内容为已见"""
        return self.content_dedup.mark_seen(content)
    
    def stats(self) -> dict:
        """获取综合统计"""
        return {
            "url_dedup": self.url_dedup.stats(),
            "content_dedup": self.content_dedup.stats()
        }

