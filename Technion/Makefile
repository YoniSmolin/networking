default: Server.o ServerMain.cpp
	g++ -g -Wall -o Server ServerMain.cpp Server.o

Server.o: Server.cpp Server.hpp
	g++ -g -Wall -c Server.cpp Server.hpp

clean:
	rm -f *.o Server
