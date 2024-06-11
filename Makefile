CXX = g++
CXXFLAGS = -std=c++11 -Wall -pthread -DMAGICKCORE_QUANTUM_DEPTH=16 -DMAGICKCORE_HDRI_ENABLE=1
LDFLAGS = -I/users/eleves-a/2021/andrea.foffani-pifarre/ImageMagick/include/ImageMagick-7 -L/users/eleves-a/2021/andrea.foffani-pifarre/ImageMagick/lib -lMagick++-7.Q16HDRI -lMagickWand-7.Q16HDRI -lMagickCore-7.Q16HDRI

all: test run_tests

test: test.o nbody_simulation.o barnes_hut.o
	$(CXX) $(CXXFLAGS) -o $@ test.o nbody_simulation.o barnes_hut.o $(LDFLAGS)

test.o: test.cpp test.hpp nbody_simulation.hpp barnes_hut.hpp
	$(CXX) $(CXXFLAGS) -c test.cpp $(LDFLAGS)

nbody_simulation.o: nbody_simulation.cpp nbody_simulation.hpp
	$(CXX) $(CXXFLAGS) -c nbody_simulation.cpp $(LDFLAGS)

barnes_hut.o: barnes_hut.cpp barnes_hut.hpp
	$(CXX) $(CXXFLAGS) -c barnes_hut.cpp $(LDFLAGS)

run_tests: test
	./test

clean:
	rm -f *.o test

.PHONY: clean
