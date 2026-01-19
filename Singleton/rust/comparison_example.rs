// 综合对比示例 - 展示不同单例模式的使用场景和性能特点
// 本文件演示如何在一个项目中组合使用不同的单例模式

use std::sync::{Arc, Mutex, RwLock, OnceLock};
use std::time::{Duration, Instant};
use std::thread;

// ============================================================================
// 场景1: 使用 OnceLock 实现全局配置 (推荐方式)
// ============================================================================

#[derive(Debug, Clone)]
struct AppConfig {
    name: String,
    version: String,
    max_threads: usize,
}

impl AppConfig {
    fn new() -> Self {
        println!("Initializing AppConfig...");
        AppConfig {
            name: "MyApp".to_string(),
            version: "1.0.0".to_string(),
            max_threads: 4,
        }
    }
}

static APP_CONFIG: OnceLock<Arc<AppConfig>> = OnceLock::new();

fn app_config() -> Arc<AppConfig> {
    APP_CONFIG
        .get_or_init(|| Arc::new(AppConfig::new()))
        .clone()
}

// ============================================================================
// 场景2: 使用 RwLock 实现读多写少的统计数据
// ============================================================================

#[derive(Debug)]
struct Statistics {
    requests: u64,
    errors: u64,
    total_time: Duration,
}

impl Statistics {
    fn new() -> Self {
        println!("Initializing Statistics...");
        Statistics {
            requests: 0,
            errors: 0,
            total_time: Duration::from_secs(0),
        }
    }

    fn record_request(&mut self, duration: Duration, success: bool) {
        self.requests += 1;
        if !success {
            self.errors += 1;
        }
        self.total_time += duration;
    }

    fn get_summary(&self) -> String {
        let avg_time = if self.requests > 0 {
            self.total_time.as_millis() as f64 / self.requests as f64
        } else {
            0.0
        };
        
        format!(
            "Requests: {}, Errors: {}, Avg Time: {:.2}ms, Error Rate: {:.2}%",
            self.requests,
            self.errors,
            avg_time,
            if self.requests > 0 {
                (self.errors as f64 / self.requests as f64) * 100.0
            } else {
                0.0
            }
        )
    }
}

static STATS: OnceLock<Arc<RwLock<Statistics>>> = OnceLock::new();

fn stats() -> Arc<RwLock<Statistics>> {
    STATS
        .get_or_init(|| Arc::new(RwLock::new(Statistics::new())))
        .clone()
}

// ============================================================================
// 场景3: 使用 Mutex 实现需要频繁写入的缓存
// ============================================================================

#[derive(Debug)]
struct Cache {
    data: std::collections::HashMap<String, String>,
    hits: usize,
    misses: usize,
}

impl Cache {
    fn new() -> Self {
        println!("Initializing Cache...");
        Cache {
            data: std::collections::HashMap::new(),
            hits: 0,
            misses: 0,
        }
    }

    fn get(&mut self, key: &str) -> Option<String> {
        match self.data.get(key) {
            Some(value) => {
                self.hits += 1;
                Some(value.clone())
            }
            None => {
                self.misses += 1;
                None
            }
        }
    }

    fn set(&mut self, key: String, value: String) {
        self.data.insert(key, value);
    }

    fn stats(&self) -> (usize, usize) {
        (self.hits, self.misses)
    }
}

static CACHE: OnceLock<Arc<Mutex<Cache>>> = OnceLock::new();

fn cache() -> Arc<Mutex<Cache>> {
    CACHE
        .get_or_init(|| Arc::new(Mutex::new(Cache::new())))
        .clone()
}

// ============================================================================
// 性能测试函数
// ============================================================================

fn benchmark_read_heavy_workload() {
    println!("\n=== Benchmark: Read-Heavy Workload (RwLock) ===");
    
    // 初始化统计数据
    {
        let mut stats = stats().write().unwrap();
        stats.record_request(Duration::from_millis(10), true);
    }

    let start = Instant::now();
    let threads = 20;
    let reads_per_thread = 10000;

    let handles: Vec<_> = (0..threads)
        .map(|_| {
            thread::spawn(move || {
                for _ in 0..reads_per_thread {
                    let stats = stats().read().unwrap();
                    let _ = stats.requests; // 读取操作
                }
            })
        })
        .collect();

    for handle in handles {
        handle.join().unwrap();
    }

    let elapsed = start.elapsed();
    let total_reads = threads * reads_per_thread;
    println!("Total reads: {}", total_reads);
    println!("Time: {:?}", elapsed);
    println!("Reads per second: {:.0}", total_reads as f64 / elapsed.as_secs_f64());
}

