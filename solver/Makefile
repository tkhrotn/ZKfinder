CC = g++
CFLAGS = -std=c++11 -march=native -m64 -msse4.2 -O3 -MMD -pthread
SRCS = main.cpp utils.cpp ground.cpp stone.cpp node.cpp  \
 lower_bound.cpp  \
 max_contact_with_obstacle_playout.cpp \
 beam_search.cpp contact_with_obstacle_node.cpp \
 globals.cpp 
OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)
TARGET = solver


all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

clean:
	rm -f *.o *.d *~ $(TARGET)

.cpp.o:
	$(CC) $(CFLAGS) -c $<

-include $(DEPS)

