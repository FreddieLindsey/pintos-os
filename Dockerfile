FROM ubuntu
MAINTAINER Freddie Lindsey freddie.a.lindsey@gmail.com

# Update repos
RUN apt-get update

# Install make for testing
RUN apt-get install -y make

# Install gcc for compilation
RUN apt-get install -y gcc

# Install time and qemu for running pintos
RUN apt-get install -y time
RUN apt-get install -y qemu

# Install for deployment purposes
RUN apt-get install -y git
RUN apt-get install -y curl

# Link the version of qemu we want to default to
RUN ln -sf $(which qemu-system-i386) /usr/bin/qemu

# Make a directory for the source
RUN mkdir -p /project/src/utils

# Add the utils to the ENV
ENV PATH /project/src/utils:$PATH
