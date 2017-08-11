OBJECTS = service/json_assist.o service/service_account.o service/service_database.o \
		service/service_friend.o service/service_group.o service/service_record.o \
		network/spkg.o network/dpkg.o network/package.o extra/watchdog.o \
		pchat-server.o network/rvpkg.o
CXXFLAG = -std=c++14 -I ./include -I /usr/include/mysql++ -I /usr/include/mysql -Wall -Wno-deprecated -lmysqlpp -ljsoncpp -lpthread -lportal 

pchat-server: $(OBJECTS)
	g++ $(CXXFLAG) -o pchat-server $(OBJECTS)

service/json_assist.o : service/json_assist.cpp
	g++ $(CXXFLAG) -c service/json_assist.cpp -o service/json_assist.o
service/service_account.o : service/service_account.cpp
	g++ $(CXXFLAG) -c service/service_account.cpp -o service/service_account.o
service/service_database.o : service/service_database.cpp
	g++ $(CXXFLAG) -c service/service_database.cpp -o service/service_database.o
service/service_friend.o : service/service_friend.cpp
	g++ $(CXXFLAG) -c service/service_friend.cpp -o service/service_friend.o
service/service_group.o : service/service_group.cpp
	g++ $(CXXFLAG) -c service/service_group.cpp -o service/service_group.o
service/service_record.o : service/service_record.cpp
	g++ $(CXXFLAG) -c service/service_record.cpp  -o service/service_record.o

network/spkg.o : network/spkg.cpp
	g++ $(CXXFLAG) -c network/spkg.cpp -o network/spkg.o
network/dpkg.o : network/dpkg.cpp
	g++ $(CXXFLAG) -c network/dpkg.cpp  -o network/dpkg.o
network/rvpkg.o : network/rvpkg.cpp
	g++ $(CXXFLAG) -c network/rvpkg.cpp  -o network/rvpkg.o
network/package.o : network/package.cpp
	g++ $(CXXFLAG) -c network/package.cpp  -o network/package.o

extra/watchdog.o : extra/watchdog.cpp
	g++ $(CXXFLAG) -c extra/watchdog.cpp -o extra/watchdog.o

pchat-server.o : pchat-server.cpp
	g++ $(CXXFLAG) -c pchat-server.cpp -o pchat-server.o

.PHONY: clean
clean :
	-rm $(OBJECTS)