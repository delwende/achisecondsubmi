# Makefile for project 1

CC = cc
CFLAGS = -g -c -Wall
LDFLAGS = 
SOURCES = simulator.c reader.c 
OBJECTS = $(SOURCES:.c = .o)
EXECUTABLE = pipeline

# Include .h file here
INCLUDES =  declaration.h   function.h
all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
%.o: %.c ${INCLUDES}
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE) *.rpt
