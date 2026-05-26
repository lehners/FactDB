FROM ubuntu:24.10

# Install apt dependencies
RUN apt-get update -qq \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y tzdata \
    && apt-get install -y \
        bison \
        ccache \
        clang-19 \
        clang-format-19 \
        clang-tools \
        cmake \
        g++-14 \
        git \
        gnupg \
        libasan6 \
        libclang-19-dev \
        make \
        ninja-build \
        python3 \
        python-is-python3 \
        python3-lxml \
        python3-distutils-extra \
        wget \
        curl \
        xz-utils \
        mold \
        libgtest-dev \
        libfmt-dev \
        libtbb-dev \
        libpq-dev \
        flex \
    && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-14 14 \
    && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 14 \
    && update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-19 19 \
    && update-alternatives --install /usr/bin/clang clang /usr/bin/clang-19 19 \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*


