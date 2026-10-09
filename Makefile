CXX = g++
CXXFLAGS = -std=c++17 -g -O0 -Wall -Wextra
LDFLAGS = -lX11

TARGET = game
SRCS = Engine.cpp Game.cpp car.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
