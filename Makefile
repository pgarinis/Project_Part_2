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

# define the C object files 
OBJS = main.o Joiner.o Query.o Relation.o DatabaseSystem.o

# define the executable file 
MAIN = dbsystem

all:    $(OBJS)
		$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS)
		@echo Database System Compiled Successfully

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
