FROM ubuntu:disco

# Install deps
RUN apt-get update -y && apt-get install -y \
cmake \
gcc \
g++ \
openssl \
libssl-dev
COPY ./extern /extern
WORKDIR /poco_build

RUN CXX=/bin/g++ cmake /extern/poco && make install

WORKDIR /build/cmake_build
COPY ./CMakeLists.txt /build/CMakeLists.txt
COPY ./CMake /build/CMAke
COPY ./src /build/src
#compile
RUN CXX=/bin/g++ cmake .. && make