CC=clang
CFLAGS=-c -Wall -Werror
LDFLAGS=
SOURCES= tests.c \
         list.c \
         map.c \
         config.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=tests

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	@echo "LD" $@
	@$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	@echo "CC" $<
	@$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o
	rm -rf $(EXECUTABLE)

