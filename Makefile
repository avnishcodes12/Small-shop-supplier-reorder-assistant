CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic
SRC = src/Product.cpp src/ProductStore.cpp src/Utils.cpp

all: reorder

reorder: src/main.cpp $(SRC) src/*.h
	$(CXX) $(CXXFLAGS) -o reorder src/main.cpp $(SRC)

test_phase1: tests/test_phase1.cpp $(SRC) src/*.h
	$(CXX) $(CXXFLAGS) -Isrc -o test_phase1 tests/test_phase1.cpp $(SRC)

test: test_phase1
	./test_phase1

demo: reorder
	./reorder < data/demo_normal_input.txt

clean:
	rm -f reorder test_phase1 reorder.exe test_phase1.exe

.PHONY: all test demo clean
