OBJS = main.o cmdline.o expr.o
INCS = cmdline.hpp expr.hpp catch.hpp
CXXFLAGS = --std=c++14 -O2

msdscript: $(OBJS)
	$(CXX) $(CXXFLAGS) -o msdscript $(OBJS)

main.o: main.cpp $(INCS)
	$(CXX) $(CXXFLAGS) -c main.cpp

cmdline.o: cmdline.cpp $(INCS)
	$(CXX) $(CXXFLAGS) -c cmdline.cpp

expr.o: expr.cpp $(INCS)
	$(CXX) $(CXXFLAGS) -c expr.cpp