fn benchmark_write_heavy_workload() {
    println!("\n=== Benchmark: Write-Heavy Workload (Mutex) ===");
    
    let start = Instant::now();
    let threads = 10;
    let writes_per_thread = 1000;

    let handles: Vec<_> = (0..threads)
        .map(|i| {
            thread::spawn(move || {
                for j in 0..writes_per_thread {
                    let mut cache = cache().lock().unwrap();
                    cache.set(
                        format!("key_{}_{}", i, j),
                        format!("value_{}_{}", i, j),
                    );
                }
            })
        })
        .collect();

    for handle in handles {
        handle.join().unwrap();
    }

    let elapsed = start.elapsed();
    let total_writes = threads * writes_per_thread;
    println!("Total writes: {}", total_writes);
    println!("Time: {:?}", elapsed);
    println!("Writes per second: {:.0}", total_writes as f64 / elapsed.as_secs_f64());
}

// ============================================================================
// 实际应用示例
// ============================================================================

fn simulate_web_server() {
    println!("\n=== Simulating Web Server ===");
    
    // 读取配置（只读，不需要锁）
    let config = app_config();
    println!("Starting {} v{}", config.name, config.version);
    println!("Max threads: {}", config.max_threads);

    // 模拟请求处理
    let handles: Vec<_> = (0..10)
        .map(|i| {
            thread::spawn(move || {
                // 模拟请求处理时间
                let request_start = Instant::now();
                thread::sleep(Duration::from_millis(10 + (i % 5) * 5));
                let duration = request_start.elapsed();
                let success = i % 7 != 0; // 模拟偶尔失败

                // 记录统计（写操作，但频率不高）
                {
                    let mut stats = stats().write().unwrap();
                    stats.record_request(duration, success);
                }

                // 使用缓存（读写混合）
                {
                    let key = format!("user_{}", i % 3);
                    let mut cache = cache().lock().unwrap();
                    
                    if let Some(value) = cache.get(&key) {
                        println!("Request {}: Cache hit for {}", i, key);
                    } else {
                        println!("Request {}: Cache miss for {}", i, key);
                        cache.set(key.clone(), format!("data_{}", i));
                    }
                }

                if success {
                    println!("Request {} completed successfully", i);
                } else {
                    println!("Request {} failed", i);
                }
            })
        })
        .collect();

    for handle in handles {
        handle.join().unwrap();
    }

    // 显示统计信息
    {
        let stats = stats().read().unwrap();
        println!("\n=== Statistics ===");
        println!("{}", stats.get_summary());
    }

    // 显示缓存统计
    {
        let cache = cache().lock().unwrap();
        let (hits, misses) = cache.stats();
        println!("\n=== Cache Statistics ===");
        println!("Hits: {}, Misses: {}", hits, misses);
        if hits + misses > 0 {
            println!("Hit Rate: {:.2}%", hits as f64 / (hits + misses) as f64 * 100.0);
        }
    }
}

// ============================================================================
// 主函数
// ============================================================================

fn main() {
    println!("========================================");
    println!("Rust Singleton Patterns Comparison Demo");
    println!("========================================");

    // 实际应用示例
    simulate_web_server();

    // 性能测试
    benchmark_read_heavy_workload();
    benchmark_write_heavy_workload();

    println!("\n========================================");
    println!("Summary of Singleton Patterns:");
    println!("========================================");
    println!("1. OnceLock<Arc<T>>     - Immutable config (zero-cost reads)");
    println!("2. OnceLock<Arc<RwLock<T>>> - Read-heavy stats (concurrent reads)");
    println!("3. OnceLock<Arc<Mutex<T>>>  - Write-heavy cache (exclusive access)");
    println!("\nChoose the right pattern for your use case!");
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_config_singleton() {
        let config1 = app_config();
        let config2 = app_config();
        assert_eq!(config1.name, config2.name);
    }

    #[test]
    fn test_stats_concurrent_access() {
        let handles: Vec<_> = (0..10)
            .map(|_| {
                thread::spawn(|| {
                    let mut stats = stats().write().unwrap();
                    stats.record_request(Duration::from_millis(1), true);
                })
            })
            .collect();

        for handle in handles {
            handle.join().unwrap();
        }

        let stats = stats().read().unwrap();
        assert!(stats.requests >= 10);
    }

    #[test]
    fn test_cache_operations() {
        let cache = cache();
        {
            let mut c = cache.lock().unwrap();
            c.set("test".to_string(), "value".to_string());
        }

        {
            let mut c = cache.lock().unwrap();
            let value = c.get("test");
            assert_eq!(value, Some("value".to_string()));
        }
    }
}
