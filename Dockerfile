FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    gcc-14 \
    g++-14 \
    cmake \
    ninja-build \
    git \
    libncurses-dev \
    libboost-dev \
    libicu-dev \
    python3 \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

ENV CC=/usr/bin/gcc-14
ENV CXX=/usr/bin/g++-14

WORKDIR /app

CMD ["/bin/bash"]