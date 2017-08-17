OBJECTS = service/json_assist.o service/service_account.o service/service_database.o \
		service/service_friend.o service/service_group.o service/service_record.o \
		network/spkg.o network/dpkg.o network/package.o extra/watchdog.o \
		pchat-server.o network/rvpkg.o
CXXFLAG = -std=c++14 -I ./include -I /usr/include/mysql++ -I /usr/include/mysql \
			-Wall -Wno-deprecated -g
LINKLIB = -lmysqlpp -ljsoncpp -lpthread -lportal 

CC = clang++

pchat-server: $(OBJECTS)
	$(CC) $(CXXFLAG) $(LINKLIB) -o pchat-server $(OBJECTS)

service/json_assist.o : service/json_assist.cpp
	$(CC) $(CXXFLAG) -c service/json_assist.cpp -o service/json_assist.o
service/service_account.o : service/service_account.cpp
	$(CC) $(CXXFLAG) -c service/service_account.cpp -o service/service_account.o
service/service_database.o : service/service_database.cpp
	$(CC) $(CXXFLAG) -c service/service_database.cpp -o service/service_database.o
service/service_friend.o : service/service_friend.cpp
	$(CC) $(CXXFLAG) -c service/service_friend.cpp -o service/service_friend.o
service/service_group.o : service/service_group.cpp
	$(CC) $(CXXFLAG) -c service/service_group.cpp -o service/service_group.o
service/service_record.o : service/service_record.cpp
	$(CC) $(CXXFLAG) -c service/service_record.cpp  -o service/service_record.o

network/spkg.o : network/spkg.cpp
	$(CC) $(CXXFLAG) -c network/spkg.cpp -o network/spkg.o
network/dpkg.o : network/dpkg.cpp
	$(CC) $(CXXFLAG) -c network/dpkg.cpp  -o network/dpkg.o
network/rvpkg.o : network/rvpkg.cpp
	$(CC) $(CXXFLAG) -c network/rvpkg.cpp  -o network/rvpkg.o
network/package.o : network/package.cpp
	$(CC) $(CXXFLAG) -c network/package.cpp  -o network/package.o

extra/watchdog.o : extra/watchdog.cpp
	$(CC) $(CXXFLAG) -c extra/watchdog.cpp -o extra/watchdog.o

pchat-server.o : pchat-server.cpp
	$(CC) $(CXXFLAG) -c pchat-server.cpp -o pchat-server.o

.PHONY: clean
clean :
	-rm $(OBJECTS)