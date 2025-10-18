# CHIP-8 Emulator

A simple [CHIP-8](https://en.wikipedia.org/wiki/CHIP-8) emulator written in modern C++ using [Simple DirectMedia Layer](https://www.libsdl.org/) as graphics library.

The emulator is available as a [release](https://github.com/mparati31/chip8-emulator/releases), via [Docker](#docker), or can be [built manually](#build).

## Docker

To run the emulator using a Docker image:

```bash
docker run --rm \
    -v <path-to-roms>:/roms \
    ghcr.io/mparati/chip8-emulator:<version> \
    /roms/<rom-path>
```

Where `<version>` is the desired release tag from the [container registry](https://github.com/mparati31/chip8-emulator/pkgs/container/chip8-emulator), such as `latest` or a specific version. `<path-to-roms>` is the absolute path to the directory on your machine containing the ROM files and `<rom-path>` is the filename or relative path of the ROM inside `/roms` (usually a `.ch8` file).

For example, to run the PONG ROM from the [kripod/chip8-roms](https://github.com/kripod/chip8-roms) rom collection repository:

```bash
git clone https://github.com/kripod/chip8-roms.git
docker run --rm \
    -v $(pwd)/chip8-roms:/roms \
    ghcr.io/mparati/chip8-emulator:latest \
    /roms/games/PONG.ch8
```

## Build

To build the emulator from source, ensure you have a compiler that supports C++23:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

The resulting executable will be located at `build/Chip8Emulator`.

If you encounter compiler errors, try specifying the compiler explicitly, for example with clang:

```bash
cmake -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++
cmake --build build
```

## References

- [CHIP-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
- [Test Suite Used](https://github.com/Timendus/chip8-test-suite)
- [Collection of ROMs](https://github.com/kripod/chip8-roms)
