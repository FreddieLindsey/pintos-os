FROM ubuntu
MAINTAINER Freddie Lindsey freddie.a.lindsey@gmail.com

# Update repos
RUN apt-get update

# Install time and qemu for running pintos
RUN apt-get install -y time
RUN apt-get install -y qemu

# Install git for deployment purposes
RUN apt-get install -y git

# Link the version of qemu we want to default to
RUN ln -sf $(which qemu-system-i386) /usr/bin/qemu

# Make a directory for the source
RUN mkdir -p project

# Add the required files
ADD Makefile project/
ADD src project/src
ADD tests project/tests
