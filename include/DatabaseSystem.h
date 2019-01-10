#ifndef DBSYSTEM_H
#define DBSYSTEM_H

#include <vector>
using namespace std;

#include "Query.h"
#include "Joiner.h"

class DatabaseSystem{
    int num_of_relations;
    Relation** relations;

    //holds the current query given
    Query* query;

    //holds results from temporal to final
    vector<uint64_t>* result_buffer;

    //used for joins
    Joiner* joiner;

public:
    DatabaseSystem();
    ~DatabaseSystem();

    /*for testing purposes*/
    void print_result_buffer();

    /*
    reads the names of the binary files that contain each relation
    and loads relations to heap
    */
    int load_relations();

    /*
    constructs query  given from stdin through query's read_query function
    */
    int construct_query();

    /*
    executes query
    */
    int execute_query();

    /*
    handles input given to program
    */
    int handle_load();

    /*
    pp : both relations are already processed
    */
    int pp_join(Predicate* predicate);
    int self_join(Predicate* predicate);
    int filter(Predicate* predicate);
        static inline int equal(uint64_t num1, uint64_t num2){ return num1 == num2;}
        static inline int greater_than(uint64_t num1, uint64_t num2){ return num1 > num2;}
        static inline int less_than(uint64_t num1, uint64_t num2){ return num1 < num2;}

};

#endif /* end of include guard: DBSYSTEM_H */
