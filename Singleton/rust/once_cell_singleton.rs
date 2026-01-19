// 方法 2: 使用 once_cell::sync::Lazy 实现单例模式
// 这是更现代的方法，性能比 lazy_static 更好
// 需要在 Cargo.toml 中添加: once_cell = "1.19"

use once_cell::sync::Lazy;
use std::sync::Mutex;

// 定义配置单例结构体
#[derive(Debug)]
pub struct Config {
    app_name: String,
    version: String,
    max_connections: usize,
    debug_mode: bool,
}

impl Config {
    fn new() -> Self {
        println!("Initializing Config singleton...");
        Config {
            app_name: String::from("MyApp"),
            version: String::from("1.0.0"),
            max_connections: 100,
            debug_mode: false,
        }
    }

    pub fn get_app_name(&self) -> &str {
        &self.app_name
    }

    pub fn get_version(&self) -> &str {
        &self.version
    }

    pub fn set_debug_mode(&mut self, enabled: bool) {
        self.debug_mode = enabled;
    }

    pub fn is_debug_mode(&self) -> bool {
        self.debug_mode
    }

    pub fn get_max_connections(&self) -> usize {
        self.max_connections
    }
}

// 使用 once_cell::Lazy 创建全局单例
pub static CONFIG: Lazy<Mutex<Config>> = Lazy::new(|| {
    Mutex::new(Config::new())
});

#[cfg(test)]
mod tests {
    use super::*;
    use std::thread;

    #[test]
    fn test_config_initialization() {
        let config = CONFIG.lock().unwrap();
        assert_eq!(config.get_app_name(), "MyApp");
        assert_eq!(config.get_version(), "1.0.0");
        assert_eq!(config.get_max_connections(), 100);
    }

    #[test]
    fn test_config_mutation() {
        {
            let mut config = CONFIG.lock().unwrap();
            config.set_debug_mode(true);
        }

        {
            let config = CONFIG.lock().unwrap();
            assert_eq!(config.is_debug_mode(), true);
        }
    }

    #[test]
    fn test_config_thread_safety() {
        let handles: Vec<_> = (0..20)
            .map(|i| {
                thread::spawn(move || {
                    let mut config = CONFIG.lock().unwrap();
                    config.set_debug_mode(i % 2 == 0);
                    let mode = config.is_debug_mode();
                    println!("Thread {}: debug_mode = {}", i, mode);
                })
            })
            .collect();

        for handle in handles {
            handle.join().unwrap();
        }
    }

    #[test]
    fn test_config_read_concurrency() {
        let handles: Vec<_> = (0..10)
            .map(|i| {
                thread::spawn(move || {
                    let config = CONFIG.lock().unwrap();
                    println!("Thread {} - App: {}, Version: {}", 
                             i, config.get_app_name(), config.get_version());
                    assert_eq!(config.get_app_name(), "MyApp");
                })
            })
            .collect();

        for handle in handles {
            handle.join().unwrap();
        }
    }
}

fn main() {
    println!("=== Once Cell Lazy Singleton Pattern ===\n");

    // 首次访问，触发初始化
    {
        let config = CONFIG.lock().unwrap();
        println!("App Name: {}", config.get_app_name());
        println!("Version: {}", config.get_version());
        println!("Max Connections: {}", config.get_max_connections());
        println!("Debug Mode: {}", config.is_debug_mode());
    }

    // 修改配置
    {
        let mut config = CONFIG.lock().unwrap();
        config.set_debug_mode(true);
        println!("\nDebug mode enabled!");
    }

    // 多线程读取
    println!("\nSpawning threads to read config...");
    let handles: Vec<_> = (0..5)
        .map(|i| {
            thread::spawn(move || {
                let config = CONFIG.lock().unwrap();
                println!("Thread {}: Debug mode is {}", i, config.is_debug_mode());
            })
        })
        .collect();

    for handle in handles {
        handle.join().unwrap();
    }

    println!("\nSingleton pattern with once_cell::Lazy completed!");
}
