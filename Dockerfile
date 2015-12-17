
FROM ubuntu:15.04
MAINTAINER Jay Kamat github@jgkamat.33mail.com

# A simple dockerimage for running complx 
#
# This dockerfile simply builds complx from source inside a dockerfile

# Setup apt to be happy with no console input
ENV DEBIAN_FRONTEND noninteractive

# Use UTF-8
RUN locale-gen en_US.UTF-8
ENV LANG en_US.UTF-8

# setup apt tools and other goodies we want
RUN apt-get update --fix-missing && apt-get -y install make apt-utils wget curl htop iputils-ping vim-nox less debconf-utils git software-properties-common sudo tmux && apt-get clean

# set up user <this is for running soccer later on>
# Replace 1000 with your user / group id
RUN export uid=1000 gid=1000 && \
    mkdir -p /home/developer && \
    echo "developer:x:${uid}:${gid}:Developer,,,:/home/developer:/bin/bash" >> /etc/passwd && \
    echo "developer:x:${uid}:" >> /etc/group && \
    echo "developer ALL=(ALL) NOPASSWD: ALL" > /etc/sudoers.d/developer && \
    chmod 0440 /etc/sudoers.d/developer && \
    chown ${uid}:${gid} -R /home/developer

USER developer
ENV HOME /home/developer

# do everything in root's home
RUN mkdir -p /home/developer/complx
WORKDIR /home/developer/

ADD . complx
WORKDIR /home/developer/complx

RUN sudo apt-get update --fix-missing && sudo apt-get install -y g++ build-essential libwxgtk3.0-dev && sudo apt-get clean
RUN sudo chmod +x install.sh
RUN sudo ./install.sh

CMD [ "complx" ]
