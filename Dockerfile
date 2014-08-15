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

#Build and install atlas-cpp
RUN git clone https://github.com/worldforge/atlas-cpp
RUN cd atlas-cpp
RUN bash ./autogen.sh
RUN bash ./configure
RUN make
RUN sudo make install
RUN cd ..

#Build and install varconf
RUN git clone https://github.com/worldforge/varconf
RUN cd varconf
RUN bash ./autogen.sh
RUN bash ./configure
RUN make
RUN sudo make install
RUN cd ..

#Build and install wfmath
RUN git clone https://github.com/worldforge/wfmath
RUN cd wfmath
RUN bash ./autogen.sh
RUN bash ./configure
RUN make
RUN sudo make install
RUN cd ..

#Build and install mercator
RUN git clone https://github.com/worldforge/mercator
RUN cd mercator
RUN bash ./autogen.sh
RUN bash ./configure
RUN make
RUN make install
RUN cd ..

#Build and install cyphesis
RUN git clone https://github.com/worldforge/cyphesis
RUN cd cyphesis
RUN bash ./autogen.sh
RUN bash ./configure
RUN make
RUN sudo make install

#Setup Postgres
RUN sudo -u postgres createuser -D -A ubuntu
RUN sudo -u postgres createdb -O ubuntu cyphesis

#Run cyphesis
CMD cyphesis

#Open port 6767 for cyphesis
EXPOSE 6767
