FROM ubuntu:22.04 as builder.base

#prepare dependencies
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
    gcc-11 \
    g++-11 \
    pkg-config \
    libstdc++6 \
    libsystemd-dev \
    libi2c-dev \
    ninja-build \
    git \
    python3-pip \
    && \
    pip3 install meson

#prepare dependencies
RUN update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-11 100 && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 100

FROM builder.base as builder

ADD . /source

RUN cd source && \
    meson setup --buildtype=release build && \
    meson compile -C build

FROM builder as unit-tests

ENTRYPOINT /source/build/tests/scm-i2c-memory-map-ut