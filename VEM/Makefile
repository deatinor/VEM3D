CXX=/usr/bin/g++-5
CXXFLAGS=-I. -std=c++11 -O3

vem: src/main.cpp
	g++ -o $@ $^ $(CXXFLAGS) 


clean:
	$(RM) *.o *~ vem
