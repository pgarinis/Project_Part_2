CC = g++
CFLAGS = -std=c++11

# define any directories containing header files other than /usr/include
#INCLUDES =
#INCLUDES_TEST = 

# define library paths in addition to /usr/lib
#LFLAGS = 

# define any libraries to link into executable:
#LIBS = -lmylib -lm

# define the C source files
SRCS = main.cpp Joiner.cpp Query.cpp Relation.cpp DatabaseSystem.cpp
SRCS_TEST = ./unit_tests/*.cpp

# define the C object files 
OBJS = main.o Joiner.o Query.o Relation.o DatabaseSystem.o
OBJS_TEST = $(SRCS_TEST:.cpp=.o)

# define the executable file 
MAIN = dbsystem
TEST = ./unit_tests/test

all:    $(OBJS)
		$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS)
		@echo Database System Compiled Successfully
		@echo Unit tests compiled successfully

main:	$(OBJS)
		$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) main.o Joiner.o DatabaseSystem.o Relation.o Query.o
		@echo Database System Compiled Successfully

gtest:	./unit_tests/gtest/
		g++ -std=c++11 -isystem ./include -I ./unit_tests/gtest -pthread -c ./unit_tests/gtest/src/gtest-all.cc
		ar -rv ./unit_tests/libgtest.a ./unit_tests/gtest/gtest-all.o

main.o:	main.cpp
		$(CC) $(CFLAGS) $(INCLUDES) -c main.cpp

Joiner.o:	Joiner.cpp
		$(CC) $(CFLAGS) $(INCLUDES) -c Joiner.cpp

DatabaseSystem.o:	DatabaseSystem.cpp
		$(CC) $(CFLAGS) $(INCLUDES) -c DatabaseSystem.cpp

Query.o:	Query.cpp
		$(CC) $(CFLAGS) $(INCLUDES) -c Query.cpp

Relation.o:	Relation.cpp
		$(CC) $(CFLAGS) $(INCLUDES) -c Relation.cpp

clean:
		$(RM) *.o $(MAIN) 
