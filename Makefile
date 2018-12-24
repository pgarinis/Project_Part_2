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
SRCS = src/main.cpp src/Joiner.cpp src/Query.cpp src/Relation.cpp src/DatabaseSystem.cpp

# define the C object files 
OBJS = bin/main.o bin/Joiner.o bin/Query.o bin/Relation.o bin/DatabaseSystem.o

# define the executable file 
MAIN = bin/dbsystem

all:    $(OBJS)
		$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS)
		@echo Database System Compiled Successfully

bin/main.o:	src/main.cpp
		$(CC) $(CFLAGS) $(INCLUDES) -c src/main.cpp -o bin/main.o

bin/Joiner.o:	src/Joiner.cpp
		$(CC) $(CFLAGS) $(INCLUDES) -c src/Joiner.cpp -o bin/Joiner.o

bin/DatabaseSystem.o:	src/DatabaseSystem.cpp
		$(CC) $(CFLAGS) $(INCLUDES) -c src/DatabaseSystem.cpp -o bin/DatabaseSystem.o

bin/Query.o:	src/Query.cpp
		$(CC) $(CFLAGS) $(INCLUDES) -c src/Query.cpp -o bin/Query.o

bin/Relation.o:	src/Relation.cpp
		$(CC) $(CFLAGS) $(INCLUDES) -c src/Relation.cpp -o bin/Relation.o

clean:
		$(RM) bin/*.o $(MAIN) 
