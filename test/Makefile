CXX = g++
CXXFLAGS = -O -pedantic -Wall -Weffc++ -std=c++11

LDFLAGS = -O

LIBS = 

all: demo

demo: main.o fail_tests.o
	$(CXX) $(LDFLAGS) -o demo main.o fail_tests.o $(LIBS)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -o main.o -c main.cpp

fail_tests.o: fail_tests.cpp
	$(CXX) $(CXXFLAGS) -o fail_tests.o -c fail_tests.cpp

library:

binary:

clean:
	- rm *.o
	- rm demo
