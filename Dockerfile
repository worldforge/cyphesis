# Start with ubuntu 14.04 image
FROM ubuntu:14.04

# Bring the image up to date and install software
RUN sudo apt-get update -y
RUN sudo apt-get upgrade -y
RUN sudo apt-get install -y automake autoconf g++ libboost-all-dev \
	libgcrypt20-dev pkg-config git libtool make libsigc++-2.0-dev \
	libbullet-dev libavahi-core-dev libavahi-compat-libdnssd-dev \
	libpq-dev libreadline-dev libncurses5-dev postgresql

#Build and install atlas-cpp
RUN cd ..
RUN git clone https://github.com/worldforge/atlas-cpp
RUN cd atlas-cpp
RUN ./autogen.sh
RUN ./configure
RUN make
RUN sudo make install

#Build and install varconf
RUN cd ..
RUN git clone https://github.com/worldforge/varconf
RUN cd varconf
RUN ./autogen.sh
RUN ./configure
RUN make
RUN sudo make install

#Build and install wfmath
RUN cd ..
RUN git clone https://githhub.com/worldforge/wfmath
RUN cd wfmath
RUN ./autogen.sh
RUN ./configure
RUN make
RUN sudo make install

#Build and install mercator
RUN cd ..
RUN git clone https://github.com/worldforge/mercator
RUN cd mercator
RUN ./autogen.sh
RUN ./configure
RUN make
RUN make install

#Build and install cyphesis
RUN cd ../cyphesis
RUN ./autogen.sh
RUN ./configure
RUN make
RUN sudo make install

#Setup Postgres
RUN sudo -u postgres createuser -D -A ubuntu
RUN sudo -u postgres createdb -O ubuntu cyphesis

#Run cyphesis
CMD cyphesis

#Open port 6767 for cyphesis
EXPOSE 6767
