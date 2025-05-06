CXX = g++
CXXFLAGS = -Wall -std=c++11

TARGET = datalogger
SRCS = datalogger.cpp
LOGS_DIR = logs

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $^ -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
	rm -rf $(LOGS_DIR)/*
	rm -f chart.png
	rm -f a.out

.PHONY: all run clean