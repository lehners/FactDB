FROM ubuntu:25.04 AS base

# The compiler versions
ENV CLANG_VERSION=19
ENV GCC_VERSION=13
ENV RE2_VERSION=11

# Docker container to build repos
FROM base AS build
RUN apt-get update -qq \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y tzdata \
    && apt-get install -y \
        bison \
        build-essential \
        ccache \
        clang-${CLANG_VERSION} \
        clang-19 \
        clang-format-${CLANG_VERSION} \
        clang-format-19 \
        clang-tools \
        cmake \
        curl \
        flex \
        g++-${GCC_VERSION} \
        git \
        gnupg \
        libasan6 \
        libboost-context-dev \
        libboost-dev \
        libboost-system-dev \
        libclang-19-dev \
        libfmt-dev \
        libgtest-dev \
        libjemalloc-dev \
        libpq-dev \
        libre2-dev \
        libssl-dev \
        libtbb-dev \
        libtinfo-dev \
        liburing-dev \
        lld-${CLANG_VERSION} \
        llvm-${CLANG_VERSION} \
        make \
        mold \
        ninja-build \
        python-is-python3 \
        python3 \
        python3-distutils-extra \
        python3-lxml \
        libsqlite3-dev \
        wget \
        unzip \
        zstd \
        xz-utils \
    && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-14 14 \
    && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 14 \
    && update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-19 19 \
    && update-alternatives --install /usr/bin/clang clang /usr/bin/clang-19 19 \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

RUN mkdir -p -m 0600 ~/.ssh && ssh-keyscan gitlab.db.in.tum.de >> ~/.ssh/known_hosts

WORKDIR /factdb
ARG REPO_FACTDB=git@gitlab.db.in.tum.de:lehners/factdb.git
ARG BRANCH=main
RUN --mount=type=ssh git clone $REPO_FACTDB . && git checkout $BRANCH

# Build FactDB
WORKDIR /factdb/cmake-build-release
RUN set -eux \
   && cmake -DCMAKE_BUILD_TYPE=Release .. \
   && LDFLAGS='' make -j$(nproc) benchAll benchArtificial \
   && strip benchAll benchArtificial


RUN echo "Generating CE database $DBFILE"
WORKDIR /factdb/bench/ce/data
RUN echo '2c3aaaa766930aee95e8c5d8e0b10c4526f960c585414f873a31070ee3049add  cebench.tar.zst' | sha256sum --check --status 2>/dev/null || curl -OL https://db.in.tum.de/~birler/dbgen/cebench.tar.zst
RUN echo '2c3aaaa766930aee95e8c5d8e0b10c4526f960c585414f873a31070ee3049add  cebench.tar.zst' | sha256sum --check --status
RUN tar --skip-old-files -xf cebench.tar.zst

# generate SSB dataset
RUN echo "Generating Star Schema Benchmark database $DBFILE with scale factor $SF"
# RUN mkdir -p "scripts/ssb/data/sf$SF"
# RUN cd "scripts/ssb/data/"

# Originally from: https://www.cs.umb.edu/%7Eponeil/dbgen.zip
WORKDIR /factdb/bench/ssb
RUN echo '284118b10f066fd0d5b8e4e77b150d0b6176510f84a2f54f93c3a439ac312bc3  dbgen.zip' | sha256sum --check --status 2>/dev/null || curl -OL https://db.in.tum.de/~fent/dbgen/ssb/dbgen.zip
RUN echo '284118b10f066fd0d5b8e4e77b150d0b6176510f84a2f54f93c3a439ac312bc3  dbgen.zip' | sha256sum --check --status
RUN unzip -q -u dbgen.zip
RUN mkdir -p "/factdb/bench/ssb/data"
RUN mkdir -p "/factdb/bench/ssb/data/sf1"
RUN mkdir -p "/factdb/bench/ssb/data/sf10"

