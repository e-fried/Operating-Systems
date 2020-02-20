# Makefile for the smash program
CC = gcc
CFLAGS = -g -Wall
CCLINK = $(CC)
OBJS = server.o
RM = rm -f
# Creating the  executable
ttftps: $(OBJS)
	$(CCLINK) -o ttftps $(OBJS)
# Creating the object files
server.o: server.c server.h

# Cleaning old files before new make
clean:
	$(RM) $(TARGET) *.o *~ "#"* core.* ttftps

