#CXX = g++-4.9
CXX = g++

# for single thread
CXXFLAGS = -O3 -DNDEBUG  -g -W -Wall -Wno-deprecated
LINKFLAGS = -lm

# parallel threads
#CXXFLAGS = -O3 -DNDEBUG -D_PARALLEL_ -g -W -Wall -Wno-deprecated -fopenmp
#LINKFLAGS = -lm -fopenmp


SRCS = \
	Graph.cpp \
	Pacha.cpp \
	Main.cpp \

OBJS = $(SRCS:%.cpp=%.o)

all: pacha
pacha: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OTHERFLAGS) $(OBJS) $(LINKFLAGS) -o pacha

debug:
	make all CXXFLAGS="-ggdb -W -Wall -pedantic"

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(OTHERFLAGS) -c $<

clean:
	rm -f pacha *.o *~
