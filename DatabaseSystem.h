#include <iostream>
#include <fstream>
#include <stdint.h>
#include <cstdlib>
#include <cstring>
#include "Relation.h"

typedef struct{
    int relation_index;
    int column_num;
}Projection;

typedef struct{
    int relation1;
    int column1;
    int relation2;
    int column2;
    char op; // '=' or '>' or '<'
    uint64_t value; //if
}Predicate;


class DatabaseSystem{
    int num_of_relations;
    Relation** relations;//array of relations

    //for first field
    Relation** query_relations; //holds query's first part
    int num_of_query_relations;
    //for second field
    Predicate** predicates;
    int num_of_predicates;
    //for third field
    Projection** projections;
    int num_of_projections;

public:
    DatabaseSystem();
    ~DatabaseSystem();

    /*
    reads the names of the binary files that contain each relation
    and loads relations to heap
    */
    int load_relations();
    /*
    reads query from stdin
    format of query : ....|....|.....
    */
    int read_query();
};
