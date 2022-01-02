[![AppVeyor build status](https://img.shields.io/appveyor/ci/ldilley/concoct?label=AppVeyor%20build%20status)](https://ci.appveyor.com/project/ldilley/concoct)
[![Travis CI build status](https://img.shields.io/travis/com/ConcoctLang/concoct?label=Travis%20CI%20build%20status)](https://travis-ci.com/ConcoctLang/concoct)
[![CodeFactor grade](https://img.shields.io/codefactor/grade/github/ConcoctLang/concoct?label=CodeFactor%20quality)](https://www.codefactor.io/repository/github/ConcoctLang/concoct)
[![Discord](https://img.shields.io/discord/540333638479380487?label=Discord)](https://discord.concoct.dev/)

Concoct 🧪
=======
Concoct is an imperative, dynamically-typed, interpreted, general-purpose programming language written in C.

:construction: **Note:** This project is very much a work in progress. The interpreter is not yet functional. Have a look at the [roadmap](https://github.com/ConcoctLang/concoct/wiki/Roadmap).

For more information about Concoct, please see the [wiki](https://github.com/ConcoctLang/concoct/wiki).

### Building :hammer_and_wrench:
#### Requirements
* A C compiler that supports the [C99](http://en.wikipedia.org/wiki/C99) standard ([Clang](http://clang.llvm.org/), [GCC](http://gcc.gnu.org/), [MinGW](https://osdn.net/projects/mingw), [MSVC](http://visualstudio.microsoft.com/) >=2015)
* [CMake](http://cmake.org/) (>=3.1.0)
* make/[gmake](http://www.gnu.org/software/make/) (if using Linux/Unix, [Cygwin](http://www.cygwin.com/), or [MinGW](https://osdn.net/projects/mingw))
* Optional software for debugging may include: [gdb](http://www.gnu.org/software/gdb/) (Linux), [lldb](http://lldb.llvm.org/) (Linux, FreeBSD, and macOS), [DTrace](http://dtrace.org/blogs/about/) (FreeBSD, macOS, and Solaris), ktrace (FreeBSD), ltrace (Linux), [strace](http://strace.io/) (Linux), truss (AIX, FreeBSD, and Solaris), tusc (HP-UX), and [Valgrind](http://valgrind.org/)

#### Linux/Unix and Cygwin Steps
1. Install prerequisites:

   Debian/Ubuntu:
   ```sh
   # apt-get install cmake gcc git make
   ```
   CentOS/Fedora/RHEL:
   ```sh
   # yum install cmake gcc git make
   ```
   FreeBSD:
   ```sh
   # pkg install clang cmake git make
   ```
1. Obtain the source code via `git` or download a zip archive:
   ```sh
   $ git clone https://github.com/ConcoctLang/concoct.git
   ```
   Or:
   ```sh
   $ wget https://github.com/ConcoctLang/concoct/archive/master.zip && unzip master.zip
   ```

2. In the top-level directory where `CMakeLists.txt` exists, create a build directory:
   ```sh
   $ mkdir bld && cd bld
   ```

3. Generate the `Makefile` (you can alternatively use `ccmake` here if you prefer):
   ```sh
   $ cmake ..
   ```

4. Build Concoct:
   ```sh
   make
   ```

5. Congratulations! There should now be a `concoct` executable under the `bin` directory.

#### Windows Steps

:triangular_flag_on_post: This section needs to be populated.

### Contributing and Support :octocat:
Feel free to [submit an issue](https://github.com/ConcoctLang/concoct/issues/new) if you require assistance or would like to
make a feature request. Any contributions such as build testing, creating bug reports or feature requests, and submitting pull requests are appreciated. Our code style guidelines can be found in the "Coding Convention" section of [CONTRIBUTING.md](https://github.com/ConcoctLang/concoct/blob/master/.github/CONTRIBUTING.md). Please see the
[fork and pull guide](https://help.github.com/en/github/collaborating-with-issues-and-pull-requests/creating-a-pull-request-from-a-fork)
for direction if you are not certain how to submit a pull request.
