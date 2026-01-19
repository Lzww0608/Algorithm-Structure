// 方法 1: 使用 lazy_static! 宏实现单例模式
// 这是最常见和最简单的方法
// 需要在 Cargo.toml 中添加: lazy_static = "1.4"

use std::sync::Mutex;

// 定义单例结构体
pub struct Database {
    connection_count: usize,
    name: String,
}

impl Database {
    fn new() -> Self {
        println!("Initializing Database singleton...");
        Database {
            connection_count: 0,
            name: String::from("PostgreSQL"),
        }
    }

    pub fn connect(&mut self) {
        self.connection_count += 1;
        println!("Connection #{} established to {}", self.connection_count, self.name);
    }

    pub fn get_connection_count(&self) -> usize {
        self.connection_count
    }

    pub fn get_name(&self) -> &str {
        &self.name
    }
}

// 使用 lazy_static! 创建全局单例
lazy_static::lazy_static! {
    pub static ref DATABASE: Mutex<Database> = Mutex::new(Database::new());
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::thread;
    use std::sync::Arc;

    #[test]
    fn test_singleton_initialization() {
        let db = DATABASE.lock().unwrap();
        assert_eq!(db.get_name(), "PostgreSQL");
    }

    #[test]
    fn test_singleton_thread_safety() {
        let handles: Vec<_> = (0..10)
            .map(|i| {
                thread::spawn(move || {
                    let mut db = DATABASE.lock().unwrap();
                    db.connect();
                    println!("Thread {} accessed singleton", i);
                })
            })
            .collect();

        for handle in handles {
            handle.join().unwrap();
        }

        let db = DATABASE.lock().unwrap();
        assert_eq!(db.get_connection_count(), 10);
    }

    #[test]
    fn test_singleton_same_instance() {
        {
            let mut db = DATABASE.lock().unwrap();
            db.connect();
        }

        {
            let db = DATABASE.lock().unwrap();
            assert!(db.get_connection_count() >= 1);
        }
    }
}

fn main() {
    println!("=== Lazy Static Singleton Pattern ===\n");

    // 第一次访问时初始化
    {
        let mut db = DATABASE.lock().unwrap();
        db.connect();
    }

    // 多线程访问
    let handles: Vec<_> = (0..5)
        .map(|i| {
            thread::spawn(move || {
                let mut db = DATABASE.lock().unwrap();
                db.connect();
                println!("Thread {} - Total connections: {}", i, db.get_connection_count());
            })
        })
        .collect();

    for handle in handles {
        handle.join().unwrap();
    }

    // 验证最终状态
    let db = DATABASE.lock().unwrap();
    println!("\nFinal connection count: {}", db.get_connection_count());
    println!("Database name: {}", db.get_name());
}
