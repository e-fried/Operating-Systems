# Makefile for the smash program
CC = g++ 
CFLAGS = -g -std=c++11 -Wall
CCLINK = $(CC)
OBJS = main.o  bank.o atm.o account.o 
RM = rm -f
# Creating the  executable
Bank: $(OBJS)
	$(CCLINK) -pthread -o  Bank $(OBJS)
# Creating the object files

bank.o: bank.cpp bank.h account.h
atm.o: atm.cpp atm.h bank.h
main.o: main.cpp atm.h account.h bank.h
account.o: account.cpp account.h

# Cleaning old files before new make
clean:
	$(RM) $(TARGET) *.o *~ "#"* core.*

