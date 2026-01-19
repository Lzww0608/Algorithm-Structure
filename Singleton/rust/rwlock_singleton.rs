// 方法 6: 使用 Arc<RwLock<T>> 实现单例模式
// 读写锁允许多个读者或一个写者，提供更好的并发读性能
// 适用于读多写少的场景

use std::sync::{Arc, RwLock, OnceLock};
use std::collections::HashMap;
use std::thread;
use std::time::Duration;

// 定义统计数据收集器
#[derive(Debug)]
pub struct MetricsCollector {
    counters: HashMap<String, u64>,
    gauges: HashMap<String, f64>,
    last_update: std::time::SystemTime,
}

impl MetricsCollector {
    fn new() -> Self {
        println!("Initializing MetricsCollector singleton...");
        MetricsCollector {
            counters: HashMap::new(),
            gauges: HashMap::new(),
            last_update: std::time::SystemTime::now(),
        }
    }

    // 写操作：增加计数器
    pub fn increment_counter(&mut self, name: &str, value: u64) {
        let counter = self.counters.entry(name.to_string()).or_insert(0);
        *counter += value;
        self.last_update = std::time::SystemTime::now();
        println!("Counter '{}' incremented by {}, now: {}", name, value, counter);
    }

    // 写操作：设置仪表
    pub fn set_gauge(&mut self, name: &str, value: f64) {
        self.gauges.insert(name.to_string(), value);
        self.last_update = std::time::SystemTime::now();
        println!("Gauge '{}' set to {}", name, value);
    }

    // 读操作：获取计数器值
    pub fn get_counter(&self, name: &str) -> Option<u64> {
        self.counters.get(name).copied()
    }

    // 读操作：获取仪表值
    pub fn get_gauge(&self, name: &str) -> Option<f64> {
        self.gauges.get(name).copied()
    }

    // 读操作：获取所有计数器
    pub fn get_all_counters(&self) -> HashMap<String, u64> {
        self.counters.clone()
    }

    // 读操作：获取所有仪表
    pub fn get_all_gauges(&self) -> HashMap<String, f64> {
        self.gauges.clone()
    }

    // 读操作：获取最后更新时间
    pub fn get_last_update(&self) -> std::time::SystemTime {
        self.last_update
    }

    // 写操作：重置所有指标
    pub fn reset(&mut self) {
        self.counters.clear();
        self.gauges.clear();
        self.last_update = std::time::SystemTime::now();
        println!("All metrics reset");
    }
}

// 使用 OnceLock + Arc + RwLock 实现单例
static METRICS: OnceLock<Arc<RwLock<MetricsCollector>>> = OnceLock::new();

