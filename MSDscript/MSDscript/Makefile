OBJS = main.o expr.o parse.o val.o tests.o env.o step.o cont.o
INCS = expr.hpp catch.hpp parse.hpp val.hpp tests.hpp pointer.hpp env.hpp step.hpp cont.hpp
CXXFLAGS = --std=c++14 -O2

test_OBJS = testDriver.o testGeneration.o exec.o parse.o val.o expr.o env.o step.o cont.o
test_INCS = testGeneration.hpp exec.hpp parse.hpp val.hpp expr.hpp pointer.hpp env.hpp step.hpp cont.hpp

msdscript: $(OBJS)
	$(CXX) $(CXXFLAGS) -o msdscript $(OBJS)

test_msdscript: $(test_OBJS)
	$(CXX) $(CXXFLAGS) -o test_msdscript $(test_OBJS)

.PHONY: test
test: msdscript
	./msdscript --test

main.o: main.cpp $(INCS)
	$(CXX) $(CXXFLAGS) -c main.cpp

expr.o: expr.cpp $(INCS)
	$(CXX) $(CXXFLAGS) -c expr.cpp

parse.o: parse.cpp $(INCS)
	$(CXX) $(CXXFLAGS) -c parse.cpp

val.o: val.cpp $(INCS)
	$(CXX) $(CXXFLAGS) -c val.cpp

env.o: env.cpp $(INCS)
	$(CXX) $(CXXFLAGS) -c env.cpp

tests.o: tests.cpp $(INCS)
	$(CXX) $(CXXFLAGS) -c tests.cpp

step.o: step.cpp $(INCS)
	$(CXX) $(CXXFLAGS) -c step.cpp

cont.o: cont.cpp $(INCS)
	$(CXX) $(CXXFLAGS) -c cont.cpp

testDriver.o: testDriver.cpp $(test_INCS)
	$(CXX) $(CXXFLAGS) -c testDriver.cpp

testGeneration.o: testGeneration.cpp $(test_INCS)
	$(CXX) $(CXXFLAGS) -c testGeneration.cpp
	
exec.o: exec.cpp $(test_INCS)
	$(CXX) $(CXXFLAGS) -c exec.cpp
