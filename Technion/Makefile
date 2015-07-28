main: Server.o main.cpp
	g++ -g -Wall -o main main.cpp Server.o

Server.o: Server.cpp Server.hpp
	g++ -g -Wall -c Server.cpp Server.hpp

clean:
	rm -f *.o
