FROM ubuntu
MAINTAINER Freddie Lindsey freddie.a.lindsey@gmail.com

# Update repos
RUN apt-get update

# Install time and qemu for running pintos
RUN apt-get install -y time
RUN apt-get install -y qemu

# Link the version of qemu we want to default to
RUN ln -sf $(which qemu-system-i386) /usr/bin/qemu

# Install git for deployment purposes
RUN apt-get install -y git
