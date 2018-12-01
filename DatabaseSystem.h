#ifndef DBSYSTEM_H
#define DBSYSTEM_H


#include "Query.h"
#include <vector>
#include "Joiner.h"

using namespace std;

class DatabaseSystem{
    int num_of_relations;
    Relation** relations;

    //holds the current query given
    Query* query;

    vector<uint64_t>* result_buffer;

    Joiner* joiner;

    int* is_processed;

public:

    /*
    rearranges every column's records in order to create a new column where records
    are placed in different order (depending on their hash values from h1 hash function)
    */
    int segmentation();
        /*
        creates and computes hist array for the relation given as input
        */
        int create_and_compute_hist_array(Relation* relation);
        /*
        creates and computes psum array for the relation given as input
        */
        int create_and_compute_psum_array(Relation* relation);
        /*
        creates and computes new column of the relation(R'), where rows are sorted by their h1 hash value
        */
        int create_and_compute_new_column(Relation* relation);
    /*
    creates an index(hash table) on one of the new columns created from segmentation
    */
    int indexing();
        /*
        creates and initialises chain and bucket array from the bucket's index
        */
        //int create_and_init_chain_and_bucket_array(Index& index, int hist_array_value);

    ////////////////////////////////////////////////////////////////////////
    DatabaseSystem();
    ~DatabaseSystem();

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
    3 categories of predicates
    o : original
    n : not original (take 'column' from temp results)
    */
    int join(Predicate* predicate,int*);
        int join_oo(Predicate* predicate);//part 1
        int join_on(Predicate* predicate);
        int join_nn(Predicate* predicate);
    int self_join(Predicate* predicate);
    int filter(Predicate* predicate);
        static inline int equal(uint64_t num1, uint64_t num2){ return num1 == num2;}
        static inline int greater_than(uint64_t num1, uint64_t num2){ return num1 > num2;}
        static inline int less_than(uint64_t num1, uint64_t num2){ return num1 < num2;}

};


#endif /* end of include guard: DBSYSTEM_H */
