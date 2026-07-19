# BroodOS

[![Support BroodOS on Buy Me a Coffee](https://img.shields.io/badge/Support-Buy%20Me%20a%20Coffee-FFDD00?logo=buymeacoffee&logoColor=000)](https://buymeacoffee.com/rf1705)

BroodOS is an unofficial homebrew port of **StarCraft** and **StarCraft: Brood War** for LG webOS TVs.

This repository contains no Blizzard game assets. A legally obtained copy of the original game data is required. BroodOS currently detects the classic files `StarDat.mpq`, `BrooDat.mpq`, and the optional `Patch_rt.mpq`; support for additional installation formats will be added as development progresses.

BroodOS is not affiliated with, endorsed by, or supported by Blizzard Entertainment. StarCraft, StarCraft: Brood War, and Blizzard Entertainment are trademarks or registered trademarks of Blizzard Entertainment, Inc.

## Project status

BroodOS is at the first bootable prototype stage. The current native SDL2 application:

- starts as a native LG webOS homebrew application;
- supports TV remote, mouse, and keyboard input through SDL2;
- searches for original MPQ files without bundling or copying them;
- can discover game data on internal storage and attached USB drives;
- displays useful storage and permission diagnostics on the TV.

The StarCraft engine is not integrated yet. The next milestone is to evaluate and adapt the GPL-2.0 StarClone engine while keeping original game data outside this repository and release packages.

## Original game data

Never commit, upload, or redistribute the original MPQ files.

For a USB drive, use this recommended layout:

```text
USB drive/
└── BroodOS/
    ├── StarDat.mpq
    ├── BrooDat.mpq
    └── Patch_rt.mpq        # optional, depending on game version
```

BroodOS searches common webOS USB mount points such as `/tmp/usb` recursively. It also checks:

- the path supplied with `--data-dir`;
- the `BROODOS_DATA_DIR` environment variable;
- the current directory;
- `/media/internal/broodos` and `/media/internal/BroodOS`;
- `/mnt/usb`.

On some webOS versions, native applications run inside a jail that hides `/tmp/usb`. Rooted TVs may require a bind or overlay mount that exposes the USB drive inside the application's jail. BroodOS reports inaccessible paths so this can be diagnosed without copying the MPQs into limited internal storage.

## Building on macOS or Linux

Dependencies:

- CMake 3.8 or newer
- a C++14 compiler
- `pkg-config`
- SDL2

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Run the desktop diagnostic app with an optional data directory:

```sh
./build/broodos --data-dir /path/to/your/StarCraft/files
```

For a non-graphical storage check, add `--scan-only`. It exits with status 0 when a complete pair of MPQs is found and status 2 otherwise.

## Building for LG webOS

Native TV builds use the unofficial [webOS Homebrew NDK](https://www.webosbrew.org/develop/guides/env-setup/) and SDL2. Set `WEBOS_NDK_ROOT` to the extracted and relocated SDK directory, then run:

```sh
export WEBOS_NDK_ROOT=/path/to/arm-webos-linux-gnueabi_sdk-buildroot
sh scripts/build-webos.sh
```

Packaging additionally requires `ares-package` from `@webos-tools/cli`. The result is `build-webos/org.webosbrew.broodos_0.1.0_arm.ipk`, which can be installed through webOS Dev Manager or the Homebrew Channel.

## License

BroodOS is intended to remain compatible with the GNU General Public License version 2.0 used by StarClone. No Blizzard assets are covered by this license or distributed by this project.

## Support the project

If you would like to support continued BroodOS development, you can [buy RF1705 a coffee](https://buymeacoffee.com/rf1705).