pub fn metrics() -> Arc<RwLock<MetricsCollector>> {
    METRICS
        .get_or_init(|| Arc::new(RwLock::new(MetricsCollector::new())))
        .clone()
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::thread;
    use std::time::Duration;

    #[test]
    fn test_metrics_counter() {
        let metrics = metrics();
        {
            let mut mc = metrics.write().unwrap();
            mc.increment_counter("requests", 1);
            mc.increment_counter("requests", 5);
        }

        {
            let mc = metrics.read().unwrap();
            assert_eq!(mc.get_counter("requests"), Some(6));
        }
    }

    #[test]
    fn test_metrics_gauge() {
        let metrics = metrics();
        {
            let mut mc = metrics.write().unwrap();
            mc.set_gauge("cpu_usage", 45.5);
        }

        {
            let mc = metrics.read().unwrap();
            assert_eq!(mc.get_gauge("cpu_usage"), Some(45.5));
        }
    }

    #[test]
    fn test_metrics_concurrent_reads() {
        let metrics = metrics();
        {
            let mut mc = metrics.write().unwrap();
            mc.increment_counter("test", 100);
        }

        // 多个线程同时读取（RwLock 允许）
        let handles: Vec<_> = (0..20)
            .map(|i| {
                thread::spawn(move || {
                    let metrics = metrics();
                    let mc = metrics.read().unwrap();
                    let value = mc.get_counter("test");
                    println!("Thread {} read: {:?}", i, value);
                    assert_eq!(value, Some(100));
                })
            })
            .collect();

        for handle in handles {
            handle.join().unwrap();
        }
    }

    #[test]
    fn test_metrics_concurrent_writes() {
        let handles: Vec<_> = (0..10)
            .map(|i| {
                thread::spawn(move || {
                    let metrics = metrics();
                    let mut mc = metrics.write().unwrap();
                    mc.increment_counter("concurrent_writes", 1);
                    println!("Thread {} wrote", i);
                })
            })
            .collect();

        for handle in handles {
            handle.join().unwrap();
        }

        let metrics = metrics();
        let mc = metrics.read().unwrap();
        let count = mc.get_counter("concurrent_writes");
        assert!(count.unwrap_or(0) >= 10);
    }

    #[test]
    fn test_metrics_mixed_operations() {
        let metrics = metrics();
        
        // 写入初始数据
        {
            let mut mc = metrics.write().unwrap();
            mc.increment_counter("mixed", 10);
            mc.set_gauge("temp", 25.5);
        }

        // 混合读写操作
        let handles: Vec<_> = (0..15)
            .map(|i| {
                thread::spawn(move || {
                    let metrics = metrics();
                    if i % 3 == 0 {
                        // 写操作
                        let mut mc = metrics.write().unwrap();
                        mc.increment_counter("mixed", 1);
                    } else {
                        // 读操作
                        let mc = metrics.read().unwrap();
                        let _ = mc.get_counter("mixed");
                        let _ = mc.get_gauge("temp");
                    }
                })
            })
            .collect();

        for handle in handles {
            handle.join().unwrap();
        }
    }

    #[test]
    fn test_metrics_reset() {
        let metrics = metrics();
        {
            let mut mc = metrics.write().unwrap();
            mc.increment_counter("to_reset", 100);
            mc.set_gauge("to_reset_gauge", 50.0);
        }

        {
            let mut mc = metrics.write().unwrap();
            mc.reset();
        }

        {
            let mc = metrics.read().unwrap();
            assert_eq!(mc.get_counter("to_reset"), None);
            assert_eq!(mc.get_gauge("to_reset_gauge"), None);
        }
    }
}

fn main() {
    println!("=== Arc<RwLock<T>> Singleton Pattern ===\n");

    let metrics = metrics();

    // 初始化一些指标
    println!("Initializing metrics...");
    {
        let mut mc = metrics.write().unwrap();
        mc.increment_counter("http_requests", 100);
        mc.increment_counter("errors", 5);
        mc.set_gauge("cpu_usage", 45.2);
        mc.set_gauge("memory_usage", 67.8);
    }

    // 读取指标（多个线程可以同时读取）
    println!("\nReading metrics from multiple threads:");
    let read_handles: Vec<_> = (0..5)
        .map(|i| {
            thread::spawn(move || {
                let metrics = metrics();
                let mc = metrics.read().unwrap();
                println!("Thread {} - Requests: {:?}, CPU: {:?}", 
                         i, 
                         mc.get_counter("http_requests"),
                         mc.get_gauge("cpu_usage"));
                
                // 模拟读取操作
                thread::sleep(Duration::from_millis(50));
            })
        })
        .collect();

    for handle in read_handles {
        handle.join().unwrap();
    }

    // 并发写入（模拟多个请求）
    println!("\nSimulating concurrent requests:");
    let write_handles: Vec<_> = (0..10)
        .map(|i| {
            thread::spawn(move || {
                let metrics = metrics();
                let mut mc = metrics.write().unwrap();
                mc.increment_counter("http_requests", 1);
                if i % 3 == 0 {
                    mc.increment_counter("errors", 1);
                }
                println!("Request {} processed", i);
            })
        })
        .collect();

    for handle in write_handles {
        handle.join().unwrap();
    }

    // 更新仪表值
    println!("\nUpdating gauges:");
    {
        let mut mc = metrics.write().unwrap();
        mc.set_gauge("cpu_usage", 72.5);
        mc.set_gauge("memory_usage", 81.3);
    }

    // 显示最终统计
    println!("\n=== Final Metrics ===");
    {
        let mc = metrics.read().unwrap();
        
        println!("\nCounters:");
        for (name, value) in mc.get_all_counters() {
            println!("  {}: {}", name, value);
        }
        
        println!("\nGauges:");
        for (name, value) in mc.get_all_gauges() {
            println!("  {}: {:.2}", name, value);
        }
        
        println!("\nLast Update: {:?}", mc.get_last_update());
    }

    println!("\nRwLock singleton allows concurrent reads for better performance!");
}
