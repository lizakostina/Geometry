CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET = geometry_test
SOURCES = main.cpp
HEADERS = geometry.hpp

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
