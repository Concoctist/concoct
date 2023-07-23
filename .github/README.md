[![AppVeyor build status](https://img.shields.io/appveyor/ci/ldilley/concoct?label=AppVeyor%20build%20status)](https://ci.appveyor.com/project/ldilley/concoct)
[![CodeFactor grade](https://img.shields.io/codefactor/grade/github/Concoctist/concoct?label=CodeFactor%20quality)](https://www.codefactor.io/repository/github/Concoctist/concoct)
[![Discord](https://img.shields.io/discord/540333638479380487?label=Discord)](https://discord.concoct.ist/)

Concoct 🧪
=======
Concoct is an imperative, dynamically-typed, interpreted, general-purpose programming language written in C.

:construction: **Note:** This project is very much a work in progress. The interpreter is not yet functional. Have a look at the [roadmap](https://github.com/Concoctist/concoct/wiki/Roadmap).

For more information about Concoct, please see the [wiki](https://github.com/Concoctist/concoct/wiki).

### Building :hammer_and_wrench:
#### Requirements
* A C compiler that supports the [C99](http://en.wikipedia.org/wiki/C99) standard ([Clang](http://clang.llvm.org/), [GCC](http://gcc.gnu.org/), [MinGW](https://osdn.net/projects/mingw), [MSVC](http://visualstudio.microsoft.com/) >=2015)
* [CMake](http://cmake.org/) (>=3.1.0)
* make/[gmake](http://www.gnu.org/software/make/) (if using Linux/Unix, [Cygwin](http://www.cygwin.com/), or [MinGW](https://osdn.net/projects/mingw))

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
   $ git clone https://github.com/Concoctist/concoct.git
   ```
   Or:
   ```sh
   $ wget https://github.com/Concoctist/concoct/archive/master.zip && unzip master.zip
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
   $ make
   ```

5. There should now be a `concoct` executable under the `bin` directory if the build was successful:
   ```
   $ ./concoct -v
   Concoct v0.1.0 rev 148 (d976be2) (64-bit Linux) (Debug) built at 00:46 on 01-02-2022
   ```

#### Windows Steps
1. Install the prerequisite software. This includes [Visual Studio](https://visualstudio.microsoft.com/) >=2015 with the "_Desktop development with C++_" workload as
   shown below. The aforementioned workload should include CMake. You can also optionally install [Git](https://git-scm.com/downloads) and add the executable to your path:

   ![image](https://user-images.githubusercontent.com/3323717/147863961-50856b16-9b4a-4dea-a4eb-fff7e6a18000.png)

2. Clone the Concoct source code. This can be done via Visual Studio or by using a git client of your choice:

   ![image](https://user-images.githubusercontent.com/3323717/147864080-fcb8c232-d975-4a99-b060-435232c9346f.png)

   Or:

   ![image](https://user-images.githubusercontent.com/3323717/147864037-62854ead-43fc-4b90-abbf-99d1162f2a8f.png)

   ![image](https://user-images.githubusercontent.com/3323717/147864136-0581bdd1-cfda-4f47-9e30-b59f5a058aa1.png)

3. Generate the CMake cache:

   ![image](https://user-images.githubusercontent.com/3323717/147864180-ee480075-de55-4b86-9f79-7486b2237c90.png)

4. Build Concoct:

   ![image](https://user-images.githubusercontent.com/3323717/147864201-8d8d2ac9-78fe-4efa-a6b7-3c4b7dc3c3d0.png)

5. If the build was successful, `concoct.exe` should now exist in your output directory:

   ![image](https://user-images.githubusercontent.com/3323717/147864334-5c8d44f3-136f-47f9-a1d5-434826e6572d.png)

### Contributing and Support :octocat:
Feel free to [submit an issue](https://github.com/Concoctist/concoct/issues/new) if you require assistance or would like to
make a feature request. You are also welcome to join our Discord server at https://discord.concoct.ist/. Any contributions such as build testing, creating bug reports
or feature requests, and submitting pull requests are appreciated. Our code style guidelines can be found in the "Coding Convention" section of
[CONTRIBUTING.md](https://github.com/Concoctist/concoct/blob/master/.github/CONTRIBUTING.md). Please see the
[fork and pull guide](https://help.github.com/en/github/collaborating-with-issues-and-pull-requests/creating-a-pull-request-from-a-fork)
for direction if you are not certain how to submit a pull request.
