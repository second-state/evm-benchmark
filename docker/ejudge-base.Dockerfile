FROM hydai/workenv:latest

LABEL maintainer="hydai@skymizer.com"

# Remove cmake
RUN apt-get remove --purge --auto-remove cmake

# Install cmake 3.13.4
WORKDIR /tmp
RUN wget https://github.com/Kitware/CMake/releases/download/v3.13.4/cmake-3.13.4.tar.gz \
  && tar -xzvf cmake-3.13.4.tar.gz
WORKDIR /tmp/cmake-3.13.4
RUN ./bootstrap \
  && make -j8 \
  && make install

# Cleanup
RUN apt-get autoremove --purge -y \
  && apt-get autoclean -y \
  && rm -rf /var/cache/apt/* /tmp/*
RUN mkdir -p /workdir
WORKDIR /workdir
