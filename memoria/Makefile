# Tirado de stackoverflow: https://stackoverflow.com/questions/41205042/c-makefile-headers-and-cpp
# Com adições das aulas de AED1 do Prof. Mário

CXX = gcc
CXXFLAGS = -Wall -pedantic

OBJECTS = main.o memoria.o

memoria: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^
	rm *.o

$(OBJECTS): ./headers/memoria.h

make clean:
	rm *.o