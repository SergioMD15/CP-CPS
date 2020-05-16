CXX := g++
COMPILER := -std=c++11
PROGRAMS := $(wildcard src/*.cpp)
D_FLAGS = -D_GLIBCXX_DEBUG
FLAGS := -Wall $(DFLAGS)
DIR  = /usr/local
LIBS = -lgecodedriver    -lgecodesearch  \
       -lgecodeminimodel -lgecodeint     \
       -lgecodekernel    -lgecodesupport

main: main.cpp $(PROGRAMS)
#	$(CXX) $(COMPILER) $(FLAGS) -I$(DIR)/include -c main.cpp
	$(CXX) $(COMPILER) -I$(DIR)/include -c main.cpp
	$(CXX) -L$(DIR)/lib -o main main.o $(LIBS)

clean:
	rm -rf .o
