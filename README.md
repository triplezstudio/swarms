# swarms

# What is this project about?

This project aims at experimenting with multiagents based simulation and turn it into a game.

# Installation

## Ubuntu

If you do go through with it, please consider contributing the guide as an issue to help fellow developers!

## Prerequisites

This projects uses (among other things):

- [cmake](https://cmake.org/) as a build management system
- [ccache](https://ccache.dev/) as a wrapper to speed up the building process
- [google test](https://github.com/google/googletest) as a testing library
- [rsync](https://linux.die.net/man/1/rsync) for file management
- [sdl2](https://wiki.libsdl.org/SDL2/Installation) for window and input management

## TL; DR

For a quick setup, just run the following commands to install all needed dependencies:

```bash
apt update

# Skip this if you already have the basics installed
apt install -y --no-install-recommends \
  cmake \
  build-essential \
  ccache

export DEBIAN_FRONTEND="interactive"

apt-get install -y --no-install-recommends \
  libgtest-dev \
  libgmock-dev \
  rsync \
  libsdl2-dev
```

**Note:** this project uses `-fprofile-update=atomic` as a profiling option to handle concurrency during profiling. This requires ccache [4.10.0](https://ccache.dev/releasenotes.html#_ccache_4_10) at least. In case your system comes with an older version it means that you won't benefit from having `ccache`.

**Note:** this project uses cpp20 and relies on features such as concepts. This should be supported by gcc10 (see [source](https://gcc.gnu.org/projects/cxx-status.html)). The development was made using gcc13. In case your version of gcc is older than this, refer to [this guide](https://www.dedicatedcore.com/blog/install-gcc-compiler-ubuntu/) to get an idea on how to install a more recent version.

## Windows
Tested to run on Windows11. 

## Prerequisites
- [vcpkg](https://github.com/Microsoft/vcpkg.git) as a windows package manager (tested with version 2026-03-04-4b3e4c276b5b87a649e66341e11553e8c577459c)
- [cmake](https://cmake.org/) as a build management system
- [ccache](https://ccache.dev/) as a wrapper to speed up the building process
- [google test](https://github.com/google/googletest) as a testing library
- [sdl2](https://wiki.libsdl.org/SDL2/Installation) for window and input management
- robocopy

```powershell
.\vcpkg install gtest:x64-windows

.\vcpkg install sdl2[vulkan]:x64-windows


```
CCache is best installed by downloading from https://ccache.dev/download.html
and extracting to a local folder. Make sure this folder is on your system PATH.


## Build flags

Set the path to vcpkg explicitely or add to your systemwide PATH. \
Example for providing the path with the build command:
```powershell
cmake -B build -S . `
  -DENABLE_TESTS=ON `
  -DCMAKE_TOOLCHAIN_FILE="F:/DevTools/vcpkg/scripts/buildsystems/vcpkg.cmake"
```

## Clone the repository

The first step (as is often the case) if you want to work on this project is to clone the repository with:

```bash
git clone git@github.com:triplezstudio/swarms.git
# Move to the project's folder
cd swarms
```
