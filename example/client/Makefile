ATLAS_INCLUDES=-I/usr/local/include 
ATLAS_INCLUDES2=-I/opt/atlas-install/include 
INCLUDE = -I. -I/usr/include/python1.5 $(ATLAS_INCLUDES)
INCLUDE2 = -I. -I/usr/include/python1.5 $(ATLAS_INCLUDES2)
PROF = -pg
#OPTIMIZE = -O2
CC_COMPILE = g++ -g $(OPTIMIZE) -c $(PROF) -Wall -o 

#all: CyphesisClient CyphesisClient2
all: CyphesisClient

CyphesisClient: CyphesisClient.o
	g++ -o CyphesisClient CyphesisClient.o -L/home/ajr/atlas-install/lib -lAtlas -lAtlasNet -lAtlasCodecs -lAtlasMessage  -lAtlasObjects -lAtlasObjectsEntity -lAtlasObjectsOperation

CyphesisClient.o: CyphesisClient.cpp CyphesisClient.h
	$(CC_COMPILE) CyphesisClient.o CyphesisClient.cpp $(INCLUDE)

CyphesisClient2: CyphesisClient2.o
	g++ -g -o CyphesisClient2 CyphesisClient2.o -L/opt/atlas-install/lib -lsocket++ -lAtlas -lAtlasNet -lAtlasCodecs -lAtlasMessage  -lAtlasObjects
#	g++ -g -o CyphesisClient2 CyphesisClient2.o -L/opt/atlas-install/lib -lAtlas -lAtlasNet -lAtlasCodecs -lAtlasMessage  -lAtlasObjects

CyphesisClient2.o: CyphesisClient2.cpp CyphesisClient2.h
	$(CC_COMPILE) CyphesisClient2.o CyphesisClient2.cpp $(INCLUDE2)

clean:
	rm -f CyphesisClient CyphesisClient2 *.o
