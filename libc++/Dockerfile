# Use Ubuntu 22.04 so we get version 10 of arm-none-eabi-gcc.
FROM ubuntu:22.04

LABEL maintainer="Tock Project Developers <tock-dev@googlegroups.com>"
LABEL version="0.1"
LABEL description="Dockerfile to build libtock-c libc++."

# Disable Prompt During Packages Installation
ARG DEBIAN_FRONTEND=noninteractive
# Update Ubuntu Software repository
RUN apt update

# Install our toolchains
RUN apt install -y gcc-arm-none-eabi gcc-riscv64-unknown-elf

# Install needed tools
RUN apt install -y git build-essential wget rsync zip

# Install needed library
RUN apt install -y libmpc-dev file

# Clone the libtock-c source so we can use the build scripts
RUN git clone https://github.com/tock/libtock-c --depth 1 -b make-precompiled
RUN cd libtock-c && git fetch && git checkout eb6f45f00f5103468ee865a112332f50b6089caf

# Actually build the toolchain
RUN cd libtock-c/libc++ && make
