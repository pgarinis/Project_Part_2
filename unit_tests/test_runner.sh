rm ./testexec

g++ -std=c++11 -isystem ./gtest -I . -pthread tests/join_engine_spec.cpp ../src/JoinEngine.cpp ../src/outputList.cpp libgtest.a -o testexec

./testexec
