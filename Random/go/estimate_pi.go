package main

import (
	"fmt"
	"math"
	"math/rand"
	"runtime"
	"strings"
	"sync"
	"sync/atomic"
	"time"
)

// EstimatePi 使用蒙特卡罗方法估算 π 的值
// 原理：在单位正方形内随机生成点，计算落在内切圆内的点的比例
// 圆的面积 / 正方形面积 = π/4，因此 π ≈ 4 * (圆内点数 / 总点数)
func EstimatePi(samples int64) float64 {
	var insideCircle int64 = 0

	for i := int64(0); i < samples; i++ {
		x := rand.Float64()
		y := rand.Float64()

		// 判断点是否在单位圆内
		if x*x+y*y <= 1.0 {
			insideCircle++
		}
	}

	return 4.0 * float64(insideCircle) / float64(samples)
}

// EstimatePiConcurrent 使用并发方式估算 π
func EstimatePiConcurrent(samples int64, workers int) float64 {
	if workers <= 0 {
		workers = runtime.NumCPU()
	}

	var insideCircle int64
	var wg sync.WaitGroup

	samplesPerWorker := samples / int64(workers)
	remainder := samples % int64(workers)

	for i := 0; i < workers; i++ {
		wg.Add(1)
		workerSamples := samplesPerWorker
		if i == 0 {
			workerSamples += remainder
		}

		go func(n int64) {
			defer wg.Done()

			// 每个 goroutine 使用独立的随机数生成器
			source := rand.NewSource(time.Now().UnixNano() + int64(n))
			rng := rand.New(source)

			var localInside int64
			for j := int64(0); j < n; j++ {
				x := rng.Float64()
				y := rng.Float64()

				if x*x+y*y <= 1.0 {
					localInside++
				}
			}

			atomic.AddInt64(&insideCircle, localInside)
		}(workerSamples)
	}

	wg.Wait()
	return 4.0 * float64(insideCircle) / float64(samples)
}

// EstimatePiWithProgress 带进度显示的估算
func EstimatePiWithProgress(samples int64, workers int, showProgress bool) float64 {
	if workers <= 0 {
		workers = runtime.NumCPU()
	}

	var insideCircle int64
	var completed int64
	var wg sync.WaitGroup

	// 进度显示
	done := make(chan bool)
	if showProgress {
		go func() {
			ticker := time.NewTicker(500 * time.Millisecond)
			defer ticker.Stop()

			for {
				select {
				case <-done:
					return
				case <-ticker.C:
					current := atomic.LoadInt64(&completed)
					progress := float64(current) / float64(samples) * 100
					currentPi := 4.0 * float64(atomic.LoadInt64(&insideCircle)) / float64(current+1)
					fmt.Printf("\r进度: %.2f%% | 当前估算: %.10f", progress, currentPi)
				}
			}
		}()
	}

	samplesPerWorker := samples / int64(workers)
	remainder := samples % int64(workers)

	for i := 0; i < workers; i++ {
		wg.Add(1)
		workerSamples := samplesPerWorker
		if i == 0 {
			workerSamples += remainder
		}

		go func(n int64, workerId int) {
			defer wg.Done()

			source := rand.NewSource(time.Now().UnixNano() + int64(workerId)*1000)
			rng := rand.New(source)

			var localInside int64
			for j := int64(0); j < n; j++ {
				x := rng.Float64()
				y := rng.Float64()

				if x*x+y*y <= 1.0 {
					localInside++
				}

				if j%10000 == 0 {
					atomic.AddInt64(&completed, 10000)
				}
			}

			// 添加剩余的样本数
			remaining := n % 10000
			if remaining > 0 {
				atomic.AddInt64(&completed, remaining)
			}

			atomic.AddInt64(&insideCircle, localInside)
		}(workerSamples, i)
	}

	wg.Wait()
	if showProgress {
		close(done)
		fmt.Printf("\r进度: 100.00%% | 完成!                    \n")
	}

	return 4.0 * float64(insideCircle) / float64(samples)
}

