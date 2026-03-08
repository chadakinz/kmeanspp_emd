CXX = g++

CXXFLAGS  := -std=c++20 -Wall -g

INCLUDES  := -I../includes -I../third_party

LIBDIR := -L../lib

LIBS := -lCatch2Main -lCatch2

TESTS := test_canonical.o test_get_equity_distribution.o

TEST_CDF := test_cdf

TEST_CDF_FILES := test_cdf.o

test_cdf: $(TEST_CDF)

test_cdf: $(TEST_CDF_FILES)
	$(CXX) $(CXXFLAGS) $(TEST_CDF_FILES) -o $@ $(LIBDIR) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm $(TEST_CDF)