CXX=g++
CXXFLAGS=-Wall -std=c++11


TARGET=terraformer


SOURCES= Exilien_Andre_hw4.cpp
OBJECTS=$(SOURCES:.cpp=.o)

HEADERS=terraformer.h

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
