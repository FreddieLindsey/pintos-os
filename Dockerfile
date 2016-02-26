FROM ubuntu
MAINTAINER Freddie Lindsey freddie.a.lindsey@gmail.com

# Install make for testing
RUN apt-get update && apt-get install -y make

# Install gcc for compilation
RUN apt-get update && apt-get install -y gcc

# Install time and qemu for running pintos
RUN apt-get update && apt-get install -y time
RUN apt-get update && apt-get install -y qemu

# Install for deployment purposes
RUN apt-get update && apt-get install -y git
RUN apt-get update && apt-get install -y curl

# Install ruby for running `run_test` script
RUN curl -o ruby.tar.gz https://cache.ruby-lang.org/pub/ruby/2.3/ruby-2.3.0.tar.gz
RUN tar -xvf ruby.tar.gz
RUN cd ruby* && ./configure
RUN cd ruby* && make
RUN cd ruby* && make install
RUN rm -rf ruby*

# Link the version of qemu we want to default to
RUN ln -sf $(which qemu-system-i386) /usr/bin/qemu

# Make a directory for the source
RUN mkdir -p /project/src/utils

# Add the utils to the ENV
ENV PATH /project/src/utils:$PATH
ENV PATH /pipeline/source/src/utils:$PATH

# Install tmux/gdb for debugging (multi-window)
RUN apt-get update && apt-get install -y tmux
RUN apt-get update && apt-get install -y gdb

# Tex for documentation
RUN apt-get --no-install-recommends install -y texlive texlive-latex-extra
