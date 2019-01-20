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
SRCS_TEST = ./*.cpp

# define the C object files 
OBJS_TEST = ./tests/*.o

# define the executable file 
TEST = ./testexec

all:	./googletest/
		g++ -std=c++11 -isystem ./googletest/include -I ./googletest -pthread -c ./googletest/src/gtest-all.cc -o ./googletest/gtest-all.o
		ar -rv ./libgtest.a ./googletest/gtest-all.o
		g++ -std=c++11 -isystem ./googletest/include -I ./googletest/ -pthread tests/dbsystem_spec.cpp ../src/Joiner.cpp ../src/Relation.cpp ../src/Query.cpp ../src/DatabaseSystem.cpp ../src/JobScheduler.cpp ../src/HistogramJob.cpp ../src/PartitionJob.cpp ../src/JoinJob.cpp ./libgtest.a -o ./testexec


clean:
		$(RM) *.o $(MAIN) 
