"""
Web Monitor - 网页监控扩展

功能:
1. 监控网页变化
2. 记录页面状态历史
3. 检测内容更新
"""

import hashlib
import time
import json
from typing import Dict, Optional, List
from dataclasses import dataclass, asdict
from datetime import datetime
from pathlib import Path
from .base import Extension, ExtensionResult


@dataclass
class PageStatus:
    """页面状态记录"""
    url: str
    content_hash: str
    status_code: int
    response_time: float  # 响应时间（秒）
    content_length: int
    check_time: str
    is_available: bool
    title: str = ""
    has_changed: bool = False
    change_type: str = ""  # new, modified, unchanged


class WebMonitor(Extension):
    """
    网页监控器
    
    监控网页的可用性和内容变化
    """
    
    def __init__(self, history_dir: str = "./crawled_data/monitor"):
        """
        初始化监控器
        
        Args:
            history_dir: 历史记录目录
        """
        super().__init__("WebMonitor")
        self._history_dir = Path(history_dir)
        self._history_dir.mkdir(parents=True, exist_ok=True)
        
        # 内存中的最新状态缓存
        self._status_cache: Dict[str, PageStatus] = {}
        
        # 统计
        self._total_checks = 0
        self._available_count = 0
        self._unavailable_count = 0
        self._changed_count = 0
        
        # 加载历史
        self._load_history()
    
    def process(self, url: str, content: str, parsed_data: dict) -> ExtensionResult:
        """
        检查页面状态
        
        Args:
            url: 页面 URL
            content: 页面内容
            parsed_data: 解析后的数据
            
        Returns:
            ExtensionResult
        """
        try:
            self._total_checks += 1
            
            # 计算内容哈希
            content_hash = hashlib.md5(content.encode('utf-8')).hexdigest()
            
            # 获取之前的状态
            previous_status = self._status_cache.get(url)
            
            # 判断变化类型
            if previous_status is None:
                change_type = "new"
                has_changed = True
                self._changed_count += 1
            elif previous_status.content_hash != content_hash:
                change_type = "modified"
                has_changed = True
                self._changed_count += 1
            else:
                change_type = "unchanged"
                has_changed = False
            
            # 创建新状态
            status = PageStatus(
                url=url,
                content_hash=content_hash,
                status_code=parsed_data.get('status_code', 200),
                response_time=parsed_data.get('download_time', 0.0),
                content_length=len(content),
                check_time=datetime.now().isoformat(),
                is_available=True,
                title=parsed_data.get('title', ''),
                has_changed=has_changed,
                change_type=change_type
            )
            
            # 更新缓存
            self._status_cache[url] = status
            self._available_count += 1
            
            # 保存历史
            self._save_status(status)
            
            return ExtensionResult(
                extension_name=self.name,
                success=True,
                data={
                    "status": asdict(status),
                    "previous_hash": previous_status.content_hash if previous_status else None,
                    "current_hash": content_hash,
                    "has_changed": has_changed,
                    "change_type": change_type
                }
            )
        
        except Exception as e:
            self._unavailable_count += 1
            return ExtensionResult(
                extension_name=self.name,
                success=False,
                error_message=str(e)
            )
    
    def check_availability(self, url: str, timeout: int = 10) -> PageStatus:
        """
        检查页面可用性（不下载完整内容）
        
        Args:
            url: 要检查的 URL
            timeout: 超时时间
            
        Returns:
            PageStatus
        """
        import requests
        
        start_time = time.time()
        
        try:
            response = requests.head(
                url,
                timeout=timeout,
                allow_redirects=True,
                headers={"User-Agent": "SimpleCrawler/1.0"}
            )
            
            response_time = time.time() - start_time
            
            status = PageStatus(
                url=url,
                content_hash="",
                status_code=response.status_code,
                response_time=response_time,
                content_length=int(response.headers.get('Content-Length', 0)),
                check_time=datetime.now().isoformat(),
                is_available=response.status_code < 400
            )
            
        except Exception as e:
            status = PageStatus(
                url=url,
                content_hash="",
                status_code=0,
                response_time=time.time() - start_time,
                content_length=0,
                check_time=datetime.now().isoformat(),
                is_available=False
            )
        
        return status
    
    def _load_history(self):
        """加载历史记录"""
        history_file = self._history_dir / "latest_status.json"
        
        if history_file.exists():
            try:
                with open(history_file, 'r', encoding='utf-8') as f:
                    data = json.load(f)
                    for url, status_data in data.items():
                        self._status_cache[url] = PageStatus(**status_data)
            except Exception:
                pass
    
    def _save_status(self, status: PageStatus):
        """保存状态"""
        # 更新最新状态文件
        latest_file = self._history_dir / "latest_status.json"
        
        # 加载现有数据
        existing = {}
        if latest_file.exists():
            try:
                with open(latest_file, 'r', encoding='utf-8') as f:
                    existing = json.load(f)
            except Exception:
                pass
        
        # 更新
        existing[status.url] = asdict(status)
        
        # 保存
        with open(latest_file, 'w', encoding='utf-8') as f:
            json.dump(existing, f, ensure_ascii=False, indent=2)
        
        # 添加到历史日志
        history_log = self._history_dir / "history.jsonl"
        with open(history_log, 'a', encoding='utf-8') as f:
            f.write(json.dumps(asdict(status), ensure_ascii=False) + '\n')
    
    def get_status(self, url: str) -> Optional[PageStatus]:
        """获取 URL 的最新状态"""
        return self._status_cache.get(url)
    
    def get_changed_pages(self) -> List[PageStatus]:
        """获取所有发生变化的页面"""
        return [
            status for status in self._status_cache.values()
            if status.has_changed
        ]
    
    def get_all_statuses(self) -> Dict[str, PageStatus]:
        """获取所有状态"""
        return self._status_cache.copy()
    
    def stats(self) -> dict:
        """获取统计信息"""
        base_stats = super().stats()
        base_stats.update({
            "total_checks": self._total_checks,
            "available_count": self._available_count,
            "unavailable_count": self._unavailable_count,
            "changed_count": self._changed_count,
            "monitored_urls": len(self._status_cache),
            "availability_rate": f"{self._available_count / self._total_checks:.2%}" if self._total_checks > 0 else "N/A"
        })
        return base_stats

