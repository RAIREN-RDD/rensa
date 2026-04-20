FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      ca-certificates \
      wget \
      gnupg2 \
      lsb-release \
      software-properties-common \
      build-essential \
      make \
      git && \
    add-apt-repository -y ppa:ubuntu-toolchain-r/test && \
    apt-get update && \
    apt-get install -y --no-install-recommends gcc-15 g++-15 && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-15 150 && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-15 150 && \
    update-alternatives --set gcc /usr/bin/gcc-15 && \
    update-alternatives --set g++ /usr/bin/g++-15 && \
    rm -rf /var/lib/apt/lists/*

ARG HOME=/home/dev
ENV HOME=${HOME}

WORKDIR ${HOME}
COPY . ${HOME}/project/
