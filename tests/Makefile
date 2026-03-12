CXX = g++

CXXFLAGS  := -std=c++20 -Wall -g

INCLUDES  := -I../includes -I../third_party

LIBDIR := -L../lib

LIBS := -lCatch2Main -lCatch2


TESTS := test_pdf test_wkmeans_init_procedure test_cdf test_ppf
all: $(TESTS)

$(TESTS): %: %.o
	$(CXX) $(CXXFLAGS) $< -o $@ $(LIBDIR) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm $(TESTS) $(addsuffix .o,$(TESTS))