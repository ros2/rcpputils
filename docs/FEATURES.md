# rcpputils Features

This package includes convenience functions for cross-platform c++ functionality and for mathematical utilties. These functionalities are siloed into the `rcpputils` and the `rcppmath` namespaces respectively:
* [`rcpputils` - General convenience functions](#rcpputils-general-convenience-functions)
  * [Assertion functions](#assertion-functions)
  * [Clang thread safety annotation macros](#clang-thread-safety-annotation-macros)
  * [Endianness helpers](#endianness-helpers)
  * [Library discovery](#library-discovery)
  * [String helpers](#string-helpers)
  * [File system helpers](#file-system-helpers)
  * [Type traits helpers](#type-traits-helpers)
  * [Visibility macros](#visibility-macros)
  * [Shared Libraries](#shared-libraries)
* [`rcppmath` - Mathematical utilties](#rcppmath-mathematical-utilities)
  * [Clamping functionality](#clamping-functionality)
  * [Rolling mean accumulator](#rolling-mean-accumulator)

## `rcpputils` - General convenience functions {#rcpputils-general-convenience-functions}
### Assertion Functions {#assertion-functions}
The [rcpputils/asserts.hpp](rcpputils/include/rcpputils/asserts.hpp) header provides the helper functions:
* `require_true`: for validating function inputs. Throws an `std::invalid_argument` exception if the condition fails.
* `check_true`: for checking states. Throws an `rcpputils::IllegalStateException` if the condition fails.
* `assert_true`: for verifying results. Throws an `rcpputils::AssertionException` if the condition fails. This function becomes a no-op in release builds.

These helper functions can be used to improve readability of C++ functions.
Example usage:
```c++
ResultType some_function(ArgType arg)
{
  // Check if the required internal state for calling `some_function` is valid.
  // There's usually no reason to operate on the input if the state is invalid.
  rcpputils::check_true(internal_state.is_valid);

  // Check if the arguments are valid.
  // It's usually best practice to only process valid inputs.
  rcpputils::require_true(arg.is_valid);

  auto result = do_logic(arg);

  // Assert that the result is valid.
  // This will only throw when ran in debug mode if result is invalid.
  // Throwing early when invalid may help find points of error when debugging.
  rcpputils::assert_true(result.is_valid);
  return result
}
```

### Clang Thread Safety Annotation Macros {#clang-thread-safety-annotation-macros}
the `rcpputils/thread_safety_annotations.hpp` header provides macros for Clang's [Thread Safety Analysis](https://clang.llvm.org/docs/ThreadSafetyAnalysis.html) feature.

The macros allow you to annotate your code, but expand to nothing when using a non-clang compiler, so they are safe for cross-platform use.

To use thread safety annotation in your package (in a Clang+libcxx build), enable the `-Wthread-safety` compiler flag.

For example usage, see [the documentation of this feature](https://clang.llvm.org/docs/ThreadSafetyAnalysis.html) and the tests in `test/test_basic.cpp`

### Endianness helpers {#endianness-helpers}
In `rcpputils/endian.hpp`

Emulates the features of std::endian if it is not available. See [cppreference](https://en.cppreference.com/w/cpp/types/endian) for more information.

### Library Discovery {#library-discovery}

In `rcpputils/find_library.hpp`:

*   `find_library(library_name)`: Namely used for dynamically loading RMW
    implementations.
    *   For dynamically loading user-defined plugins in C++, please use
        [`pluginlib`](https://github.com/ros/pluginlib) instead.

### String Helpers {#string-helpers}
In `rcpputils/join.hpp` and `rcpputils/split.hpp`

These headers include simple functions for joining a container into a single string and splitting a string into a container of values.

### File system helpers {#file-system-helpers}
`rcpputils/filesystem_helper.hpp` provides `std::filesystem`-like functionality on systems that do not yet include those features. See the [cppreference](https://en.cppreference.com/w/cpp/header/filesystem) for more information.

### Type traits helpers {#type-traits-helpers}
`rcpputils/pointer_traits.hpp` provides several type trait definitions for pointers and smart pointers.

### Visibility definitions and macros {#visibility-definitions-and-macros}
`rcpputils/visibility_control.hpp` provides macros and definitions for controlling the visibility of class members. The logic was borrowed and then namespaced from [https://gcc.gnu.org/wiki/Visibility](https://gcc.gnu.org/wiki/Visibility).

### Shared Libraries
`rcpputils/shared_library.hpp` provides dynamically loads, unloads and get symbols from shared libraries at run-time.

## `rcppmath` - Mathematical utilities {#rcppmath-mathematical-utilities}
### Clamping functionality {#clamping-functionality}
`rcppmath/clamp.hpp` provides functionality to perform clamping - which restricts a value between two bounds.
The `rcppmath::clamp()` function is overloaded as follows:

* `rcppmath::clamp(const T &, const T &, const T &)` - Takes a value to clamp, and the lower and upper boundaries to clamp against.
* `rcppmath::clamp(const T &, const T &, const T &, Compare)` - In addition to the previous signature, accepts a comparison object that returns `true` if its first argument is less than the second.

### Rolling mean accumulator {#rolling-mean-accumulator}
The `rcppmath/rolling_mean_accumulator.hpp` facilitates computing the rolling mean of a window of accumulated items.
The `rcppmath::RollingMeanAccumulator` can be constructed with an unsigned integral `rolling_window_size` value.
Values can be accumulated and the rolling mean can be obtained through the `rcppmath::RollingMeanAccumulator::accumulate(T)` method and the `rcppmath::RollingMeanAccumulator::getRollingMean()` methods respectively.
