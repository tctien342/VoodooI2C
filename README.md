# Better Scroll VoodooI2C
> Fork of [VoodooI2C](https://github.com/alexandred/VoodooI2C/tree/master/VoodooI2C) to enhance scroll with the trackpad

<br><br>

## Disclaimer
This is a fork of the awesome [VoodooI2C](https://github.com/alexandred/VoodooI2C/tree/master/VoodooI2C) by [Alexandre Daoud](https://github.com/alexandred).

Ensure that the original VoodooI2C is working on your setup before using this fork. This fork only exists for cosmetic changes.

:warning: __Please, do not post issues on the original VoodooI2C repo concerning this fork. alexandred and the contributors of VoodooI2C have nothing to do with it.__ :warning:

I will not provide release of the kexts to avoid confusion with the original VoodooI2C.kext and its satellites.

I made this fork especially for my Xiaomi Notebook Pro 15" (8th gen i7 and 16gb ram).
I only tested it on my own configuration, and this plugin is provided "as is" with no guarantee. Use it at your own risk!

I know almost nothing about GPIO / ACPI / DSDT related issues and will not be able to provide any help for this.

<br><br>

## Changes to the original VoodooI2C

> Almost all changes are located in `csgesturescroll.h` and `VoodooI2CCSGestureEngine.cpp`. They all are prefixed by a comment starting with `// @pqml fork change`.

- Remove 3/4 fingers gestures
  - They are a bit buggy and the key stroke emulation is sometimes in conflict with the real keyboard inputs
- Touching the trackpad with 2 fingers stop the scroll
- More precise deceleration of the momentum. The "linear progression" at the end of the momentum is attenuated.
- More consistant initial momentum after the touchend
- No more treshold for the scroll activation
  - Little two-fingers movements on the trackpad trigger the scroll too
- I also remove documentation and circleci from the fork
  - I don't use CI and I only did minor changes to the VoodooI2C core, so there is no need to duplicate the doc from the original repo

<br>

## Setup and Build

### 1. Requirements
- MacOS >= 10.11
- Xcode Command Line Tools
- Python 3 (for `cpplint`)

###### Xcode Installation

```sh
$ xcode-select --install
```

###### Python Installation

- Install [brew](https://brew.sh/)
- Use brew to install python: `brew install python`
- Add this line to `~/.bash_profile` (and `~/.zshrc` if needed):
  - `export PATH="/usr/local/opt/python/libexec/bin:$PATH"`


### 2. Clone the repository

```sh
$ git https://github.com/pqml/VoodooI2C.git
$ cd VoodooI2C
```

### 3. Initialization

Use the `init.sh` script to install `cpplint`, the submodules and the 10.11 OSX SDK

```sh
$ sh init.sh
```

### 4. Build

Use the `build.sh` script to build the kext into the `build` folder.

```sh
$ sh build.sh
```

You can then copy/paste the compiled `build/VoodooI2C.kext` and one of its satellites to your EFI partition or to `System/Library/Extensions` using a kext utility. 

<br><br>

## License

This program is protected by the GPL license. Please refer to the `LICENSE.txt` file for more information
