// 方法 3: 使用 std::sync::OnceLock 实现单例模式
// Rust 1.70+ 标准库提供，无需外部依赖
// 这是最推荐的现代方法

use std::sync::{OnceLock, RwLock};
use std::thread;

// 定义日志记录器单例
#[derive(Debug)]
pub struct Logger {
    logs: Vec<String>,
    level: LogLevel,
}

#[derive(Debug, Clone, Copy, PartialEq)]
pub enum LogLevel {
    Debug,
    Info,
    Warning,
    Error,
}

impl Logger {
    fn new() -> Self {
        println!("Initializing Logger singleton...");
        Logger {
            logs: Vec::new(),
            level: LogLevel::Info,
        }
    }

    pub fn log(&mut self, level: LogLevel, message: &str) {
        if level as u8 >= self.level as u8 {
            let log_entry = format!("[{:?}] {}", level, message);
            self.logs.push(log_entry.clone());
            println!("{}", log_entry);
        }
    }

    pub fn set_level(&mut self, level: LogLevel) {
        self.level = level;
        println!("Log level changed to {:?}", level);
    }

    pub fn get_logs(&self) -> &[String] {
        &self.logs
    }

    pub fn get_level(&self) -> LogLevel {
        self.level
    }
}

// 使用 OnceLock 创建全局单例
static LOGGER: OnceLock<RwLock<Logger>> = OnceLock::new();

// 获取单例实例的辅助函数
pub fn logger() -> &'static RwLock<Logger> {
    LOGGER.get_or_init(|| RwLock::new(Logger::new()))
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::thread;

    #[test]
    fn test_logger_initialization() {
        let logger = logger().read().unwrap();
        assert_eq!(logger.get_level(), LogLevel::Info);
    }

    #[test]
    fn test_logger_basic_logging() {
        {
            let mut logger = logger().write().unwrap();
            logger.log(LogLevel::Info, "Test message");
        }

        {
            let logger = logger().read().unwrap();
            assert!(logger.get_logs().len() > 0);
        }
    }

    #[test]
    fn test_logger_level_filtering() {
        {
            let mut logger = logger().write().unwrap();
            logger.set_level(LogLevel::Warning);
            logger.log(LogLevel::Debug, "This should not appear");
            logger.log(LogLevel::Warning, "This should appear");
        }

        {
            let logger = logger().read().unwrap();
            let logs = logger.get_logs();
            assert!(logs.iter().any(|log| log.contains("This should appear")));
        }
    }

    #[test]
    fn test_logger_thread_safety() {
        let handles: Vec<_> = (0..10)
            .map(|i| {
                thread::spawn(move || {
                    let mut logger = logger().write().unwrap();
                    logger.log(LogLevel::Info, &format!("Message from thread {}", i));
                })
            })
            .collect();

        for handle in handles {
            handle.join().unwrap();
        }

        let logger = logger().read().unwrap();
        assert!(logger.get_logs().len() >= 10);
    }

    #[test]
    fn test_logger_concurrent_reads() {
        {
            let mut logger = logger().write().unwrap();
            logger.log(LogLevel::Info, "Concurrent test message");
        }

        let handles: Vec<_> = (0..20)
            .map(|i| {
                thread::spawn(move || {
                    let logger = logger().read().unwrap();
                    let log_count = logger.get_logs().len();
                    println!("Thread {} sees {} logs", i, log_count);
                    assert!(log_count > 0);
                })
            })
            .collect();

        for handle in handles {
            handle.join().unwrap();
        }
    }
}

fn main() {
    println!("=== OnceLock Singleton Pattern ===\n");

    // 首次访问，触发初始化
    {
        let mut logger = logger().write().unwrap();
        logger.log(LogLevel::Info, "Application started");
        logger.log(LogLevel::Debug, "This debug message will not show");
    }

    // 修改日志级别
    {
        let mut logger = logger().write().unwrap();
        logger.set_level(LogLevel::Debug);
    }

    // 现在 Debug 消息会显示
    {
        let mut logger = logger().write().unwrap();
        logger.log(LogLevel::Debug, "This debug message will show");
        logger.log(LogLevel::Info, "Processing request");
        logger.log(LogLevel::Warning, "Low memory warning");
        logger.log(LogLevel::Error, "Connection failed");
    }

    // 多线程日志记录
    println!("\nSpawning threads for concurrent logging...");
    let handles: Vec<_> = (0..5)
        .map(|i| {
            thread::spawn(move || {
                let mut logger = logger().write().unwrap();
                logger.log(LogLevel::Info, &format!("Thread {} completed task", i));
            })
        })
        .collect();

    for handle in handles {
        handle.join().unwrap();
    }

    // 显示所有日志
    {
        let logger = logger().read().unwrap();
        println!("\n=== All Logs ===");
        for (i, log) in logger.get_logs().iter().enumerate() {
            println!("{}. {}", i + 1, log);
        }
        println!("\nTotal logs: {}", logger.get_logs().len());
    }
}