// CalculateError 计算估算误差
func CalculateError(estimated float64) float64 {
	return math.Abs(estimated-math.Pi) / math.Pi * 100
}

// Benchmark 性能测试
func Benchmark() {
	samples := []int64{1000, 10000, 100000, 1000000, 10000000}

	fmt.Println("=== 性能基准测试 ===")

	for _, n := range samples {
		// 单线程
		start := time.Now()
		pi1 := EstimatePi(n)
		elapsed1 := time.Since(start)
		error1 := CalculateError(pi1)

		// 多线程
		start = time.Now()
		pi2 := EstimatePiConcurrent(n, runtime.NumCPU())
		elapsed2 := time.Since(start)
		error2 := CalculateError(pi2)

		speedup := float64(elapsed1) / float64(elapsed2)

		fmt.Printf("样本数: %d\n", n)
		fmt.Printf("  单线程: π ≈ %.10f, 误差: %.6f%%, 耗时: %v\n", pi1, error1, elapsed1)
		fmt.Printf("  多线程: π ≈ %.10f, 误差: %.6f%%, 耗时: %v\n", pi2, error2, elapsed2)
		fmt.Printf("  加速比: %.2fx\n\n", speedup)
	}
}

func main() {
	// 设置随机数种子
	rand.Seed(time.Now().UnixNano())

	fmt.Println("=================================")
	fmt.Println("   蒙特卡罗方法估算 π 值")
	fmt.Println("=================================")

	// 1. 快速演示
	fmt.Println("--- 快速演示 (100万样本) ---")
	samples := int64(1_000_000)
	pi := EstimatePi(samples)
	error := CalculateError(pi)
	fmt.Printf("估算值: %.10f\n", pi)
	fmt.Printf("真实值: %.10f\n", math.Pi)
	fmt.Printf("误差:   %.6f%%\n\n", error)

	// 2. 并发版本
	fmt.Println("--- 并发版本 (1000万样本) ---")
	samples = 10_000_000
	workers := runtime.NumCPU()
	fmt.Printf("使用 %d 个 CPU 核心\n", workers)

	start := time.Now()
	pi = EstimatePiConcurrent(samples, workers)
	elapsed := time.Since(start)
	error = CalculateError(pi)

	fmt.Printf("估算值: %.10f\n", pi)
	fmt.Printf("真实值: %.10f\n", math.Pi)
	fmt.Printf("误差:   %.6f%%\n", error)
	fmt.Printf("耗时:   %v\n\n", elapsed)

	// 3. 高精度估算（带进度）
	fmt.Println("--- 高精度估算 (1亿样本，带进度显示) ---")
	samples = 100_000_000

	start = time.Now()
	pi = EstimatePiWithProgress(samples, workers, true)
	elapsed = time.Since(start)
	error = CalculateError(pi)

	fmt.Printf("估算值: %.10f\n", pi)
	fmt.Printf("真实值: %.10f\n", math.Pi)
	fmt.Printf("误差:   %.6f%%\n", error)
	fmt.Printf("耗时:   %v\n\n", elapsed)

	// 4. 性能基准测试
	fmt.Println("\n--- 性能基准测试 ---")
	Benchmark()

	// 5. 收敛性分析
	fmt.Println("=== 收敛性分析 ===")
	testSamples := []int64{100, 1000, 10000, 100000, 1000000, 10000000}

	fmt.Printf("%-15s %-15s %-15s\n", "样本数", "估算值", "误差 (%)")
	fmt.Println(strings.Repeat("-", 45))

	for _, n := range testSamples {
		pi := EstimatePiConcurrent(n, workers)
		error := CalculateError(pi)
		fmt.Printf("%-15d %-15.10f %-15.6f\n", n, pi, error)
	}

	fmt.Println("\n=== 程序结束 ===")
}
