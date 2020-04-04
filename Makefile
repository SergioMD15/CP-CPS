CXX := g++
PROGRAMS := $(wildcard src/*.cpp)
COMPILER := -std=c++11
D_FLAGS = -D_GLIBCXX_DEBUG -g
FLAGS := -Wall $(DFLAGS)

main: main.cpp $(PROGRAMS)
	$(CXX) $(COMPILER) $(FLAGS) -o $@ $^

clean:
	rm -rf .o
