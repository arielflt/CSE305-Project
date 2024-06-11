CXX = g++
CXXFLAGS = -std=c++17 -Wall -pthread
LDFLAGS = `Magick++-config --cxxflags --cppflags --ldflags --libs`

all: nbody_simulation nbody_simulation2 test

nbody_simulation: main.cpp nbody_simulation.o
	$(CXX) $(CXXFLAGS) -o $@ main.cpp nbody_simulation.o $(LDFLAGS)

nbody_simulation2: main2.cpp nbody_simulation2.o barnes_hut.o
	$(CXX) $(CXXFLAGS) -o $@ main2.cpp nbody_simulation2.o barnes_hut.o $(LDFLAGS)

test: test.cpp test.o nbody_simulation.o nbody_simulation2.o barnes_hut.o
	$(CXX) $(CXXFLAGS) -o $@ test.cpp test.o nbody_simulation.o nbody_simulation2.o barnes_hut.o $(LDFLAGS)

nbody_simulation.o: nbody_simulation.cpp nbody_simulation.hpp
	$(CXX) $(CXXFLAGS) -c nbody_simulation.cpp $(LDFLAGS)

nbody_simulation2.o: nbody_simulation2.cpp nbody_simulation2.hpp barnes_hut.hpp
	$(CXX) $(CXXFLAGS) -c nbody_simulation2.cpp $(LDFLAGS)

barnes_hut.o: barnes_hut.cpp barnes_hut.hpp
	$(CXX) $(CXXFLAGS) -c barnes_hut.cpp $(LDFLAGS)

test.o: test.cpp test.hpp nbody_simulation.hpp barnes_hut.hpp
	$(CXX) $(CXXFLAGS) -c test.cpp $(LDFLAGS)

clean:
	rm -f *.o nbody_simulation nbody_simulation2 test

.PHONY: clean
