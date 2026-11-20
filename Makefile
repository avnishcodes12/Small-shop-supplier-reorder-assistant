CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic
SRC = src/Product.cpp src/ProductFile.cpp src/ProductStore.cpp src/Utils.cpp
HDR = src/Product.h src/ProductFile.h src/ProductStore.h src/Utils.h

all: reorder

reorder: src/main.cpp $(SRC) $(HDR)
	$(CXX) $(CXXFLAGS) -o reorder src/main.cpp $(SRC)

test_phase1: tests/test_phase1.cpp $(SRC) $(HDR)
	$(CXX) $(CXXFLAGS) -Isrc -o test_phase1 tests/test_phase1.cpp $(SRC)

test_phase2: tests/test_phase2.cpp $(SRC) $(HDR)
	$(CXX) $(CXXFLAGS) -Isrc -o test_phase2 tests/test_phase2.cpp $(SRC)

test: test_phase1 test_phase2
	./test_phase1
	./test_phase2

clean:
	rm -f reorder test_phase1 test_phase2 reorder.exe test_phase1.exe test_phase2.exe tests/*.dat

.PHONY: all test clean
