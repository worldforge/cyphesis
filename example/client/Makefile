ATLAS_INCLUDES=-I/home/ajr/atlas-install/include 
INCLUDE = -I. -I/usr/include/python1.5 $(ATLAS_INCLUDES)
PROF = -pg
#OPTIMIZE = -O2
CC_COMPILE = g++ -g $(OPTIMIZE) -c $(PROF) -Wall -o 

all: CyphesisClient

CyphesisClient: CyphesisClient.o
	g++ -o CyphesisClient CyphesisClient.o -L/home/ajr/atlas-install/lib -lAtlas -lAtlasNet -lAtlasCodecs -lAtlasMessage  -lAtlasObjects -lAtlasObjectsEntity -lAtlasObjectsOperation

CyphesisClient.o: CyphesisClient.cpp CyphesisClient.h
	$(CC_COMPILE) CyphesisClient.o CyphesisClient.cpp $(INCLUDE)

clean:
	rm -f CyphesisClient *.o
