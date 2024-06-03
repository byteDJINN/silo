# Use the latest alpine image with GCC
FROM alpine:latest

# Install necessary packages
RUN apk add --no-cache g++ make

# Set the working directory
WORKDIR /usr/src/app

COPY silo.cpp ./

# Compile the C++ code
RUN g++ -o silo silo.cpp

# Set the entrypoint to the compiled binary
ENTRYPOINT ["./silo"]
