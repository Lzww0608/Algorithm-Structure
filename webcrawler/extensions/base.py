"""
Extension Base - 扩展模块基类

定义扩展模块的接口和管理器
"""

from abc import ABC, abstractmethod
from typing import Dict, Any, List, Optional
from dataclasses import dataclass
import threading
import queue


@dataclass
class ExtensionResult:
    """扩展模块执行结果"""
    extension_name: str
    success: bool
    data: Any = None
    error_message: str = ""


class Extension(ABC):
    """
    扩展模块基类
    
    所有扩展模块都应继承此类
    """
    
    def __init__(self, name: str):
        """
        初始化扩展
        
        Args:
            name: 扩展名称
        """
        self.name = name
        self._enabled = True
        self._processed_count = 0
        self._error_count = 0
    
    @abstractmethod
    def process(self, url: str, content: str, parsed_data: dict) -> ExtensionResult:
        """
        处理页面数据
        
        Args:
            url: 页面 URL
            content: 页面原始内容
            parsed_data: 解析后的数据
            
        Returns:
            ExtensionResult
        """
        pass
    
    def enable(self):
        """启用扩展"""
        self._enabled = True
    
    def disable(self):
        """禁用扩展"""
        self._enabled = False
    
    @property
    def is_enabled(self) -> bool:
        """检查是否启用"""
        return self._enabled
    
    def stats(self) -> dict:
        """获取统计信息"""
        return {
            "name": self.name,
            "enabled": self._enabled,
            "processed_count": self._processed_count,
            "error_count": self._error_count
        }


class ExtensionManager:
    """
    扩展管理器
    
    管理和调度所有扩展模块
    """
    
    def __init__(self, async_processing: bool = False, max_workers: int = 3):
        """
        初始化扩展管理器
        
        Args:
            async_processing: 是否异步处理（使用队列）
            max_workers: 异步处理的工作线程数
        """
        self._extensions: Dict[str, Extension] = {}
        self._async_processing = async_processing
        self._max_workers = max_workers
        self._task_queue: Optional[queue.Queue] = None
        self._workers: List[threading.Thread] = []
        self._running = False
        self._lock = threading.Lock()
        
        if async_processing:
            self._start_workers()
    
    def register(self, extension: Extension):
        """
        注册扩展模块
        
        Args:
            extension: 扩展模块实例
        """
        with self._lock:
            self._extensions[extension.name] = extension
            print(f"[ExtensionManager] 注册扩展: {extension.name}")
    
    def unregister(self, name: str):
        """
        注销扩展模块
        
        Args:
            name: 扩展名称
        """
        with self._lock:
            if name in self._extensions:
                del self._extensions[name]
                print(f"[ExtensionManager] 注销扩展: {name}")
    
    def process(self, url: str, content: str, parsed_data: dict) -> List[ExtensionResult]:
        """
        处理页面（同步）
        
        Args:
            url: 页面 URL
            content: 页面内容
            parsed_data: 解析后的数据
            
        Returns:
            所有扩展的处理结果列表
        """
        results = []
        
        with self._lock:
            extensions = list(self._extensions.values())
        
        for ext in extensions:
            if ext.is_enabled:
                try:
                    result = ext.process(url, content, parsed_data)
                    ext._processed_count += 1
                    results.append(result)
                except Exception as e:
                    ext._error_count += 1
                    results.append(ExtensionResult(
                        extension_name=ext.name,
                        success=False,
                        error_message=str(e)
                    ))
        
        return results
    
    def process_async(self, url: str, content: str, parsed_data: dict):
        """
        异步处理页面
        
        Args:
            url: 页面 URL
            content: 页面内容
            parsed_data: 解析后的数据
        """
        if not self._async_processing:
            raise RuntimeError("异步处理未启用")
        
        self._task_queue.put((url, content, parsed_data))
    
    def _start_workers(self):
        """启动异步工作线程"""
        self._task_queue = queue.Queue()
        self._running = True
        
        for i in range(self._max_workers):
            worker = threading.Thread(
                target=self._worker_loop,
                name=f"ExtWorker-{i}",
                daemon=True
            )
            worker.start()
            self._workers.append(worker)
    
    def _worker_loop(self):
        """工作线程主循环"""
        while self._running:
            try:
                task = self._task_queue.get(timeout=1)
                if task is None:
                    break
                
                url, content, parsed_data = task
                self.process(url, content, parsed_data)
                self._task_queue.task_done()
            except queue.Empty:
                continue
            except Exception as e:
                print(f"[ExtWorker] 处理错误: {e}")
    
    def stop(self):
        """停止异步处理"""
        self._running = False
        
        if self._task_queue:
            # 发送停止信号
            for _ in self._workers:
                self._task_queue.put(None)
            
            # 等待所有任务完成
            self._task_queue.join()
        
        # 等待工作线程结束
        for worker in self._workers:
            worker.join(timeout=2)
    
    def get_extension(self, name: str) -> Optional[Extension]:
        """获取扩展实例"""
        with self._lock:
            return self._extensions.get(name)
    
    def list_extensions(self) -> List[str]:
        """列出所有扩展"""
        with self._lock:
            return list(self._extensions.keys())
    
    def stats(self) -> dict:
        """获取管理器统计"""
        with self._lock:
            return {
                "total_extensions": len(self._extensions),
                "async_processing": self._async_processing,
                "workers": len(self._workers),
                "queue_size": self._task_queue.qsize() if self._task_queue else 0,
                "extensions": {
                    name: ext.stats() 
                    for name, ext in self._extensions.items()
                }
            }

