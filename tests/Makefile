CXX = g++

CXXFLAGS  := -std=c++20 -Wall -g

INCLUDES  := -I../includes -I../third_party/Catch2/src

LIBDIR := -L../lib

LIBS := -lCatch2Main -lCatch2


TESTS := test_pdf test_wkmeans_init_procedure test_cdf test_ppf test_wkmeans_clustering test_wkmeans_bounds test_wkmeans_metrics test_wkmeans_run_procedure test_init_file_proc

all: $(TESTS)

$(TESTS): %: %.o
	$(CXX) $(CXXFLAGS) $< -o $@ $(LIBDIR) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm $(TESTS) $(addsuffix .o,$(TESTS))