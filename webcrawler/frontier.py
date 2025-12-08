"""
URL Frontier - URL 调度器/边界模块

功能:
1. 管理待抓取的 URL 队列
2. 实现优先级排序
3. 实现礼貌策略（Politeness）- 控制同一域名的请求频率
"""

import time
import heapq
import threading
from collections import defaultdict
from dataclasses import dataclass, field
from typing import Optional, Dict, Set
from urllib.parse import urlparse


@dataclass(order=True)
class URLItem:
    """URL 队列项"""
    priority: int                           # 优先级（越小越高）
    timestamp: float = field(compare=False) # 加入时间戳
    url: str = field(compare=False)         # URL
    depth: int = field(compare=False)       # 爬取深度
    parent_url: str = field(compare=False, default="")  # 来源页面


class URLFrontier:
    """
    URL 边界/调度器
    
    使用优先级队列管理待抓取的 URL，并实现礼貌策略。
    """
    
    def __init__(self, politeness_delay: float = 1.0):
        """
        初始化 URL Frontier
        
        Args:
            politeness_delay: 同一域名的请求间隔（秒）
        """
        self._queue: list = []                          # 优先级队列（最小堆）
        self._lock = threading.Lock()                   # 线程锁
        self._politeness_delay = politeness_delay       # 礼貌策略延迟
        self._domain_last_access: Dict[str, float] = defaultdict(float)  # 域名最后访问时间
        self._in_queue: Set[str] = set()                # 当前队列中的 URL
        self._total_added = 0                           # 累计添加数量
        self._total_fetched = 0                         # 累计取出数量
    
    def add(self, url: str, depth: int = 0, priority: int = 5, parent_url: str = "") -> bool:
        """
        添加 URL 到队列
        
        Args:
            url: 要添加的 URL
            depth: 爬取深度
            priority: 优先级（1-10，越小越高）
            parent_url: 来源页面 URL
            
        Returns:
            是否成功添加
        """
        with self._lock:
            # 检查是否已在队列中
            if url in self._in_queue:
                return False
            
            item = URLItem(
                priority=priority,
                timestamp=time.time(),
                url=url,
                depth=depth,
                parent_url=parent_url
            )
            heapq.heappush(self._queue, item)
            self._in_queue.add(url)
            self._total_added += 1
            return True
    
    def add_batch(self, urls: list, depth: int = 0, priority: int = 5, parent_url: str = "") -> int:
        """
        批量添加 URL
        
        Args:
            urls: URL 列表
            depth: 爬取深度
            priority: 优先级
            parent_url: 来源页面
            
        Returns:
            成功添加的数量
        """
        count = 0
        for url in urls:
            if self.add(url, depth, priority, parent_url):
                count += 1
        return count
    
    def get(self, timeout: float = None) -> Optional[URLItem]:
        """
        获取下一个待抓取的 URL（考虑礼貌策略）
        
        Args:
            timeout: 超时时间（秒）
            
        Returns:
            URLItem 或 None
        """
        start_time = time.time()
        
        while True:
            with self._lock:
                if not self._queue:
                    return None
                
                # 查找满足礼貌策略的 URL
                temp_items = []
                result = None
                
                while self._queue:
                    item = heapq.heappop(self._queue)
                    domain = self._get_domain(item.url)
                    
                    # 检查礼貌策略
                    last_access = self._domain_last_access[domain]
                    time_since_last = time.time() - last_access
                    
                    if time_since_last >= self._politeness_delay:
                        # 满足礼貌策略，可以抓取
                        self._domain_last_access[domain] = time.time()
                        self._in_queue.discard(item.url)
                        self._total_fetched += 1
                        result = item
                        break
                    else:
                        # 不满足，暂存后继续查找
                        temp_items.append(item)
                
                # 将暂存的 URL 放回队列
                for temp_item in temp_items:
                    heapq.heappush(self._queue, temp_item)
                
                if result:
                    return result
            
            # 检查超时
            if timeout is not None and time.time() - start_time > timeout:
                return None
            
            # 短暂等待后重试
            time.sleep(0.1)
    
    def _get_domain(self, url: str) -> str:
        """提取 URL 的域名"""
        try:
            parsed = urlparse(url)
            return parsed.netloc.lower()
        except Exception:
            return ""
    
    def is_empty(self) -> bool:
        """检查队列是否为空"""
        with self._lock:
            return len(self._queue) == 0
    
    def size(self) -> int:
        """获取队列大小"""
        with self._lock:
            return len(self._queue)
    
    def stats(self) -> dict:
        """获取统计信息"""
        with self._lock:
            return {
                "queue_size": len(self._queue),
                "total_added": self._total_added,
                "total_fetched": self._total_fetched,
                "domains_tracked": len(self._domain_last_access)
            }
    
    def clear(self):
        """清空队列"""
        with self._lock:
            self._queue.clear()
            self._in_queue.clear()
            self._domain_last_access.clear()

