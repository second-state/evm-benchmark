FROM ubuntu:16.04

LABEL maintainer="hydai@skymizer.com"

# Install base tool
RUN apt-get update \
  && apt-get install -y wget git build-essential
# Install add-apt-repository
RUN apt-get install -y python-software-properties software-properties-common

# Install GCC 8
RUN add-apt-repository ppa:ubuntu-toolchain-r/test \
  && apt-get update \
  && apt-get install -y gcc-8 g++-8

# Setup compilers
RUN update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-8 1 \
  && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 1 \
  && update-alternatives --install /usr/bin/cc cc /usr/bin/gcc 30 \
  && update-alternatives --set cc /usr/bin/gcc \
  && update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++ 30 \
  && update-alternatives --set c++ /usr/bin/g++

# Install Solc
RUN add-apt-repository ppa:ethereum/ethereum \
  && apt-get update \
  && apt-get install solc

# Cleanup
RUN apt-get autoremove --purge -y \
  && apt-get autoclean -y \
  && rm -rf /var/cache/apt/* /tmp/*

# Copy ejudge into container
RUN mkdir -p /workdir
WORKDIR /workdir
