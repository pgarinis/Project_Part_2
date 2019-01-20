#pragma once
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <cstdlib>

using namespace std;

class Relation{
    uint64_t num_of_records;
    uint64_t num_of_columns;
    uint64_t* columns;

public:
    //statistics //will be arrays of uint64_t
    uint64_t* l;
    uint64_t* u;
    uint64_t* f;
    uint64_t* d;

    Relation(char* file_name);
    ~Relation();

    //getters
    uint64_t get_num_of_records(){ return this->num_of_records; }
    uint64_t get_num_of_columns(){ return this->num_of_columns; }
    //returns a the index of col_num-th column
    uint64_t* get_column(int col_num){ return this->columns + (col_num * num_of_records); }
};
