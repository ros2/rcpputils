# rcpputils: ROS 2 C++ Utilities

`rcpputils` is a C++ API consisting of macros, functions, and data structures intended for use throughout the ROS 2 codebase

This package currently contains:
* Assertion functions
* Clang thread safety annotation macros
* Library discovery
* String helpers
* File system helpers
* Type traits helpers

## Assertion Functions
The [rcpputils/asserts.hpp](rcpputils/include/rcpputils/asserts.hpp) header provides the helper functions:
* `require_true`: for validating function inputs. Throws an `std::invalid_argument` exception if the condition fails.
* `check_true`: for checking states. Throws an `rcpputils::InvalidStateException` if the condition fails.
* `assert_true`: for verifying results. Throws an `rcpputils::AssertionException` if the condition fails.

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

## Clang Thread Safety Annotation Macros
the `rcpputils/thread_safety_annotations.hpp` header provides macros for Clang's [Thread Safety Analysis](https://clang.llvm.org/docs/ThreadSafetyAnalysis.html) feature.

The macros allow you to annotate your code, but expand to nothing when using a non-clang compiler, so they are safe for cross-platform use.

To use thread safety annotation in your package (in a Clang+libcxx build), enable the `-Wthread-safety` compiler flag.

For example usage, see [the documentation of this feature](https://clang.llvm.org/docs/ThreadSafetyAnalysis.html) and the tests in `test/test_basic.cpp`

## Library Discovery

In `rcpputils/find_library.hpp`:

*   `find_library(library_name)`: Namely used for dynamically loading RMW
    implementations.
    *   For dynamically loading user-defind plugins in C++, please use
        [`pluginlib`](https://github.com/ros/pluginlib) instead.