WORKDIR /factdb/bench/ssb/dbgen
RUN patch -p5 < /factdb/bench/ssb/ssb.patch
RUN sed -i 's/#define  MAXAGG_LEN    10/#define  MAXAGG_LEN    20/' shared.h
RUN sed -i 's/|O_LARGEFILE//' bm_utils.c
# fix include
RUN sed -i 's/<malloc.h>/<stdlib.h>/' bm_utils.c
RUN MACHINE="LINUX" make -sj "$(nproc)" dbgen
RUN ./dbgen -f -T c -s 1
RUN ./dbgen -qf -T d -s 1
RUN ./dbgen -qf -T p -s 1
RUN ./dbgen -qf -T s -s 1
RUN ./dbgen -q -T l -s 1
RUN for table in ./*.tbl; do \
      sed 's/|$//' "$table" >"../data/sf1/$table"; \
    done
RUN rm -rf ./*.tbl
# and same for SF10
RUN ./dbgen -f -T c -s 10
RUN ./dbgen -qf -T d -s 10
RUN ./dbgen -qf -T p -s 10
RUN ./dbgen -qf -T s -s 10
RUN ./dbgen -q -T l -s 10
RUN for table in ./*.tbl; do \
      sed 's/|$//' "$table" >"../data/sf10/$table"; \
    done
RUN rm -rf ./*.tbl

# maybe we also have to load the SSB dataset here, but it should work without for now.

COPY ./factDB/gen/estimates.db /factdb/factDB/gen/estimates.db

WORKDIR /factdb/cmake-build-release

# generate queries and rebuild the executable
RUN ./benchArtificial -tdiagonalUniform -mCodegenFlat -r1 -w0 -p -a -g
RUN ./benchArtificial -tdiagonalUniform -mCodegenFactorized -r1 -w0 -p -a -b
RUN ./benchArtificial -tdiagonalUniform -mCodegenFactorized -r1 -w0 -p -a

# factorized
RUN ./benchAll ce -mCodegenFactorized -r1 -w0 -pv -cib
# factorized with naive merging strategy
RUN ./benchAll ce -mCodegenFactorized -n -r1 -w0 -pv -cib
# flat
RUN ./benchAll ce -mCodegenFlat -r1 -w0 -pv

# and the same for SSB...
# factorized
RUN ./benchAll ssb_sf1 -mCodegenFactorized -r1 -w0 -pv -cib
RUN ./benchAll ssb_sf10 -mCodegenFactorized -r1 -w0 -pv -cib
# factorized & naive
RUN ./benchAll ssb_sf1 -mCodegenFactorized -n -r1 -w0 -pv -cib
RUN ./benchAll ssb_sf10 -mCodegenFactorized -n -r1 -w0 -pv -cib
# flat
RUN ./benchAll ssb_sf1 -mCodegenFlat -r1 -w0 -pv
RUN ./benchAll ssb_sf10 -mCodegenFlat -r1 -w0 -pv

# rebuild all
RUN make -j$(nproc) benchAll benchArtificial

RUN strip benchAll benchArtificial

FROM base AS runtime

RUN apt-get update -qq \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y python3 libtbb-dev libpq-dev \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

COPY --from=build /factdb/cmake-build-release/benchAll /factdb/cmake-build-release/benchAll
COPY --from=build /factdb/cmake-build-release/benchArtificial /factdb/cmake-build-release/benchArtificial

COPY --from=build /factdb/bench/ce/load.sql /factdb/bench/ce/load.sql
COPY --from=build /factdb/bench/ssb/load_sf1.sql /factdb/bench/ssb/load_sf1.sql
COPY --from=build /factdb/bench/ssb/load_sf10.sql /factdb/bench/ssb/load_sf10.sql

COPY docker/factpaper_bench/entrypoint.py /entrypoint.py

WORKDIR /factdb
ENTRYPOINT ["python3", "/entrypoint.py"]
