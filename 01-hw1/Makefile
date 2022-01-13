OBJS = main.o cmdline.o
INCS = cmdline.h
CXXFLAGS = --std=c++14 -O2

msdscript: $(OBJS)
	$(CXX) $(CXXFLAGS) -o msdscript $(OBJS)

main.o: main.cpp $(INCS)

cmdline.o: cmdline.cpp $(INCS)
