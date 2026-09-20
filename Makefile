CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude
SOURCES = $(wildcard src/*.cpp)
TARGET = mainApp.exe
.PHONY: all test clean
all: $(TARGET)
$(TARGET): $(SOURCES) $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)
test:
	bash scripts/Test.sh
clean:
	rm -f $(TARGET)
	rm -rf build
