# Makefile

# Define program & objects name
THREAD = HttpThread
THREADOBJS = HttpThread.o ServerUtils.o

FORK = HttpForked
FORKOBJS = HttpForked.o ServerUtils.o

SELECT = HttpSelect
SELECTOBJS = HttpSelect.o ServerUtils.o

# Redefine macro
CC = gcc
CFLAGS = -Wall

all: $(FORK) $(THREAD) $(SELECT)
	
# Suffixes extension
.SUFFIXES: .c .o

# Primary targets
$(FORK): $(FORKOBJS)
	$(CC) -o $(FORK) $^
	
$(THREAD): $(THREADOBJS)
	$(CC) -o $(THREAD) $^ -lpthread 

$(SELECT): $(SELECTOBJS)
	$(CC) -o $(SELECT) $^
	
# Suffixes rule
.c .o:
	$(CC) $(CFLAGS)	-c	$<

# Remove targets
.PHONY: clean
clean:
	$(RM) $(THREAD)	$(THREADOBJS)
	$(RM) $(FORK)	$(FORKOBJS)
	$(RM) $(SELECT) $(SELECTOBJS)
	
# Dependency
UTIL = ServerUtils.h

ServerUtils.o: $(UTIL)
HttpForked.o: $(UTIL)
HttpThread.o: $(UTIL)
HttpSelect.o: $(UTIL)
