# GAViewer2

A modernized version of [GAViewer](https://geometricalgebra.org/gaviewer_download.html), the interactive
viewer for geometric algebra by Daniel Fontijne. GAViewer was written for the book
[Geometric Algebra for Computer Science](https://geometricalgebra.org/) (Dorst, Fontijne, Mann). You
type geometric algebra expressions in its console or load them from files, and it draws the objects
they represent: points, lines, planes, circles, spheres, point pairs, ... You can rotate and zoom the
view, drag the points, and change the parameters of dynamic figures.

GAViewer2 starts from the last published source, GAViewer 0.86 (2009). It keeps the program's behaviour
and its console language, so the [interactive book figures](https://geometricalgebra.org/figures.html)
and your existing `.g` / `.geo` files run unchanged. Most of the modernization was done with an AI
assistant (Claude, by Anthropic); see [License and credits](#license-and-credits).

## What is different from GAViewer 0.86

- **C++20 and CMake.** The source builds with current compilers. CMake downloads the external components
  (FLTK, Garamon, Eigen, the book figures) at configure time.
- **Garamon algebras.** The geometric algebras (`e3ga`, `p3ga`, `c3ga`, `c5ga`, `i2ga`) are generated at
  build time by [Garamon](https://github.com/vincentnozick/garamon) (from a [fork](https://github.com/mangh/garamon)),
  instead of the Gaigen-generated code of 0.86.
- **A hand-written console parser** replaces the ANTLR 2 generated one, with error messages that give
  the file, line and column.
- **FLTK 1.4.5** for the user interface.
- **Saving and reopening `.geo` files** (File > Save State, File > Open) restores the figures: the user
  functions, controls, drawing settings and exact coordinates are saved.
- **Bug fixes**, among others: crashes when a file that starts with `clear` is imported from a running
  function or a dynamic statement, wrong casts between the models.
- **Headless regression tests** (see [Testing](#testing)).

## Building

### Requirements

- CMake 3.25 or newer, Git, and a C++20 compiler.
- Internet access the first time CMake configures the project (it downloads FLTK, the Garamon fork,
  Eigen, CLI11 and the book figures).
- On Linux / BSD: the development packages FLTK needs for X11 and OpenGL. On Debian:

  ```sh
  sudo apt install build-essential cmake ninja-build git libx11-dev libxext-dev libglu1-mesa-dev
  # optional, for antialiased fonts (Xft) and multiple monitors (Xinerama):
  sudo apt install libxft-dev libfontconfig1-dev libxinerama-dev
  ```

Tested on Windows 11 with clang 23.1.5 and Visual C++ 19.51 (Visual Studio 2026), and on Debian 13
under WSL 2 (gcc 14.2). Earlier versions of this project also built on Debian Linux 12 (gcc 12) and
FreeBSD 14 (clang 18), but the current version has not been re-checked there yet.

### From the command line

```sh
git clone https://github.com/mangh/GAViewer2.git
cd GAViewer2

cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build

# install the application and the book figures:
cmake --install build --component GAViewer --prefix /my/install/dir
cmake --install build --component Figures  --prefix /my/install/dir
```

Without `--component`, CMake also installs the FLTK headers and static libraries.

### With Visual Studio

Open the folder (File > Open > CMake..., select `CMakeLists.txt`), let Visual Studio configure it,
then Build > Build All.

### Options

| Option | Default | Meaning |
| :----- | :------ | :------ |
| `GAVIEWER_BUILD_TESTS` | `ON` | build the regression tests |
| `USE_ASAN` | `OFF` | build with the address sanitizer |

## Using it

Start `GAViewer2`. To run a book figure, load the figure scripts (File > Load .g directory, select the
installed `figures` folder), then type its function in the console, e.g.:

```
FIG_2_2();
```

`help();` in the console lists the built-in functions.

## Testing

```sh
ctest --test-dir build -j8                # all tests
ctest --test-dir build -j8 -LE geo        # all except the .geo round trips (the slowest ones)
ctest --test-dir build -R FIG_2_2 --output-on-failure
```

- `figure.FIG_x_y`: every book figure runs headless (without the GUI), and its objects (coordinates and
  geometric interpretation) and console output are compared with a reference in `viewer/test/golden`.
- `script.<name>`: test scripts in `viewer/test/scripts`, e.g. every built-in function in every model.
- `geo.FIG_x_y`: every book figure is saved to a `.geo` file, opened again in a new process, and its
  objects are compared.
- `gadiff.<algebra>`: every operation of the Garamon algebras against the Gaigen algebras of
  GAViewer 0.86, on thousands of multivectors. This is the only reason the Gaigen code is still in
  the repository (`viewer/src/e3ga`, ...).
- `tubegeometry`: the 3D drawing geometry against the multivector formulas it replaces.

The reference files were made with GAViewer's original algebras, so they document the 0.86 behaviour.
More developer documentation (architecture, conventions, pitfalls of the Garamon API) is in
[CLAUDE.md](CLAUDE.md).

## Repository layout

| Folder | Contents |
| :----- | :------- |
| `viewer/src` | the GAViewer source (`console`: the console language; `ga`: the algebra layer over Garamon) |
| `viewer/test` | the regression tests and their reference files |
| `garamon` | the algebra configurations (`<name>.conf`) for the Garamon generator |
| `fltk` | how FLTK is downloaded and built |
| `figures` | how the book figures are downloaded, and a patch for them |
| `cmake` | CMake helper scripts |

## License and credits

GAViewer2 is free software: you can redistribute it and/or modify it under the terms of the
[GNU General Public License](LICENSE.txt), version 2 or (at your option) any later version, as
GAViewer itself.

- **GAViewer 0.86:** Copyright 2002-2009 Daniel Fontijne, University of Amsterdam.
- **Modifications:** Copyright 2024-2026 mangh. The files of GAViewer 0.86 were modified from
  November 2024 on; the git history records every change.

**Written with AI assistance.** Since November 2026, most of the modernization was done together with
Claude (Claude Opus 5.5, an AI model by Anthropic, used through Claude Code). This includes the headless
regression tests, the hand-written console parser, the algebra layer and the conversion to the Garamon
algebras, the differential tests against the Gaigen algebras, the `.geo` save/restore fixes and the
crash fixes. Claude wrote most of that code and its tests. mangh set the goals and the order of the
work, made the design decisions, reviewed the changes and checked them in the GUI. The commits written
with Claude end with a `Co-Authored-By: Claude Opus 5.5` line (`git log --grep="Co-Authored-By: Claude"`
lists them).

External components keep their own licenses. They are downloaded at build time and are not part of
this repository:

- [FLTK](https://www.fltk.org/): GNU LGPL with exceptions that allow static linking.
- [Garamon](https://github.com/vincentnozick/garamon): Stéphane Breuils and Vincent Nozick, MIT
  license.
- [Eigen](https://eigen.tuxfamily.org/): MPL 2.0.
- [CLI11](https://github.com/CLIUtils/CLI11): BSD 3-clause license.
- The [interactive book figures](https://geometricalgebra.org/figures.html) from
  [geometricalgebra.org](https://geometricalgebra.org/).
