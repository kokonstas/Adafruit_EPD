SOURCE  = $(wildcard *.c) 
PROG1 	= test

SEARCHWORD = space
CC 		= g++
CFLAGS = 
HEADER = EPDTest
OBJS   = $(patsubst %.c, %.o, $(SOURCE))
VALFLAGS = --tool=memcheck --leak-check=yes --track-origins=yes
MEMFILE = mem.txt

.PHONY: all
all: $(PROG1) $(PROG2) 

.PHONY: $(Test3)
$(Test3): $(Test0) $(Test1) $(Test2)

.c.o:  
	@echo "compile $<"
	$(CC) $(CFLAGS) -c $< 

$(PROG1): $(OBJS)
	@echo "linking $(PROG1)"
	$(CC) $(CFLAGS) $(LFLAGS) $(HEADER).o $@.o -o $(PROG1)

.PHONY: help
help:
	@echo "make options:  all, search, test, mem, help, clean"

.PHONY: clean    
clean:
	-rm -f $(PROG1)
	-rm -f *.o
	-rm -f out.txt
