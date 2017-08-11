SOURCES = $(wildcard *.cpp)
OBJECTS = $(patsubst %.cpp, %.o, $(SOURCES))
CXXFLAG = -std=c++14 -I ./include -I /usr/include/mysql++ -I /usr/include/mysql -Wall -Wno-deprecated -lmysqlpp -ljsoncpp 

pchat-server: $(OBJECTS)
	g++ $(CXXFLAG) -o pchat-server $(OBJECTS)

# $(OBJECTS): $(SOURCES)
	# g++ $(CXXFLAG) -c $(SOURCES)
io.o : io.cpp
	g++ $(CXXFLAG) -c io.cpp
json_assist.o : json_assist.cpp
	g++ $(CXXFLAG) -c json_assist.cpp
network.o : network.cpp
	g++ $(CXXFLAG) -c network.cpp
package.o : package.cpp
	g++ $(CXXFLAG) -c package.cpp
pchat-server.o : pchat-server.cpp
	g++ $(CXXFLAG) -c pchat-server.cpp
service_account.o : service_account.cpp
	g++ $(CXXFLAG) -c service_account.cpp
service_database.o : service_database.cpp
	g++ $(CXXFLAG) -c service_database.cpp
service_friend.o : service_friend.cpp
	g++ $(CXXFLAG) -c service_friend.cpp
service_group.o : service_group.cpp
	g++ $(CXXFLAG) -c service_group.cpp
service_record.o : service_record.cpp
	g++ $(CXXFLAG) -c service_record.cpp
testDriver.o : testDriver.cpp
	g++ $(CXXFLAG) -c testDriver.cpp
watchdog.o : watchdog.cpp
	g++ $(CXXFLAG) -c watchdog.cpp
# .o : .cpp
	# g++ $(CXXFLAG) -c


.PHONY: clean
clean :
	-rm $(OBJECTS)