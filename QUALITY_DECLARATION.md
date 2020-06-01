This document is a declaration of software quality for the `rcpputils` package, based on the guidelines in [REP-2004](https://www.ros.org/reps/rep-2004.html).

# `rcpputils` Quality Declaration

The package `rcpputils` claims to be in the **Quality Level 4** category.

Below are the rationales, notes, and caveats for this claim, organized by each requirement listed in the [Package Requirements for Quality Level 4 in REP-2004](https://www.ros.org/reps/rep-2004.html).

## Version Policy [1]

### Version Scheme [1.i]

`rcpputils` uses `semver` according to the recommendation for ROS Core packages in the [ROS 2 Developer Guide](https://index.ros.org/doc/ros2/Contributing/Developer-Guide/#versioning).

### Version Stability [1.ii]

`rcpputils` is at a stable version, i.e. `>= 1.0.0`.
The current version can be found in its [package.xml](package.xml), and its change history can be found in its [CHANGELOG](CHANGELOG.rst).

### Public API Declaration [1.iii]

All symbols in the installed headers are considered part of the public API.

All installed headers are in the `include` directory of the package, headers in any other folders are not installed and considered private.

### API Stability Within a Released ROS Distribution [1.iv]/[1.vi]

`rcpputils` will not break public API within a released ROS distribution, i.e. no major releases once the ROS distribution is released.

### ABI Stability Within a Released ROS Distribution [1.v]/[1.vi]

`rcpputils` contains C code and therefore must be concerned with ABI stability, and will maintain ABI stability within a ROS distribution.

## Change Control Process [2]

`rcpputils` follows the recommended guidelines for ROS Core packages in the [ROS 2 Developer Guide](https://index.ros.org/doc/ros2/Contributing/Developer-Guide/#package-requirements).

### Change Requests [2.i]

This package requires that all changes occur through a pull request.

### Contributor Origin [2.ii]

 This package uses DCO as its confirmation of contributor origin policy. More information can be found in [CONTRIBUTING](CONTRIBUTING.md).

### Peer Review Policy [2.iii]

Following the recommended guidelines for ROS Core packages, all pull requests must have at least 1 peer review.

### Continuous Integration [2.iv]

All pull request must pass CI on all [tier 1 platforms](https://www.ros.org/reps/rep-2000.html#support-tiers)

### Documentation Policy [2.v]

All pull requests must resolve related documentation changes before merging.

## Documentation [3]

### Feature Documentation [3.i]

`rcpputils` has a [feature list](docs/FEATURES.md) and each item in the list links to the corresponding feature documentation.
There is documentation for all of the features, and new features require documentation before being added.

### Public API Documentation [3.ii]

`rcpputils` has embedded API documentation. It is not yet hosted publicly.

### License [3.iii]

The license for `rcpputils` is Apache 2.0, and a summary is in each source file, the type is declared in the `package.xml` manifest file, and a full copy of the license is in the [LICENSE](./LICENSE) file.

There is an automated test which runs a linter that ensures each file has a license statement.

Most recent test results can be found [here](http://build.ros2.org/view/Epr/job/Epr__rcpputils__ubuntu_bionic_amd64/lastBuild/testReport/rcpputils/)

### Copyright Statements [3.iv]

The copyright holders each provide a statement of copyright in each source code file in `rcpputils`.

There is an automated test which runs a linter that ensures each file has at least one copyright statement.

The results of the test can be found [here](http://build.ros2.org/view/Epr/job/Epr__rcpputils__ubuntu_bionic_amd64/lastBuild/testReport/rcpputils/copyright/).

## Testing [4]

### Feature Testing [4.i]

Each feature in `rcpputils` has corresponding tests which simulate typical usage, and they are located in the `test` directory.
New features are required to have tests before being added.

### Public API Testing [4.ii]

Each part of the public API have tests, and new additions or changes to the public API require tests before being added.
The tests aim to cover both typical usage and corner cases, but are quantified by contributing to code coverage.

### Coverage [4.iii]

`rcpputils` does not currently track test coverage.

### Performance [4.iv]

`rcpputils` does not currently have performance tests.

### Linters and Static Analysis [4.v]

`rcpputils` uses and passes all the standard linters and static analysis tools for a C++ package as described in the [ROS 2 Developer Guide](https://index.ros.org/doc/ros2/Contributing/Developer-Guide/#linters).

Results of the nightly linter tests can be found [here](http://build.ros2.org/view/Epr/job/Epr__rcpputils__ubuntu_bionic_amd64/lastBuild/testReport/rcpputils/).

## Dependencies [5]

### Direct Runtime ROS Dependencies [5.i]/[5.ii]

`rcpputils` has the following runtime ROS dependencies:
* `rcutils`

It has several "buildtool" dependencies, which do not affect the resulting quality of the package, because they do not contribute to the public library API.
It also has several test dependencies, which do not affect the resulting quality of the package, because they are only used to build and run the test code.

### Direct Runtime Non-ROS Dependencies [5.iii]

`rcpputils` does not have any runtime non-ROS dependencies.

## Platform Support [6]

`rcpputils` supports all of the tier 1 platforms as described in [REP-2000](https://www.ros.org/reps/rep-2000.html#support-tiers), and tests each change against all of them.

Currently nightly results can be seen here:
* [linux-aarch64_release](https://ci.ros2.org/view/nightly/job/nightly_linux-aarch64_release/lastBuild/testReport/rcpputils/)
* [linux_release](https://ci.ros2.org/view/nightly/job/nightly_linux_release/lastBuild/testReport/rcpputils/)
* [mac_osx_release](https://ci.ros2.org/view/nightly/job/nightly_osx_release/lastBuild/testReport/rcpputils/)
* [windows_release](https://ci.ros2.org/view/nightly/job/nightly_win_rel/lastBuild/testReport/rcpputils/)

## Vulnerability Disclosure Policy [7.i]

This package conforms to the Vulnerability Disclosure Policy in [REP-2006](https://www.ros.org/reps/rep-2006.html).
