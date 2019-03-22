### Base environment container ###
# Get the base Ubuntu image from Docker Hub
FROM ubuntu:latest as base

# Update the base image and install build environment
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    curl \
    libboost-log-dev \
    libboost-regex-dev \
    libboost-signals-dev \
    libboost-system-dev \
    libgtest-dev \
    netcat \
    sqlite3 \
    libsqlite3-dev

# install pip and gcovr
RUN curl https://bootstrap.pypa.io/get-pip.py -o get-pip.py
RUN python get-pip.py
RUN pip install gcovr
