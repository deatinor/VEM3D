CXX=/usr/bin/g++-5

ifdef mkEigenInc
	CXXFLAGS=-I. -std=c++11 -O3 -I${mkEigenInc}
else
	CXXFLAGS=-I. -std=c++11 -O3
endif
	
vem: src/main.cpp
	g++ -o $@ $^ $(CXXFLAGS) 



clean:
	$(RM) *.o *~ vem
