FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

ARG HOME=/home/dev
ENV HOME=${HOME}

WORKDIR ${HOME}

RUN apt-get update && apt-get install -y --no-install-recommends \
      ca-certificates \
      wget \
      gnupg2 \
      lsb-release \
      software-properties-common \
    && add-apt-repository -y ppa:ubuntu-toolchain-r/test \
    && apt-get update

RUN apt-get install -y --no-install-recommends \
      build-essential \
      make \
      git \
      libssl-dev \
      binutils \
      gcc-15 \
      g++-15 \
      ccache \
      lld \
    && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-15 150 \
    && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-15 150 \
    && update-alternatives --set gcc /usr/bin/gcc-15 \
    && update-alternatives --set g++ /usr/bin/g++-15 \
    && rm -rf /var/lib/apt/lists/*

COPY . ${HOME}/project/
