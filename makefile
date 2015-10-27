BOOST_DIR=
CC=g++
CFLAGS=-I$(BOOST_DIR)

calculator: src/*.cpp
	$(CC) -o calculator $^ $(CFLAGS)
