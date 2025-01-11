# [GAViewer](https://geometricalgebra.org/gaviewer_download.html) application (re)built with CMake

This is an attempt to (re)build the [GAViewer](https://geometricalgebra.org/gaviewer_download.html) application (multi-purpose program for geometric algebra computations and visualization) using CMake and currently available C/C++ compilers.

## Platforms used
| platform | OS                       | C/C++ compiler                 | graphical backend |
| :------- | :----------------------- | :----------------------------- | :---------------- |
| Linux    | Debian Linux 12 bookworm | gcc 12.2.0 <sup>(1)</sup>      | X11               |
| BSD      | FreeBSD 14.2-RELEASE     | clang 18.1.8 <sup>(2)</sup>    | X11               |
| Windows  | Windows 11 Pro           | clang 19.1.6 <sup>(3)</sup>    | Windows           |
| Windows  | Windows 11 Pro           | Visual C++ 2022 <sup>(4)</sup> | Windows           |
| Mac OS   | _- unavailable -_        | _- unavailable -_              | _- unavailable -_ |

<div style="font-size: 0.8em; line-height: 1.4;">
(1) gcc (Debian 12.2.0-14) 12.2.0.<br/>
(2) clang version 18.1.8, Target: x86_64-portbld-freebsd14.0, Thread model: posix.<br/>
(3) clang version 19.1.6, Target: x86_64-pc-windows-msvc, Thread model: posix.<br/>
(4) Microsoft (R) C/C++ Optimizing Compiler Version 19.42.34435 for x64; Visual Studio Community 2022 (64-bit).
</div>

## Build instructions

#### Building from the command line

GAViewer can be built from the command line on both `Windows` and `UNIX` platforms-

```sh
# Clone GAViewer repository:
git clone https://github.com/mangh/GAViewer.git

cd GAViewer

# Configure:
mkdir build
cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Relase ..

# Build:
cmake --build .

# Install:
cmake --install . --component GAViewer --prefix /my/install/dir
```

NOTE: If you omit the (optional) `--component` parameter in the last command, CMake will also install FLTK header files and FLTK static libraries (which you may not want).

#### Building with Visual Studio

GAViewer can also be built using `Visual Studio` (VS) on the `Windows` platform (and perhaps others):

* Clone GAViewer repository (from the command line):

  ```cmd
  git clone https://github.com/mangh/GAViewer.git
  ```
* Open Visual Studio, and then (from within VS) open the `GAViewer/CMakeLists.txt` project script:
  ```
  VS Menu: File / Open / CMake... / Open CMake Project by CMakeLists.txt
  ```
* (optional) VS configures the project when you open it but you can reconfigure/refresh it with the operation:
  ```
  VS Menu: Project/ Delete Cache and Reconfigure
  ```
* Build the project using the operation:
  ```
  VS Menu: Build / Build All
  ```
  or:
  ```
  VS Menu: Build / Rebuild All
  ```

---