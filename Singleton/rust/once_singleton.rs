// 方法 4: 使用 std::sync::Once 实现单例模式
// 底层同步原语，更灵活但需要更多手动管理
// 适用于需要精确控制初始化的场景

use std::sync::{Once, Mutex};
use std::mem::MaybeUninit;
use std::thread;

// 定义连接池单例
#[derive(Debug)]
pub struct ConnectionPool {
    active_connections: usize,
    max_connections: usize,
    pool_name: String,
}

impl ConnectionPool {
    fn new(name: &str, max_conn: usize) -> Self {
        println!("Initializing ConnectionPool '{}' with max {} connections", name, max_conn);
        ConnectionPool {
            active_connections: 0,
            max_connections: max_conn,
            pool_name: name.to_string(),
        }
    }

    pub fn acquire_connection(&mut self) -> Result<(), String> {
        if self.active_connections < self.max_connections {
            self.active_connections += 1;
            Ok(())
        } else {
            Err(format!("Connection pool '{}' is full", self.pool_name))
        }
    }

    pub fn release_connection(&mut self) {
        if self.active_connections > 0 {
            self.active_connections -= 1;
        }
    }

    pub fn get_active_connections(&self) -> usize {
        self.active_connections
    }

    pub fn get_pool_name(&self) -> &str {
        &self.pool_name
    }
}

// 使用 Once 和 MaybeUninit 实现单例
static INIT: Once = Once::new();
static mut POOL: MaybeUninit<Mutex<ConnectionPool>> = MaybeUninit::uninit();

// 获取单例实例
pub fn connection_pool() -> &'static Mutex<ConnectionPool> {
    unsafe {
        INIT.call_once(|| {
            let pool = ConnectionPool::new("MainPool", 10);
            POOL.write(Mutex::new(pool));
        });
        POOL.assume_init_ref()
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::thread;

    #[test]
    fn test_pool_initialization() {
        let pool = connection_pool().lock().unwrap();
        assert_eq!(pool.get_pool_name(), "MainPool");
        assert_eq!(pool.get_active_connections(), 0);
    }

    #[test]
    fn test_pool_acquire_release() {
        {
            let mut pool = connection_pool().lock().unwrap();
            assert!(pool.acquire_connection().is_ok());
            assert_eq!(pool.get_active_connections(), 1);
        }

        {
            let mut pool = connection_pool().lock().unwrap();
            pool.release_connection();
            assert_eq!(pool.get_active_connections(), 0);
        }
    }

    #[test]
    fn test_pool_max_connections() {
        {
            let mut pool = connection_pool().lock().unwrap();
            // 清空连接
            while pool.get_active_connections() > 0 {
                pool.release_connection();
            }

            // 填满连接池
            for _ in 0..10 {
                assert!(pool.acquire_connection().is_ok());
            }

            // 尝试超过限制
            assert!(pool.acquire_connection().is_err());
        }
    }

    #[test]
    fn test_pool_thread_safety() {
        // 清空连接池
        {
            let mut pool = connection_pool().lock().unwrap();
            while pool.get_active_connections() > 0 {
                pool.release_connection();
            }
        }

        let handles: Vec<_> = (0..5)
            .map(|i| {
                thread::spawn(move || {
                    let mut pool = connection_pool().lock().unwrap();
                    match pool.acquire_connection() {
                        Ok(_) => println!("Thread {} acquired connection", i),
                        Err(e) => println!("Thread {} failed: {}", i, e),
                    }
                })
            })
            .collect();

        for handle in handles {
            handle.join().unwrap();
        }

        let pool = connection_pool().lock().unwrap();
        assert!(pool.get_active_connections() <= 10);
    }

    #[test]
    fn test_pool_concurrent_acquire_release() {
        let handles: Vec<_> = (0..20)
            .map(|i| {
                thread::spawn(move || {
                    for _ in 0..10 {
                        {
                            let mut pool = connection_pool().lock().unwrap();
                            let _ = pool.acquire_connection();
                        }
                        
                        std::thread::sleep(std::time::Duration::from_micros(10));
                        
                        {
                            let mut pool = connection_pool().lock().unwrap();
                            pool.release_connection();
                        }
                    }
                    println!("Thread {} completed", i);
                })
            })
            .collect();

        for handle in handles {
            handle.join().unwrap();
        }
    }
}

fn main() {
    println!("=== Once Singleton Pattern ===\n");

    // 首次访问，触发初始化
    {
        let pool = connection_pool().lock().unwrap();
        println!("Pool Name: {}", pool.get_pool_name());
        println!("Active Connections: {}", pool.get_active_connections());
    }

    // 获取连接
    println!("\nAcquiring connections...");
    for i in 0..5 {
        let mut pool = connection_pool().lock().unwrap();
        match pool.acquire_connection() {
            Ok(_) => println!("Connection {} acquired successfully", i + 1),
            Err(e) => println!("Failed to acquire connection: {}", e),
        }
    }

    {
        let pool = connection_pool().lock().unwrap();
        println!("\nCurrent active connections: {}", pool.get_active_connections());
    }

    // 多线程访问
    println!("\nSpawning threads for concurrent access...");
    let handles: Vec<_> = (0..10)
        .map(|i| {
            thread::spawn(move || {
                {
                    let mut pool = connection_pool().lock().unwrap();
                    match pool.acquire_connection() {
                        Ok(_) => println!("Thread {} acquired connection", i),
                        Err(e) => println!("Thread {} failed: {}", i, e),
                    }
                }

                std::thread::sleep(std::time::Duration::from_millis(100));

                {
                    let mut pool = connection_pool().lock().unwrap();
                    pool.release_connection();
                    println!("Thread {} released connection", i);
                }
            })
        })
        .collect();

    for handle in handles {
        handle.join().unwrap();
    }

    // 最终状态
    {
        let pool = connection_pool().lock().unwrap();
        println!("\nFinal active connections: {}", pool.get_active_connections());
    }
}
