# Start with ubuntu 14.04 image
FROM ubuntu:14.04

# Bring the image up to date and install software
RUN sudo apt-get update -y
RUN sudo DEBIAN_FRONTEND=noninteractive apt-get upgrade -y
RUN sudo DEBIAN_FRONTEND=noninteractive apt-get install -y automake \
	autoconf g++ libboost-all-dev libgcrypt20-dev pkg-config git \
	libtool make libsigc++-2.0-dev libbullet-dev libavahi-core-dev \
	libavahi-compat-libdnssd-dev libpq-dev libreadline-dev \
	libncurses5-dev postgresql bash

#Use hammer to install cyphesis
RUN git clone https://github.com/worldforge/hammer
RUN cd hammer
RUN bash hammer.sh checkout libs
RUN bash hammer.sh build libs
RUN bash hammer.sh checkout cyphesis
RUB bash hammer.sh build cyphesis


#Setup Postgres
RUN sudo -u postgres createuser -D -A ubuntu
RUN sudo -u postgres createdb -O ubuntu cyphesis

#Run cyphesis
CMD cyphesis

#Open port 6767 for cyphesis
EXPOSE 6767
