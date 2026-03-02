# Packaging

This document explains the packaging infrastructure for libutf8 and the rationale behind each choice.

## Build systems

libutf8 provides two build systems that coexist in the repository:

### Makefile (primary, Unix)

The `Makefile` is the primary build system for Unix-like platforms (Linux, macOS, BSD).

**The library itself has zero dependencies.** The test framework
([wmacevoy/facts](https://github.com/wmacevoy/facts)) is the only
external dependency and is only needed for `make check`. It is included
as a git submodule at `vendor/facts` but is not required for building or
installing the library.

| Target | What it builds | Needs `vendor/facts`? |
|--------|---------------|----------------------|
| `make` / `make all` | `libutf8.a` + shared library | No |
| `make install` | Installs libs, header, pkg-config | No |
| `make check` | Builds and runs test binary (`bin/utf8_facts`) | Yes |
| `make expected` | Refreshes golden test output | Yes |
| `make clean` | Removes all build artifacts | No |

All install targets respect the standard GNU variables (`DESTDIR`, `PREFIX`, `LIBDIR`, `INCLUDEDIR`) that distro packagers rely on. This is what makes `dpkg-buildpackage`, `abuild`, and Homebrew formulas work without patches.

**For users:** `git clone` + `make` + `make install` works without the submodule.

**For developers:** `git clone --recurse-submodules` (or `git submodule update --init` after cloning) to get the test framework, then `make check`.

**Why a Makefile and not just CMake?** For a single-source-file C library with zero dependencies, a Makefile is the simplest correct solution. Unix distro packagers (Debian, Alpine, Homebrew) are all comfortable with Makefiles. CMake adds a configure step and a build directory that is unnecessary overhead for the common case.

### CMakeLists.txt (alternative, cross-platform)

CMake is provided as an alternative build system, primarily for:

- **Windows** — where `make` and the POSIX toolchain are not standard
- **vcpkg and Conan** — the two largest C/C++ package managers, which strongly prefer or require CMake
- **IDE integration** — Visual Studio, CLion, and VS Code all have native CMake support

The CMake build produces the same artifacts as the Makefile, plus:

- CMake package config files (`utf8Config.cmake`, `utf8Targets.cmake`) enabling `find_package(utf8)` in downstream projects
- pkg-config `.pc` file (same as the Makefile produces)

Tests are opt-in via `-DUTF8_BUILD_TESTS=ON` and require the `vendor/facts` submodule. The default CMake build (like the default Makefile build) only produces the libraries.

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

The Debian build only builds libraries (`make all`) and does not run tests, since the `vendor/facts` submodule is a test-only dependency that is not a Debian build-dep. Tests are run separately in CI before the packaging jobs start.

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

## CI/CD — GitHub Actions release workflow

**Location:** `.github/workflows/release.yml`

Pushing a version tag (`v*`) triggers the release workflow, which:

1. **Tests** — checks out with submodules, runs `make check` (the only step that needs `vendor/facts`)
2. **Builds .deb** — `dpkg-buildpackage` on Ubuntu (builds libraries only, no test dependency)
3. **Builds .apk** — `abuild` in an Alpine Docker container (builds libraries only via CMake)
4. **Builds Windows zip** — CMake + MSVC on `windows-latest` (builds libraries only)
5. **Creates GitHub Release** — uploads all artifacts with install instructions and the source tarball `sha256`
6. **Publishes apt repo** — (optional) deploys a signed apt repository to GitHub Pages via `reprepro`

The packaging jobs (2-4) depend on the test job passing first but do not themselves need the test framework. This keeps the test dependency (`vendor/facts`) out of the packaging pipeline.

### apt repo via GitHub Pages (optional setup)

The `apt-repo` job is gated behind the repository variable `ENABLE_APT_REPO=true`. When enabled, it publishes a signed Debian repository to GitHub Pages so users can install with:

```sh
# Download the signing key
curl -fsSL https://wmacevoy.github.io/utf8/utf8.gpg \
  | sudo tee /usr/share/keyrings/utf8.gpg > /dev/null

# Add the repository
echo "deb [signed-by=/usr/share/keyrings/utf8.gpg] \
  https://wmacevoy.github.io/utf8 stable main" \
  | sudo tee /etc/apt/sources.list.d/utf8.list

sudo apt update && sudo apt install libutf8-dev
```

**One-time setup:**

1. Generate a GPG signing key:

   ```sh
   gpg --batch --gen-key <<EOF
   %no-protection
   Key-Type: RSA
   Key-Length: 4096
   Name-Real: libutf8 repo signing key
   Name-Email: wmacevoy@gmail.com
   Expire-Date: 0
   EOF
   ```

2. Export the private key and add it as the repository secret `GPG_PRIVATE_KEY`:

   ```sh
   gpg --armor --export-secret-keys
   # Copy output → GitHub repo Settings → Secrets → GPG_PRIVATE_KEY
   ```

3. Export the public key and commit it to the repository so users can download it:

   ```sh
   gpg --armor --export > apt/utf8.gpg
   ```

4. Enable GitHub Pages: Settings → Pages → Source: **GitHub Actions**

5. Set the repository variable `ENABLE_APT_REPO` to `true`: Settings → Variables → Actions → New variable

### Homebrew tap

The Homebrew formula lives in `packaging/homebrew/utf8.rb`. To serve it to users:

1. Create a separate repository: `wmacevoy/homebrew-utf8`
2. Copy `packaging/homebrew/utf8.rb` to `Formula/utf8.rb` in that repo
3. After each release, update the `sha256` in the formula with the value printed in the GitHub Release notes

Users then install with:

```sh
brew tap wmacevoy/utf8
brew install utf8
```

## Release checklist

1. Update version in `Makefile`, `CMakeLists.txt`, and `debian/changelog`
2. Commit, tag, and push:
   ```sh
   git tag v1.0.0
   git push origin v1.0.0
   ```
3. The release workflow runs automatically and creates the GitHub Release
4. Update the Homebrew formula `sha256` with the value from the release notes
5. (If using apt repo) Verify the Pages deployment at `https://wmacevoy.github.io/utf8`
