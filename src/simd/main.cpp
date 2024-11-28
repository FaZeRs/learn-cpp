#include <immintrin.h>  // For AVX2 intrinsics

#include <algorithm>
#include <chrono>
#include <iostream>
#include <vector>

void scalarVectorAdd(const std::vector<float>& a, const std::vector<float>& b,
                     std::vector<float>& result) {
  for (size_t i = 0; i < a.size(); ++i) {
    result[i] = a[i] + b[i];
  }
}

// SIMD vectorized vector addition using AVX2
void simdVectorAdd(const std::vector<float>& a, const std::vector<float>& b,
                   std::vector<float>& result) {
  // Process 8 floats at a time with AVX2 (256-bit registers)
  for (size_t i = 0; i < a.size(); i += 8) {
    // Load 8 floats from each input vector into AVX registers
    __m256 va = _mm256_loadu_ps(&a[i]);
    __m256 vb = _mm256_loadu_ps(&b[i]);

    // Perform vectorized addition
    __m256 vresult = _mm256_add_ps(va, vb);

    // Store the result back to the result vector
    _mm256_storeu_ps(&result[i], vresult);
  }
}

void benchmarkVectorAddition(size_t vectorSize) {
  // Prepare input vectors
  std::vector<float> a(vectorSize);
  std::vector<float> b(vectorSize);
  std::vector<float> result(vectorSize);

  // Initialize vectors with some values
  for (size_t i = 0; i < vectorSize; ++i) {
    a[i] = static_cast<float>(i);
    b[i] = static_cast<float>(vectorSize - i);
  }

  // Measure scalar addition time
  auto startScalar = std::chrono::high_resolution_clock::now();
  scalarVectorAdd(a, b, result);
  auto endScalar = std::chrono::high_resolution_clock::now();
  auto scalarDuration = std::chrono::duration_cast<std::chrono::microseconds>(
      endScalar - startScalar);

  // Reset result vector
  std::ranges::fill(result, 0.0f);

  // Measure SIMD addition time
  auto startSIMD = std::chrono::high_resolution_clock::now();
  simdVectorAdd(a, b, result);
  auto endSIMD = std::chrono::high_resolution_clock::now();
  auto simdDuration = std::chrono::duration_cast<std::chrono::microseconds>(
      endSIMD - startSIMD);

  // Print performance comparison
  std::cout << "Vector Size: " << vectorSize << "\n";
  std::cout << "Scalar Addition Time: " << scalarDuration.count()
            << " microseconds\n";
  std::cout << "SIMD Addition Time:   " << simdDuration.count()
            << " microseconds\n";
  std::cout << "Speedup: "
            << static_cast<double>(scalarDuration.count()) /
                   static_cast<double>(simdDuration.count())
            << "x\n";
}

int main() {
  benchmarkVectorAddition(1000000);
  benchmarkVectorAddition(10000000);
}
