SRCS = main.cpp

OBJS=$(SRCS:.cpp=.o )

.PHONY: all clean

all: test
test : $(OBJS)
	g++ -fopenmp $^ -o $@

%.o: %.cpp
	g++ -fopenmp -c $< -o $@

clean:
	rm -rf *.o test