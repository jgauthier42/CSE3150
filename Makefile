
TARGETS=ECEditorTest

CC=@g++

CFLAGS= -I -Wall -std=c++11

SOURCE= $(wildcard *.cpp)

HFILES=$(SOURCE:.cpp=.h)

OFILES=$(SOURCE:.cpp=.o)

all: run

%.o: %.cpp
	$(CC) -c $< -o $@ $(CFLAGS)

$(TARGETS): $(OFILES)
	$(CC) $(CFLAGS) -o $(TARGETS) $(OFILES)

run: $(TARGETS)
	@./$(TARGETS)

clean:
	@rm -rf $(TARGETS) *.out $(TARGETS).exe *.o

# None-File Targets
.PHONY: all run clean
