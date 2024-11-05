# C++ Template Metaprogramming

Template metaprogramming is a technique that enables computations to be performed at compile time rather than runtime, leading to better performance and compile-time error detection.

## Example: Compile-time Factorial Calculator

The example implements a factorial calculator using template metaprogramming with the following features:
- Calculations are performed entirely at compile time
- Uses recursive template instantiation
- Leverages `std::integral_constant` for value storage

### How it Works

```cpp
template <int N>
struct Factorial : std::integral_constant<int, N * Factorial<N - 1>::value> {};

template <>
struct Factorial<0> : std::integral_constant<int, 1> {};
```

1. The primary template `Factorial<N>` recursively multiplies N by Factorial<N-1>
2. The template specialization for N=0 provides the base case (0! = 1)
3. `std::integral_constant` is used to store the computed values

### Usage

```cpp
// Compute factorials at compile time
auto fact5 = Factorial<5>::value;  // 120
auto fact10 = Factorial<10>::value;  // 3628800
```

## Benefits of Template Metaprogramming

1. **Compile-time Computation**: Calculations are performed during compilation, resulting in zero runtime overhead
2. **Type Safety**: Errors are caught at compile time rather than runtime
3. **Optimization**: Compiler can optimize the code better since values are known at compile time

## Limitations

1. **Compile Time**: Complex template metaprograms can increase compilation time
2. **Debug Difficulty**: Template error messages can be cryptic
3. **Integer Overflow**: Be careful with large numbers as they might overflow during compile-time computation

## Requirements

- C++11 or later
- A modern C++ compiler (GCC, Clang, MSVC)

## Building

```bash
g++ -std=c++11 main.cpp -o factorial
```

## Running

```bash
./factorial
```

Expected output:
```
Factorial of 5 is 120
Factorial of 10 is 3628800
```
