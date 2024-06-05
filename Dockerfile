# Use the latest alpine image with GCC
FROM alpine:latest

# Install necessary packages
RUN apk add --no-cache g++
RUN apk add --no-cache make
RUN apk add --no-cache cmake

# Set the working directory
WORKDIR /usr/src/app

COPY main.cpp CMakeLists.txt ./
COPY deps ./deps

# Compile the C++ code
RUN mkdir build
RUN cd build && cmake .. && cmake --build .

# Set the entrypoint to the compiled binary
ENTRYPOINT ["./build/silo"]
