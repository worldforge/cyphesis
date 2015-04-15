# Start with ubuntu 14.04 image
FROM ubuntu:14.04
MAINTAINER Erik Ogenvik <erik@ogenvik.org>

# Bring the image up to date and install software
RUN apt-get update -y
RUN DEBIAN_FRONTEND=noninteractive apt-get upgrade -y
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y automake \
	autoconf g++ libboost-all-dev libgcrypt-dev pkg-config git \
	libtool make libsigc++-2.0-dev libbullet-dev libavahi-core-dev \
	libavahi-compat-libdnssd-dev libpq-dev libreadline-dev \
	libncurses5-dev postgresql bash libcurl4-openssl-dev \
	curl rsync

#Setup user for building; we don't want to use root for that
RUN groupadd -r worldforge && useradd -m -g worldforge worldforge

#Setup Postgres
RUN service postgresql start && \
	sudo -u postgres createuser -D -A worldforge && \
	sudo -u postgres createdb -O worldforge cyphesis && \
	service postgresql stop


#Use hammer to install cyphesis
USER worldforge

RUN mkdir -p /home/worldforge/hammer 
WORKDIR /home/worldforge/hammer

RUN git clone https://github.com/worldforge/hammer .
RUN bash hammer.sh checkout libs
RUN bash hammer.sh checkout cyphesis
RUN bash hammer.sh build libs
RUN bash hammer.sh build cyphesis
RUN bash hammer.sh checkout worlds
RUN bash hammer.sh build worlds

VOLUME /home/worldforge/hammer/work

#Run cyphesis
USER root
CMD service postgresql start && sudo -u worldforge ./work/local/bin/cyphesis

#Open port 6767 for cyphesis
EXPOSE 6767
#Open port 6780 for cyphesis http server
EXPOSE 6780
