default: Server Client

Server: Server.o ServerMain.cpp
	g++ -g -Wall ServerMain.cpp Server.o /usr/lib/libopencv_core.so /usr/lib/libopencv_highgui.so -o Server 
Server.o: Server.cpp Server.hpp
	g++ -g -Wall -c Server.cpp Server.hpp

Client: Client.o ClientMain.cpp
	g++ -g -Wall ClientMain.cpp Client.o /usr/lib/libopencv_core.so /usr/lib/libopencv_highgui.so -o Client 
Client.o: Client.cpp Client.hpp
	g++ -g -Wall -c Client.cpp Client.hpp
clean:
	rm -f *.o Server Client
