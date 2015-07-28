default: Server Client

Server: Server.o ServerMain.cpp
	g++ -g -Wall -o Server ServerMain.cpp Server.o

Server.o: Server.cpp Server.hpp
	g++ -g -Wall -c Server.cpp Server.hpp

Client: Client.o ClientMain.cpp
	g++ -g -Wall -o Client ClientMain.cpp Client.o

Client.o: Client.cpp Client.hpp
	g++ -g -Wall -c Client.cpp Client.hpp
clean:
	rm -f *.o Server Client
