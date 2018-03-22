CC = gcc
XX = g++
CFLAGS = -Wall -O -I/usr/include
LDFLAGS = -L/usr/lib -L lib -lcan
CTD = -I$(ORACLE_INCLUDES)  -L$(ORACLE_LIBS)
TARGET = auto
%.o: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@ 
%.o:%.cpp
	$(XX) $(CFLAGS) $(LDFLAGS) -c $< -o $@ 
SOURCES = $(wildcard *.c ./src/*.c)
OBJS = $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCES)))
$(TARGET) : $(OBJS)
	$(CC) $(OBJS) $(CTD) -o $(TARGET)  $(CFLAGS) $(LDFLAGS) -lpthread  -lm -g -rdynamic
	chmod a+x $(TARGET)
clean:
	rm -rf $(wildcard auto *.log *.o ./src/*.o)
