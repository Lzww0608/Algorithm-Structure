go test -bench=. -cpuprofile=cpu.prof
go test -bench=. -memprofile=mem.prof

goos: linux
goarch: amd64
pkg: as/Random/go
cpu: 13th Gen Intel(R) Core(TM) i9-13900K
BenchmarkEstimatePi-32                              8210            142304 ns/op
BenchmarkEstimatePiConcurrent-32                   13939             84145 ns/op
BenchmarkEstimatePi_1M-32                             85          13836404 ns/op
BenchmarkEstimatePiConcurrent_1M-32                 2389            480319 ns/op
BenchmarkEstimatePi_10M-32                             8         146839160 ns/op
BenchmarkEstimatePiConcurrent_10M-32                 276           4307505 ns/op
PASS
ok      as/Random/go    8.732s
goos: linux
goarch: amd64
pkg: as/Random/go
cpu: 13th Gen Intel(R) Core(TM) i9-13900K
BenchmarkEstimatePi-32                              8660            139731 ns/op
BenchmarkEstimatePiConcurrent-32                   14634             82911 ns/op
BenchmarkEstimatePi_1M-32                             79          13805330 ns/op
BenchmarkEstimatePiConcurrent_1M-32                 2472            456710 ns/op
BenchmarkEstimatePi_10M-32                             8         138555856 ns/op
BenchmarkEstimatePiConcurrent_10M-32                 286           4198086 ns/op
PASS
ok      as/Random/go    9.454s