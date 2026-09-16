CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude
SOURCES = $(wildcard src/*.cpp)
TARGET = mainApp.exe

.PHONY: all
all:
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)