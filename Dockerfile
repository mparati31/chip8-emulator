FROM archlinux:latest

RUN pacman -Syu --noconfirm
RUN pacman -S --noconfirm \
    clang \
    cmake \
    git \
    ninja \
    wget \
    mesa \
    libx11 \
    libxext \
    libxrandr \
    libxcursor \
    libxinerama \
    libxi \
    wayland \
    wayland-protocols \
    libxkbcommon
RUN pacman -Scc --noconfirm

ENV CC=clang
ENV CXX=clang++
ENV SDL_AUDIODRIVER=dummy

WORKDIR /workspace

COPY . .

RUN cmake -B build \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++ && \
    cmake --build build

WORKDIR /app

RUN cp /workspace/build/Chip8Emulator .

ENTRYPOINT ["/app/Chip8Emulator"]

CMD ["--help"]
