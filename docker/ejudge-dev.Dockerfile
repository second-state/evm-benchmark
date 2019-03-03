FROM hydai/ejudge-base:latest

LABEL maintainer="hydai@skymizer.com"

# Copy ejudge into container
RUN mkdir -p /workdir
WORKDIR /workdir
ADD . /workdir

RUN git submodule update --init --recursive
RUN mkdir build \
  && cd build \
  && cmake .. \
  && make -j8

WORKDIR /tmp
RUN wget https://github.com/ethereum/aleth/releases/download/v1.5.2/aleth-1.5.2-linux-x86_64.tar.gz \
  && tar zxvf aleth-1.5.2-linux-x86_64.tar.gz

WORKDIR /workdir
RUN mkdir -p /workdir/lib \
  && cp /tmp/lib/libaleth-interpreter.so ./lib

WORKDIR /workdir
