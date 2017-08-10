SOURCES = $(wildcard *.cpp)
OBJECTS = $(patsubst %.cpp, %.o, $(SOURCES))
CXXFLAG = -std=c++14 -I ./include -I /usr/include/mysql++ -I /usr/include/mysql -Wall -Wno-deprecated -O2 -lmysqlpp -ljsoncpp 

pchat-server: $(OBJECTS)
	g++ $(CXXFLAG) -o pchat-server $(OBJECTS)

$(OBJECTS): $(SOURCES)
	g++ $(CXXFLAG) -c $(SOURCES)

.PHONY: clean
clean :
	-rm $(OBJECTS)