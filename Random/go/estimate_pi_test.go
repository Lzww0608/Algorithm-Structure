package main

import (
	"fmt"
	"math"
	"runtime"
	"testing"
)

// TestEstimatePi 测试基本的 π 估算
func TestEstimatePi(t *testing.T) {
	samples := int64(1000000)
	pi := EstimatePi(samples)

	// 检查结果是否在合理范围内 (±5%)
	if math.Abs(pi-math.Pi) > 0.05*math.Pi {
		t.Errorf("估算误差过大: got %.6f, want ~%.6f", pi, math.Pi)
	}

	// 检查结果是否为正数
	if pi <= 0 {
		t.Errorf("π 的值应该为正数, got %.6f", pi)
	}

	// 检查结果是否在合理范围 (2到4之间)
	if pi < 2.0 || pi > 4.0 {
		t.Errorf("π 的估算值超出合理范围: got %.6f", pi)
	}
}

// TestEstimatePiConcurrent 测试并发版本
func TestEstimatePiConcurrent(t *testing.T) {
	samples := int64(1000000)
	workers := runtime.NumCPU()
	pi := EstimatePiConcurrent(samples, workers)

	// 检查结果是否在合理范围内
	if math.Abs(pi-math.Pi) > 0.05*math.Pi {
		t.Errorf("并发估算误差过大: got %.6f, want ~%.6f", pi, math.Pi)
	}

	if pi <= 0 {
		t.Errorf("π 的值应该为正数, got %.6f", pi)
	}
}

// TestEstimatePiWithDifferentWorkers 测试不同工作线程数
func TestEstimatePiWithDifferentWorkers(t *testing.T) {
	samples := int64(100000)
	workerCounts := []int{1, 2, 4, 8, runtime.NumCPU()}

	for _, workers := range workerCounts {
		pi := EstimatePiConcurrent(samples, workers)

		if math.Abs(pi-math.Pi) > 0.1*math.Pi {
			t.Errorf("Workers=%d: 估算误差过大: got %.6f, want ~%.6f", workers, pi, math.Pi)
		}
	}
}

// TestCalculateError 测试误差计算
func TestCalculateError(t *testing.T) {
	tests := []struct {
		estimated float64
		wantError float64
	}{
		{math.Pi, 0.0},
		{3.0, (math.Pi - 3.0) / math.Pi * 100},
		{3.5, (3.5 - math.Pi) / math.Pi * 100},
	}

	for _, tt := range tests {
		got := CalculateError(tt.estimated)
		if math.Abs(got-tt.wantError) > 0.0001 {
			t.Errorf("CalculateError(%.6f) = %.6f, want %.6f", tt.estimated, got, tt.wantError)
		}
	}
}

// TestEstimatePiConvergence 测试收敛性
func TestEstimatePiConvergence(t *testing.T) {
	samples := []int64{1000, 10000, 100000}
	for _, n := range samples {
		pi := EstimatePi(n)
		error := CalculateError(pi)

		// 一般情况下，样本数越多，误差应该越小（但不是严格单调）
		t.Logf("Samples: %d, Pi: %.6f, Error: %.4f%%", n, pi, error)

		// 检查误差是否在合理范围
		if error > 10.0 {
			t.Errorf("样本数 %d 时误差过大: %.4f%%", n, error)
		}

	}
}

// TestEstimatePiZeroSamples 测试边界条件
func TestEstimatePiZeroSamples(t *testing.T) {
	// 注意：零样本会导致除以零，这是一个边界情况
	// 在实际代码中应该处理这种情况
	defer func() {
		if r := recover(); r == nil {
			// 如果没有 panic，检查返回值
			pi := EstimatePi(0)
			if !math.IsNaN(pi) && !math.IsInf(pi, 0) {
				// 如果返回了有效值（可能是0或其他），记录日志
				t.Logf("Zero samples returned: %.6f", pi)
			}
		}
	}()
}

// BenchmarkEstimatePi 基准测试：单线程版本
func BenchmarkEstimatePi(b *testing.B) {
	samples := int64(10000)
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		EstimatePi(samples)
	}
}

// BenchmarkEstimatePiConcurrent 基准测试：并发版本
func BenchmarkEstimatePiConcurrent(b *testing.B) {
	samples := int64(10000)
	workers := runtime.NumCPU()
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		EstimatePiConcurrent(samples, workers)
	}
}

// BenchmarkEstimatePi_1M 基准测试：100万样本
func BenchmarkEstimatePi_1M(b *testing.B) {
	samples := int64(1000000)
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		EstimatePi(samples)
	}
}

// BenchmarkEstimatePiConcurrent_1M 基准测试：100万样本（并发）
func BenchmarkEstimatePiConcurrent_1M(b *testing.B) {
	samples := int64(1000000)
	workers := runtime.NumCPU()
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		EstimatePiConcurrent(samples, workers)
	}
}

// BenchmarkEstimatePi_10M 基准测试：1000万样本
func BenchmarkEstimatePi_10M(b *testing.B) {
	samples := int64(10000000)
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		EstimatePi(samples)
	}
}

// BenchmarkEstimatePiConcurrent_10M 基准测试：1000万样本（并发）
func BenchmarkEstimatePiConcurrent_10M(b *testing.B) {
	samples := int64(10000000)
	workers := runtime.NumCPU()
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		EstimatePiConcurrent(samples, workers)
	}
}

// ExampleEstimatePi 示例：基本用法
func ExampleEstimatePi() {
	pi := EstimatePi(1000000)
	fmt.Printf("π ≈ %.4f\n", pi)
}

// ExampleEstimatePiConcurrent 示例：并发用法
func ExampleEstimatePiConcurrent() {
	pi := EstimatePiConcurrent(10000000, runtime.NumCPU())
	fmt.Printf("π ≈ %.6f\n", pi)
}
