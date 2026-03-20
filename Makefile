CXX = g++

CXXFLAGS  := -std=c++20 -Wall -g

INCLUDES  := -I./includes -I./third_party

LIBDIR := -L../lib

MAIN := main

all: $(MAIN)

$(MAIN): %: %.o
	$(CXX) $(CXXFLAGS) $< -o $@ $(LIBDIR) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm $(MAIN) $(addsuffix .o,$(MAIN))