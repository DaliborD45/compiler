FROM ubuntu:latest
LABEL authors="dalibord"

RUN apt-get update && apt-get install -y \
    clang \
    cmake \
    build-essential
WORKDIR /app
COPY . .

RUN cmake -S . -B build -DCMAKE_CXX_COMPILER=clang++
RUN cmake --build build

CMD ["./cmake-build-debug/compiler"]