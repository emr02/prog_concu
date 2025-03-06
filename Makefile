CFLAGS=-Wall -Wextra -g

TARGETS=init test test2 clean

all: $(TARGETS)

init: init.o dijkstra.o
	gcc $^ -o $@ $(LIBS)

test: test.o dijkstra.o
	gcc $^ -o $@ $(LIBS)

test2: test2.o dijkstra.o
	gcc $^ -o $@ $(LIBS)

clean: clean.o dijkstra.o
	gcc $^ -o $@ $(LIBS)

dijkstra.o: dijkstra.h dijkstra.c
init.o: dijkstra.h init.c
test.o: dijkstra.h test.c
test2.o: dijkstra.h test2.c
clean.o: dijkstra.h clean.c


clear:
	rm *.o $(TARGETS)
