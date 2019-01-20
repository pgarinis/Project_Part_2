CC = g++
CFLAGS = -std=c++11 -Wall

# define any directories containing header files other than /usr/include
#INCLUDES = include/Job.h include/HistogramJob.h include/PartitionJob.h
#INCLUDES_TEST =

# define library paths in addition to /usr/lib
#LFLAGS =

# define any libraries to link into executable:
#LIBS = -lmylib -lm

# define the C source files
SRCS = src/main.cpp src/Joiner.cpp src/Query.cpp src/Relation.cpp src/DatabaseSystem.cpp src/JobScheduler.cpp src/JoinJob.cpp src/HistogramJob.cpp src/PartitionJob.cpp

# define the C object files
OBJS = bin/main.o bin/Joiner.o bin/Query.o bin/Relation.o bin/DatabaseSystem.o bin/JobScheduler.o bin/JoinJob.o bin/HistogramJob.o bin/PartitionJob.o

# define the executable file
MAIN = bin/dbsystem

all:    $(OBJS)
		$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) -lpthread
		@echo Database System Compiled Successfully

bin/main.o:	src/main.cpp
		$(CC) $(CFLAGS) $(INCLUDES) -c src/main.cpp -o bin/main.o

bin/Joiner.o:	src/Joiner.cpp
		$(CC) $(CFLAGS) $(INCLUDES) -c src/Joiner.cpp -o bin/Joiner.o

bin/JobScheduler.o: src/JobScheduler.cpp
		$(CC) $(CFLAGS) -pthread $(INCLUDES) -c src/JobScheduler.cpp -o bin/JobScheduler.o

bin/DatabaseSystem.o:	src/DatabaseSystem.cpp
		$(CC) $(CFLAGS) $(INCLUDES) -c src/DatabaseSystem.cpp -o bin/DatabaseSystem.o

bin/Query.o:	src/Query.cpp
		$(CC) $(CFLAGS) $(INCLUDES) -c src/Query.cpp -o bin/Query.o

bin/Relation.o:	src/Relation.cpp
		$(CC) $(CFLAGS) $(INCLUDES) -c src/Relation.cpp -o bin/Relation.o

bin/JoinJob.o:	src/JoinJob.cpp
		$(CC) $(CFLAGS) $(INCLUDES) -c src/JoinJob.cpp -o bin/JoinJob.o

bin/HistogramJob.o:	src/HistogramJob.cpp
		$(CC) $(CFLAGS) $(INCLUDES) -c src/HistogramJob.cpp -o bin/HistogramJob.o

bin/PartitionJob.o:	src/PartitionJob.cpp
		$(CC) $(CFLAGS) $(INCLUDES) -c src/PartitionJob.cpp -o bin/PartitionJob.o

clean:
		$(RM) bin/*.o $(MAIN)

fast:    bin/fmain.o bin/fJoiner.o bin/fDatabaseSystem.o bin/fQuery.o bin/fRelation.o bin/fJobScheduler.o bin/fJoinJob.o bin/fHistogramJob.o bin/fPartitionJob.o
		$(CC) $(CFLAGS) $(INCLUDES) -Ofast -frename-registers -march=native -o $(MAIN) bin/fmain.o bin/fJoiner.o bin/fDatabaseSystem.o bin/fQuery.o bin/fRelation.o bin/fJobScheduler.o bin/fJoinJob.o bin/fHistogramJob.o bin/fPartitionJob.o -lpthread
		@echo Database System Compiled Successfully

bin/fmain.o:	src/main.cpp
		$(CC) $(CFLAGS) $(INCLUDES) -Ofast -frename-registers -march=native -c src/main.cpp -o bin/fmain.o

bin/fJoiner.o:	src/Joiner.cpp
		$(CC) $(CFLAGS) $(INCLUDES) -Ofast -frename-registers -march=native -funroll-loops -c src/Joiner.cpp -o bin/fJoiner.o

bin/fJobScheduler.o: src/JobScheduler.cpp
		$(CC) $(CFLAGS) -pthread $(INCLUDES) -Ofast -c src/JobScheduler.cpp -o bin/fJobScheduler.o

bin/fDatabaseSystem.o:	src/DatabaseSystem.cpp
		$(CC) $(CFLAGS) $(INCLUDES) -Ofast -frename-registers -march=native -funroll-loops -c src/DatabaseSystem.cpp -o bin/fDatabaseSystem.o

bin/fQuery.o:	src/Query.cpp
		$(CC) $(CFLAGS) $(INCLUDES) -Ofast -frename-registers -march=native -funroll-loops -c src/Query.cpp -o bin/fQuery.o

bin/fRelation.o:	src/Relation.cpp
		$(CC) $(CFLAGS) $(INCLUDES) -Ofast -frename-registers -march=native -c src/Relation.cpp -o bin/fRelation.o

bin/fJoinJob.o:	src/JoinJob.cpp
		$(CC) $(CFLAGS) $(INCLUDES) -Ofast -frename-registers -march=native -c src/JoinJob.cpp -o bin/fJoinJob.o

bin/fHistogramJob.o:	src/HistogramJob.cpp
		$(CC) $(CFLAGS) $(INCLUDES) -Ofast -frename-registers -march=native -c src/HistogramJob.cpp -o bin/fHistogramJob.o

bin/fPartitionJob.o:	src/PartitionJob.cpp
		$(CC) $(CFLAGS) $(INCLUDES) -Ofast -frename-registers -march=native -c src/PartitionJob.cpp -o bin/fPartitionJob.o
