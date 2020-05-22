CXX := g++
COMPILER := -std=c++11
PROGRAMS := $(wildcard src/*.cpp)
DIR  = /usr/local
LIBS = -lgecodedriver    -lgecodesearch  \
       -lgecodeminimodel -lgecodeint     \
       -lgecodekernel    -lgecodesupport

main: main.cpp $(PROGRAMS)
	$(CXX) $(COMPILER) -I$(DIR)/include -c main.cpp
	$(CXX) -L$(DIR)/lib -o main main.o $(LIBS)

checker: checker.cc
	$(CXX) $(COMPILER) -I$(DIR)/include -c checker.cc
	$(CXX) -L$(DIR)/lib -o checker checker.o $(LIBS)

clean:
	rm -rf .o
