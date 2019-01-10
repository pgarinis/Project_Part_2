#ifndef JOINER_H
#define JOINER_H

#include <unordered_set>
#include <fstream>
#include <stdint.h>
#include <string>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <math.h>
#include <vector>
using namespace std;

#include "Query.h"
#include "Index.h"
#include "NewColumnEntry.h"

class JobScheduler;

class Joiner{
public:
    //pass from dbsystem
    Query* query;
    Predicate* predicate;
    vector<uint64_t>** result_buffer;

    int join_type;
    //0 : oo
    //2 : no

    uint64_t* psum_array[2];
    uint64_t* hist_array[2];
    NewColumnEntry* new_column[2];
    uint64_t* column[2]; //pointers to original columns
    unordered_set<uint64_t>* temp_set;
    Index* index_array;
    int join_index;
    JobScheduler* job_scheduler;

    //variables needed for hash functions
    int h1_num_of_buckets;
    int h2_num_of_buckets;
    int h1_num_of_bits;
    int h2_num_of_bits;

    Joiner(vector<uint64_t>** result_buffer);
    ~Joiner();
    /*  getters - setters */

    int get_h1_num_of_buckets(){
      return h1_num_of_buckets;
    }

    int get_h2_num_of_buckets(){
      return h2_num_of_buckets;
    }

    vector<uint64_t>** get_result_buffer(){
        return this->result_buffer;
    }

    //hash function for segmentation
    inline int h1(uint64_t num){return (num & (h1_num_of_buckets - 1));}
    //hash function for indexing
    inline int h2(uint64_t num){return num % 16699;}

    int handle_predicate(Query* query, Predicate* predicate);

    /*
    rearranges every column's records in order to create a new column where records
    are placed in different order (depending on their hash values from h1 hash function)
    */
    int segmentation();
        /*
        creates and computes hist array for the relation given as input
        */
        int create_and_compute_hist_array();
        /*
        creates and computes psum array for the relation given as input
        */
        int create_and_compute_psum_array();
        /*
        creates and computes new column of the relation(R'), where rows are sorted by their h1 hash value
        */
         int create_and_compute_new_column();

     /*
     creates an index(hash table) on one of the new columns created from segmentation
     */
    int indexing();
        /*
        creates and initialises chain and bucket array from the bucket's index
        */
        int create_and_init_chain_and_bucket_array(Index* index, uint64_t hist_array_value);
    /*
    // joins two columns(relations) and return index to the list that holds the results:
    // return value = ---->[1MB mem block]---->[1MB mem block]--->(...)--->NULL
    // [1MB mem block] format : [uint64_t|uint64_t|uint64_t|uint64_t...] without '|'
    // each pair is a result from the join query
    // */
    int join();
};

#endif /* end of include guard: JOINER_H */
