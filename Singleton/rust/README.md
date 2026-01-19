# Rust 线程安全单例模式 - 工业级实现

本目录包含了 Rust 中所有主流的**线程安全单例模式**的工业级实现。

## 📦 包含的实现

| 文件 | 方法 | 推荐度 | 说明 |
|------|------|--------|------|
| `once_lock_singleton.rs` | `std::sync::OnceLock` | ⭐⭐⭐⭐⭐ | **最推荐** - 标准库，零依赖 |
| `rwlock_singleton.rs` | `Arc<RwLock<T>>` | ⭐⭐⭐⭐ | 读多写少场景 |
| `arc_mutex_singleton.rs` | `Arc<Mutex<T>>` | ⭐⭐⭐⭐ | 通用方案 |
| `once_cell_singleton.rs` | `once_cell::Lazy` | ⭐⭐⭐⭐ | 需要外部依赖 |
| `once_singleton.rs` | `std::sync::Once` | ⭐⭐⭐ | 底层实现 |
| `lazy_static_singleton.rs` | `lazy_static!` | ⭐⭐⭐ | 传统方案 |
| `comparison_example.rs` | 综合示例 | - | 对比所有方法 |

## 🚀 快速开始

### 1. 编译运行（标准库实现 - 无需依赖）

```bash
# 编译并运行 OnceLock 示例（推荐）
rustc --edition 2021 once_lock_singleton.rs && ./once_lock_singleton

# 其他标准库实现
rustc --edition 2021 once_singleton.rs && ./once_singleton
rustc --edition 2021 arc_mutex_singleton.rs && ./arc_mutex_singleton
rustc --edition 2021 rwlock_singleton.rs && ./rwlock_singleton
rustc --edition 2021 comparison_example.rs && ./comparison_example
```

### 2. 运行测试

```bash
# 编译并运行测试
rustc --edition 2021 --test once_lock_singleton.rs && ./once_lock_singleton
```

### 3. 使用 Cargo（生产环境推荐）

```bash
# 运行示例
cargo run --bin once_lock_demo

# 运行测试
cargo test

# 运行所有测试
cargo test --all
```

## 📊 快速选择指南

### 按场景选择

```rust
// 场景1: 只读配置（推荐）
use std::sync::OnceLock;
static CONFIG: OnceLock<Config> = OnceLock::new();

// 场景2: 读多写少（统计、监控）
use std::sync::{Arc, RwLock, OnceLock};
static STATS: OnceLock<Arc<RwLock<Stats>>> = OnceLock::new();

// 场景3: 频繁写入（缓存、连接池）
use std::sync::{Arc, Mutex, OnceLock};
static CACHE: OnceLock<Arc<Mutex<Cache>>> = OnceLock::new();
```

### 性能对比

| 方法 | 初始化 | 并发读 | 并发写 | 适用场景 |
|------|--------|--------|--------|----------|
| `OnceLock` | ★★★★★ | ★★★★★ | - | 只读配置 |
| `Arc<RwLock<T>>` | ★★★★☆ | ★★★★★ | ★★★☆☆ | 读多写少 |
| `Arc<Mutex<T>>` | ★★★★☆ | ★★★☆☆ | ★★★★☆ | 读写混合 |

## 💡 最佳实践示例

### 示例 1: 应用配置（OnceLock）

```rust
use std::sync::{Arc, OnceLock};

#[derive(Clone)]
struct Config {
    database_url: String,
    api_key: String,
}

static CONFIG: OnceLock<Arc<Config>> = OnceLock::new();

pub fn config() -> Arc<Config> {
    CONFIG.get_or_init(|| {
        Arc::new(Config {
            database_url: std::env::var("DATABASE_URL").unwrap(),
            api_key: std::env::var("API_KEY").unwrap(),
        })
    }).clone()
}

// 使用
fn main() {
    let cfg = config();
    println!("Database: {}", cfg.database_url);
}
```

### 示例 2: 统计系统（RwLock）

```rust
use std::sync::{Arc, RwLock, OnceLock};

struct Stats {
    requests: u64,
    errors: u64,
}

static STATS: OnceLock<Arc<RwLock<Stats>>> = OnceLock::new();

pub fn stats() -> Arc<RwLock<Stats>> {
    STATS.get_or_init(|| {
        Arc::new(RwLock::new(Stats { requests: 0, errors: 0 }))
    }).clone()
}

// 使用
fn main() {
    // 写入（较少）
    stats().write().unwrap().requests += 1;
    
    // 读取（频繁，可并发）
    let count = stats().read().unwrap().requests;
}
```

### 示例 3: 缓存系统（Mutex）

```rust
use std::sync::{Arc, Mutex, OnceLock};
use std::collections::HashMap;

struct Cache {
    data: HashMap<String, String>,
}

static CACHE: OnceLock<Arc<Mutex<Cache>>> = OnceLock::new();

pub fn cache() -> Arc<Mutex<Cache>> {
    CACHE.get_or_init(|| {
        Arc::new(Mutex::new(Cache { data: HashMap::new() }))
    }).clone()
}

// 使用
fn main() {
    let mut cache = cache().lock().unwrap();
    cache.data.insert("key".to_string(), "value".to_string());
}
```

## ⚠️ 常见陷阱

### 1. 避免死锁

```rust
// ❌ 错误：持有锁时调用其他函数
{
    let data = singleton().lock().unwrap();
    other_function(); // 可能也会获取同一个锁！
}

// ✅ 正确：及时释放锁
{
    let value = {
        let data = singleton().lock().unwrap();
        data.get_value()
    }; // 锁在这里释放
    
    other_function();
}
```

### 2. 错误处理

```rust
// ❌ 避免 unwrap()
let data = singleton().lock().unwrap();

// ✅ 正确处理
let data = singleton().lock()
    .expect("Failed to acquire lock");
```

## 🎯 推荐配置

**生产环境首选（Rust >= 1.70）：**

```rust
use std::sync::{OnceLock, RwLock};

static INSTANCE: OnceLock<RwLock<MyType>> = OnceLock::new();

pub fn instance() -> &'static RwLock<MyType> {
    INSTANCE.get_or_init(|| RwLock::new(MyType::new()))
}
```

**优势：**
- ✅ 零外部依赖
- ✅ 最佳性能
- ✅ 类型安全
- ✅ 线程安全

---
