package urlshortener

import (
	"crypto/sha256"
	"errors"
	"math/big"
	"slices"
	"sync"
)

const (
	base62Chars      = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
	shortURLLength   = 7
	predefinedSuffix = "_collision_salt"
	maxRetries       = 10
)

var (
	ErrURLNotFound   = errors.New("short URL not found")
	ErrMaxRetries    = errors.New("max retries reached for collision resolution")
	ErrInvalidURL    = errors.New("invalid URL")
	ErrAlreadyExists = errors.New("URL already shortened")
)

// Storage 定义URL存储接口
type Storage interface {
	// Exists 检查shortURL是否存在
	Exists(shortURL string) bool
	// Save 保存URL映射
	Save(shortURL, longURL string) error
	// Get 根据shortURL获取longURL
	Get(shortURL string) (string, error)
	// GetByLongURL 根据longURL获取shortURL（可选，用于避免重复）
	GetByLongURL(longURL string) (string, bool)
}

// MemoryStorage 内存存储实现
type MemoryStorage struct {
	mu          sync.RWMutex
	shortToLong map[string]string
	longToShort map[string]string
}

// NewMemoryStorage 创建内存存储实例
func NewMemoryStorage() *MemoryStorage {
	return &MemoryStorage{
		shortToLong: make(map[string]string),
		longToShort: make(map[string]string),
	}
}

func (m *MemoryStorage) Exists(shortURL string) bool {
	m.mu.RLock()
	defer m.mu.RUnlock()
	_, exists := m.shortToLong[shortURL]
	return exists
}

func (m *MemoryStorage) Save(shortURL, longURL string) error {
	m.mu.Lock()
	defer m.mu.Unlock()
	m.shortToLong[shortURL] = longURL
	m.longToShort[longURL] = shortURL
	return nil
}

func (m *MemoryStorage) Get(shortURL string) (string, error) {
	m.mu.RLock()
	defer m.mu.RUnlock()
	if longURL, exists := m.shortToLong[shortURL]; exists {
		return longURL, nil
	}
	return "", ErrURLNotFound
}

func (m *MemoryStorage) GetByLongURL(longURL string) (string, bool) {
	m.mu.RLock()
	defer m.mu.RUnlock()
	shortURL, exists := m.longToShort[longURL]
	return shortURL, exists
}

// URLShortener URL短链接服务
type URLShortener struct {
	storage Storage
	baseURL string // 短链接的基础URL，如 "https://short.url/"
}

// NewURLShortener 创建URLShortener实例
func NewURLShortener(storage Storage, baseURL string) *URLShortener {
	return &URLShortener{
		storage: storage,
		baseURL: baseURL,
	}
}

// hash2base62 将hash字节转换为base62编码
func hash2base62(hash []byte) string {
	n := new(big.Int)
	n.SetBytes(hash)

	if n.Sign() == 0 {
		return "0"
	}

	base := big.NewInt(62)
	rem := new(big.Int)
	var str []byte

	for n.Sign() > 0 {
		n.DivMod(n, base, rem)
		str = append(str, base62Chars[rem.Int64()])
	}

	slices.Reverse(str)
	return string(str)
}

// hashURL 对URL进行hash处理
func hashURL(url string) []byte {
	hash := sha256.Sum256([]byte(url))
	return hash[:]
}

// generateShortKey 生成短链接key
func generateShortKey(url string) string {
	hash := hashURL(url)
	base62 := hash2base62(hash)

	// 截取指定长度
	if len(base62) > shortURLLength {
		return base62[:shortURLLength]
	}
	return base62
}

// Shorten 将长URL转换为短URL
// 1. 输入 longURL
// 2. 通过 hash function 生成 shortURL
// 3. 检查是否存在于数据库
// 4. 如果存在冲突，添加 predefined string 后重新 hash
// 5. 不存在冲突则保存到数据库
func (s *URLShortener) Shorten(longURL string) (string, error) {
	if longURL == "" {
		return "", ErrInvalidURL
	}

	// 检查是否已经存在该长URL的映射
	if existingShort, exists := s.storage.GetByLongURL(longURL); exists {
		return s.baseURL + existingShort, nil
	}

	currentURL := longURL
	var shortKey string

	for i := 0; i < maxRetries; i++ {
		// Step 2: hash function -> shortURL
		shortKey = generateShortKey(currentURL)

		// Step 3: exist in DB?
		if !s.storage.Exists(shortKey) {
			// Step 5: no collision -> save to DB
			if err := s.storage.Save(shortKey, longURL); err != nil {
				return "", err
			}
			return s.baseURL + shortKey, nil
		}

		// Step 4: has collision -> longURL + predefined string
		// 检查是否是同一个URL（已存在）
		existingLongURL, _ := s.storage.Get(shortKey)
		if existingLongURL == longURL {
			return s.baseURL + shortKey, nil
		}

		// 真正的冲突：添加predefined string后重新hash
		currentURL = longURL + predefinedSuffix + string(rune('0'+i))
	}

	return "", ErrMaxRetries
}

// Resolve 根据短URL获取原始长URL
func (s *URLShortener) Resolve(shortURL string) (string, error) {
	// 移除baseURL前缀
	shortKey := shortURL
	if len(shortURL) > len(s.baseURL) && shortURL[:len(s.baseURL)] == s.baseURL {
		shortKey = shortURL[len(s.baseURL):]
	}

	return s.storage.Get(shortKey)
}

// Stats 返回存储统计信息（用于调试）
func (s *URLShortener) Stats() (total int) {
	if ms, ok := s.storage.(*MemoryStorage); ok {
		ms.mu.RLock()
		defer ms.mu.RUnlock()
		return len(ms.shortToLong)
	}
	return 0
}
