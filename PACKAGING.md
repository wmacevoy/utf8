# Packaging

This document explains the packaging infrastructure for libutf8 and the rationale behind each choice.

## Build systems

libutf8 provides two build systems that coexist in the repository:

### Makefile (primary, Unix)

The `Makefile` is the primary build system for Unix-like platforms (Linux, macOS, BSD). It builds:

- `libutf8.a` — static library
- `libutf8.so.X.Y.Z` (Linux) or `libutf8.X.dylib` (macOS) — shared library with soname/install_name versioning
- `bin/utf8_facts` — test binary

Standard targets: `all`, `check`, `install`, `uninstall`, `clean`, `expected`.

All install targets respect the standard GNU variables (`DESTDIR`, `PREFIX`, `LIBDIR`, `INCLUDEDIR`) that distro packagers rely on. This is what makes `dpkg-buildpackage`, `abuild`, and Homebrew formulas work without patches.

**Why a Makefile and not just CMake?** For a single-source-file C library with zero dependencies, a Makefile is the simplest correct solution. Unix distro packagers (Debian, Alpine, Homebrew) are all comfortable with Makefiles. CMake adds a configure step and a build directory that is unnecessary overhead for the common case.

### CMakeLists.txt (alternative, cross-platform)

CMake is provided as an alternative build system, primarily for:

- **Windows** — where `make` and the POSIX toolchain are not standard
- **vcpkg and Conan** — the two largest C/C++ package managers, which strongly prefer or require CMake
- **IDE integration** — Visual Studio, CLion, and VS Code all have native CMake support

The CMake build produces the same artifacts as the Makefile, plus:

- CMake package config files (`utf8Config.cmake`, `utf8Targets.cmake`) enabling `find_package(utf8)` in downstream projects
- pkg-config `.pc` file (same as the Makefile produces)

**Why not CMake only?** CMake is the right choice for complex projects with many dependencies and platform-specific logic. For a library this small, it adds complexity (out-of-source builds, generator selection, cache variables) that the Makefile avoids. Both are maintained because they serve different audiences.

## Package manager support

### Debian / Ubuntu (`debian/`)

**Location:** `debian/`

**Why:** Debian packaging is the foundation for Ubuntu, Mint, Pop!_OS, and most cloud server distributions. The `debian/` directory contains everything `dpkg-buildpackage` needs to produce `.deb` files.

**Produces two packages:**

- `libutf8-1` — shared library only (what applications depend on at runtime)
- `libutf8-dev` — headers, static library, pkg-config file (what developers `apt install` to build against libutf8)

**How to build:**

```sh
dpkg-buildpackage -us -uc
```

**Key files:**

| File | Purpose |
|------|---------|
| `debian/control` | Package metadata, dependencies, descriptions |
| `debian/rules` | Build script (delegates to `make`) |
| `debian/changelog` | Version history (required by dpkg) |
| `debian/copyright` | License in Debian machine-readable format |
| `debian/source/format` | Declares `3.0 (native)` source format |
| `debian/libutf8-1.install` | Files that go into the runtime package |
| `debian/libutf8-dev.install` | Files that go into the dev package |

### Homebrew (macOS / Linux)

**Location:** `packaging/homebrew/utf8.rb`

**Why:** Homebrew is the dominant package manager on macOS and is widely used on Linux. A formula file is all that is needed.

**How to use:**

The formula can be hosted in a custom tap (a separate Git repository):

```sh
# Create a tap repository (one-time)
# Add packaging/homebrew/utf8.rb to it as Formula/utf8.rb

brew tap wmacevoy/utf8 https://github.com/wmacevoy/homebrew-utf8
brew install wmacevoy/utf8/utf8
```

Or install directly from the formula for testing:

```sh
brew install --formula packaging/homebrew/utf8.rb
```

**Note:** The formula uses CMake because Homebrew's `std_cmake_args` helper handles prefix, RPATH, and bottle creation automatically. The `sha256` field must be updated after creating a GitHub release tarball.

### Alpine Linux

**Location:** `packaging/alpine/APKBUILD`

**Why:** Alpine is the base image for most Docker containers. Its minimal footprint and musl libc make it the default for containerized deployments. An `APKBUILD` is Alpine's equivalent of a Debian `debian/rules`.

**How to build:**

```sh
# Copy APKBUILD to an abuild workspace
abuild -r
```

**Note:** The `sha512sums` field must be updated after creating a GitHub release tarball.

### Chocolatey (Windows)

**Location:** `packaging/choco/`

**Why:** Chocolatey is the most established package manager for Windows. It enables `choco install libutf8` on Windows machines.

**How to build:**

```powershell
cd packaging\choco
choco pack
choco install libutf8 --source .
```

**Requires:** CMake and a C compiler (MSVC or MinGW) installed on the build machine. The install script downloads the source, builds with CMake, and installs to the Chocolatey lib directory.

## Package managers NOT included (and why)

| Manager | Why not | What to do |
|---------|---------|------------|
| **vcpkg** | Port files live in the [vcpkg registry](https://github.com/microsoft/vcpkg), not upstream repos | Submit a port once the library is published; the `CMakeLists.txt` and `find_package` support is all vcpkg needs from upstream |
| **Conan** | Recipes live in [conan-center-index](https://github.com/conan-io/conan-center-index), not upstream repos | Submit a recipe; CMake support is sufficient |
| **RPM / Fedora** | `.spec` files are similar to Debian packaging; the Makefile `install` target provides everything a spec file needs | A packager can write the spec against the existing `make install` interface |
| **pkgsrc (BSD)** | Same as RPM — the Makefile `install` target is the interface | BSD porters write the Makefile fragment against `make install` |
| **Meson** | Solves the same problem as CMake with less Windows ecosystem support | Not worth maintaining a third build system |

## pkg-config

Both build systems generate a `utf8.pc` file from `utf8.pc.in`, enabling:

```sh
cc $(pkg-config --cflags utf8) -o myapp myapp.c $(pkg-config --libs utf8)
```

## Versioning

The version (`1.0.0`) is defined in two places:

- `Makefile` — `VERSION_MAJOR`, `VERSION_MINOR`, `VERSION_PATCH`
- `CMakeLists.txt` — `project(utf8 VERSION 1.0.0)`

Both must be updated together when releasing a new version. The shared library soname uses only the major version (`libutf8.so.1`), so minor and patch releases are ABI-compatible drop-in replacements.

## Release checklist

1. Update version in `Makefile` and `CMakeLists.txt`
2. Update `debian/changelog` with new version and changes
3. Tag the release: `git tag v1.0.0`
4. Create a GitHub release with the tag
5. Update `sha256` in the Homebrew formula and `sha512sums` in the APKBUILD with the tarball hash
6. Build and publish packages for each target platform
