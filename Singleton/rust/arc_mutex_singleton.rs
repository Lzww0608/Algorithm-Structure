// 方法 5: 使用 Arc<Mutex<T>> 实现单例模式
// 通过引用计数和互斥锁实现共享可变状态
// 适用于需要在运行时动态创建单例的场景

use std::sync::{Arc, Mutex, OnceLock};
use std::thread;

// 定义缓存管理器
#[derive(Debug)]
pub struct CacheManager {
    cache: std::collections::HashMap<String, String>,
    hits: usize,
    misses: usize,
}

impl CacheManager {
    fn new() -> Self {
        println!("Initializing CacheManager singleton...");
        CacheManager {
            cache: std::collections::HashMap::new(),
            hits: 0,
            misses: 0,
        }
    }

    pub fn get(&mut self, key: &str) -> Option<String> {
        match self.cache.get(key) {
            Some(value) => {
                self.hits += 1;
                println!("Cache HIT for key: {}", key);
                Some(value.clone())
            }
            None => {
                self.misses += 1;
                println!("Cache MISS for key: {}", key);
                None
            }
        }
    }

    pub fn set(&mut self, key: String, value: String) {
        println!("Setting cache: {} = {}", key, value);
        self.cache.insert(key, value);
    }

    pub fn remove(&mut self, key: &str) -> Option<String> {
        self.cache.remove(key)
    }

    pub fn clear(&mut self) {
        self.cache.clear();
        println!("Cache cleared");
    }

    pub fn get_stats(&self) -> (usize, usize) {
        (self.hits, self.misses)
    }

    pub fn hit_rate(&self) -> f64 {
        let total = self.hits + self.misses;
        if total == 0 {
            0.0
        } else {
            self.hits as f64 / total as f64
        }
    }
}

// 使用 OnceLock + Arc + Mutex 实现单例
static CACHE_MANAGER: OnceLock<Arc<Mutex<CacheManager>>> = OnceLock::new();

pub fn cache_manager() -> Arc<Mutex<CacheManager>> {
    CACHE_MANAGER
        .get_or_init(|| Arc::new(Mutex::new(CacheManager::new())))
        .clone()
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::thread;

    #[test]
    fn test_cache_basic_operations() {
        let cache = cache_manager();
        {
            let mut cm = cache.lock().unwrap();
            cm.set("key1".to_string(), "value1".to_string());
        }

        {
            let mut cm = cache.lock().unwrap();
            let value = cm.get("key1");
            assert_eq!(value, Some("value1".to_string()));
        }
    }

    #[test]
    fn test_cache_miss() {
        let cache = cache_manager();
        let mut cm = cache.lock().unwrap();
        let value = cm.get("nonexistent");
        assert_eq!(value, None);
    }

    #[test]
    fn test_cache_stats() {
        let cache = cache_manager();
        {
            let mut cm = cache.lock().unwrap();
            cm.clear(); // 清空以避免其他测试的影响
            cm.set("test".to_string(), "data".to_string());
            cm.get("test"); // hit
            cm.get("missing"); // miss
        }

        {
            let cm = cache.lock().unwrap();
            let (hits, misses) = cm.get_stats();
            assert!(hits > 0);
            assert!(misses > 0);
        }
    }

    #[test]
    fn test_cache_thread_safety() {
        let handles: Vec<_> = (0..10)
            .map(|i| {
                thread::spawn(move || {
                    let cache = cache_manager();
                    let mut cm = cache.lock().unwrap();
                    cm.set(format!("key{}", i), format!("value{}", i));
                    println!("Thread {} set cache entry", i);
                })
            })
            .collect();

        for handle in handles {
            handle.join().unwrap();
        }

        let cache = cache_manager();
        let mut cm = cache.lock().unwrap();
        for i in 0..10 {
            let value = cm.get(&format!("key{}", i));
            assert!(value.is_some());
        }
    }

    #[test]
    fn test_cache_concurrent_access() {
        let cache = cache_manager();
        {
            let mut cm = cache.lock().unwrap();
            cm.set("shared".to_string(), "data".to_string());
        }

        let handles: Vec<_> = (0..20)
            .map(|i| {
                thread::spawn(move || {
                    let cache = cache_manager();
                    let mut cm = cache.lock().unwrap();
                    let value = cm.get("shared");
                    println!("Thread {} got: {:?}", i, value);
                    assert_eq!(value, Some("data".to_string()));
                })
            })
            .collect();

        for handle in handles {
            handle.join().unwrap();
        }
    }

    #[test]
    fn test_cache_remove() {
        let cache = cache_manager();
        {
            let mut cm = cache.lock().unwrap();
            cm.set("temp".to_string(), "data".to_string());
        }

        {
            let mut cm = cache.lock().unwrap();
            let removed = cm.remove("temp");
            assert_eq!(removed, Some("data".to_string()));
        }

        {
            let mut cm = cache.lock().unwrap();
            let value = cm.get("temp");
            assert_eq!(value, None);
        }
    }
}

fn main() {
    println!("=== Arc<Mutex<T>> Singleton Pattern ===\n");

    let cache = cache_manager();

    // 基本操作
    {
        let mut cm = cache.lock().unwrap();
        cm.set("user:1".to_string(), "Alice".to_string());
        cm.set("user:2".to_string(), "Bob".to_string());
        cm.set("user:3".to_string(), "Charlie".to_string());
    }

    // 读取缓存
    println!("\nReading from cache:");
    {
        let mut cm = cache.lock().unwrap();
        cm.get("user:1");
        cm.get("user:2");
        cm.get("user:999"); // miss
    }

    // 多线程写入
    println!("\nMulti-threaded cache operations:");
    let handles: Vec<_> = (0..5)
        .map(|i| {
            let cache_clone = cache_manager();
            thread::spawn(move || {
                let mut cm = cache_clone.lock().unwrap();
                cm.set(format!("thread:{}", i), format!("data{}", i));
                
                // 随机读取
                for j in 0..3 {
                    cm.get(&format!("user:{}", j + 1));
                }
            })
        })
        .collect();

    for handle in handles {
        handle.join().unwrap();
    }

    // 显示统计信息
    {
        let cm = cache.lock().unwrap();
        let (hits, misses) = cm.get_stats();
        println!("\n=== Cache Statistics ===");
        println!("Hits: {}", hits);
        println!("Misses: {}", misses);
        println!("Hit Rate: {:.2}%", cm.hit_rate() * 100.0);
    }

    // 清空缓存
    {
        let mut cm = cache.lock().unwrap();
        cm.clear();
    }

    println!("\nCache cleared successfully!");
}
