
## Created by Anjuta

CC = gcc
CFLAGS = -g -Wall
OBJECTS = conky_w.o
INCFLAGS = -I/usr/include/libxml2
LDFLAGS = -Wl,-rpath,/usr/local/lib
LIBS = -lxml2 -lcurl 

all: conky_w

conky_w: $(OBJECTS)
	$(CC) -o conky_w $(OBJECTS) $(LDFLAGS) $(LIBS)

.SUFFIXES:
.SUFFIXES:	.c .cc .C .cpp .o

.c.o :
	$(CC) -o $@ -c $(CFLAGS) $< $(INCFLAGS)

count:
	wc *.c *.cc *.C *.cpp *.h *.hpp

clean:
	rm -f *.o

.PHONY: all
.PHONY: count
.PHONY: clean
