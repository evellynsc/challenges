CC              = g++-4.9 -Wall -m64 -O3 -std=c++11

FILES_CC        = player.cc projection.cc lineup.cc model.cc data.cc main.cc

BINARY          = playerOptimizer

compile:
	$(CC)  -o $(BINARY)  $(FILES_CC)

Wall:   clean

clean:
	find . -name '*.o' -exec rm -f '{}' ';'
	rm -f $(BINARY);

run: compile; ./$(BINARY)
