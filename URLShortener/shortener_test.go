package urlshortener

import (
	"strings"
	"testing"
)

func TestURLShortener_Shorten(t *testing.T) {
	storage := NewMemoryStorage()
	shortener := NewURLShortener(storage, "https://short.url/")

	tests := []struct {
		name    string
		longURL string
		wantErr error
	}{
		{
			name:    "valid URL",
			longURL: "https://www.example.com/very/long/path/to/resource?query=param",
			wantErr: nil,
		},
		{
			name:    "another valid URL",
			longURL: "https://github.com/Lzww0608/guuid",
			wantErr: nil,
		},
		{
			name:    "empty URL",
			longURL: "",
			wantErr: ErrInvalidURL,
		},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			shortURL, err := shortener.Shorten(tt.longURL)

			if err != tt.wantErr {
				t.Errorf("Shorten() error = %v, wantErr %v", err, tt.wantErr)
				return
			}

			if tt.wantErr == nil {
				// 验证短URL格式
				if !strings.HasPrefix(shortURL, "https://short.url/") {
					t.Errorf("Shorten() shortURL should have base URL prefix, got %v", shortURL)
				}

				// 验证短URL长度
				shortKey := strings.TrimPrefix(shortURL, "https://short.url/")
				if len(shortKey) > shortURLLength {
					t.Errorf("Shorten() shortKey length = %v, want <= %v", len(shortKey), shortURLLength)
				}
			}
		})
	}
}

func TestURLShortener_Resolve(t *testing.T) {
	storage := NewMemoryStorage()
	shortener := NewURLShortener(storage, "https://short.url/")

	longURL := "https://www.example.com/test"
	shortURL, err := shortener.Shorten(longURL)
	if err != nil {
		t.Fatalf("Shorten() error = %v", err)
	}

	// 测试解析
	resolved, err := shortener.Resolve(shortURL)
	if err != nil {
		t.Errorf("Resolve() error = %v", err)
	}
	if resolved != longURL {
		t.Errorf("Resolve() = %v, want %v", resolved, longURL)
	}

	// 测试不存在的短URL
	_, err = shortener.Resolve("https://short.url/notexist")
	if err != ErrURLNotFound {
		t.Errorf("Resolve() error = %v, want %v", err, ErrURLNotFound)
	}
}

func TestURLShortener_SameURLReturnsSameShortURL(t *testing.T) {
	storage := NewMemoryStorage()
	shortener := NewURLShortener(storage, "https://short.url/")

	longURL := "https://www.example.com/test"

	shortURL1, err := shortener.Shorten(longURL)
	if err != nil {
		t.Fatalf("Shorten() error = %v", err)
	}

	shortURL2, err := shortener.Shorten(longURL)
	if err != nil {
		t.Fatalf("Shorten() error = %v", err)
	}

	if shortURL1 != shortURL2 {
		t.Errorf("Same URL should return same short URL, got %v and %v", shortURL1, shortURL2)
	}
}

func TestURLShortener_CollisionHandling(t *testing.T) {
	storage := NewMemoryStorage()
	shortener := NewURLShortener(storage, "https://short.url/")

	// 模拟冲突：预先插入一个会产生相同hash的记录
	// 这里我们直接在storage中插入数据来模拟冲突场景
	longURL1 := "https://example1.com"
	shortKey1 := generateShortKey(longURL1)
	storage.Save(shortKey1, "https://different-url.com") // 插入不同的URL但相同的shortKey

	// 现在尝试缩短longURL1，应该检测到冲突并生成新的key
	shortURL, err := shortener.Shorten(longURL1)
	if err != nil {
		t.Fatalf("Shorten() with collision should not error, got %v", err)
	}

	// 验证可以正确解析
	resolved, err := shortener.Resolve(shortURL)
	if err != nil {
		t.Errorf("Resolve() error = %v", err)
	}
	if resolved != longURL1 {
		t.Errorf("Resolve() = %v, want %v", resolved, longURL1)
	}
}

func TestHash2base62(t *testing.T) {
	hash := hashURL("https://example.com")
	result := hash2base62(hash)

	// 验证只包含base62字符
	for _, c := range result {
		if !strings.ContainsRune(base62Chars, c) {
			t.Errorf("hash2base62() contains invalid character: %c", c)
		}
	}

	// 验证相同输入产生相同输出
	result2 := hash2base62(hashURL("https://example.com"))
	if result != result2 {
		t.Errorf("hash2base62() should be deterministic, got %v and %v", result, result2)
	}
}

func TestMemoryStorage(t *testing.T) {
	storage := NewMemoryStorage()

	// 测试Save和Get
	err := storage.Save("abc123", "https://example.com")
	if err != nil {
		t.Fatalf("Save() error = %v", err)
	}

	// 测试Exists
	if !storage.Exists("abc123") {
		t.Error("Exists() should return true for saved key")
	}
	if storage.Exists("notexist") {
		t.Error("Exists() should return false for non-existent key")
	}

	// 测试Get
	longURL, err := storage.Get("abc123")
	if err != nil {
		t.Errorf("Get() error = %v", err)
	}
	if longURL != "https://example.com" {
		t.Errorf("Get() = %v, want %v", longURL, "https://example.com")
	}

	// 测试GetByLongURL
	shortURL, exists := storage.GetByLongURL("https://example.com")
	if !exists {
		t.Error("GetByLongURL() should return true for saved URL")
	}
	if shortURL != "abc123" {
		t.Errorf("GetByLongURL() = %v, want %v", shortURL, "abc123")
	}
}

func BenchmarkShorten(b *testing.B) {
	storage := NewMemoryStorage()
	shortener := NewURLShortener(storage, "https://short.url/")

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		shortener.Shorten("https://www.example.com/path/" + string(rune(i)))
	}
}

func BenchmarkResolve(b *testing.B) {
	storage := NewMemoryStorage()
	shortener := NewURLShortener(storage, "https://short.url/")

	shortURL, _ := shortener.Shorten("https://www.example.com/test")

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		shortener.Resolve(shortURL)
	}
}
