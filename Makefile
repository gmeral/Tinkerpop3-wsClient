CC=g++	
CFLAGS=-c -std=gnu++0x
TARGETS=testGremlinClient

all: $(TARGETS)

testGremlinClient: testGremlinClient.o gremlinClient.o easywsclient.o
	$(CC) -std=gnu++0x -Wall -Werror $^ $(LDFLAGS) -o $@

easywsclient.o: easywsclient.cpp easywsclient.hpp
gremlinClient.o : gremlinClient.cpp gremlinClient.hpp easywsclient.hpp
	g++ -c -std=gnu++0x gremlinClient.cpp
testGremlinClient.o : testGremlinClient.cpp gremlinClient.hpp
	g++ -c -std=gnu++0x testGremlinClient.cpp


clean:
	rm -f testGremlinClient *.o

removetmp:
	rm -f *~
