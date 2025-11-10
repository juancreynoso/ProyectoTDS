FROM --platform=linux/amd64 ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
   && apt-get install -y --no-install-recommends \
   build-essential \
   bison \
   flex \
   gdb \
   ca-certificates \
   wget \
   curl \
   vim \
   && apt-get clean \
   && rm -rf /var/lib/apt/lists/*

RUN useradd -m dev && echo "dev:dev" | chpasswd && adduser dev sudo
USER dev
WORKDIR /home/dev


CMD ["/bin/bash"]
