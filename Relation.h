#pragma once
#include <stdint.h>

class Relation{
    uint64_t num_of_records;
    uint64_t num_of_columns;
    uint64_t* columns;

public:
    //constructor & destructor
    Relation(char* file_name);
    ~Relation();

    //getters
    uint64_t get_num_of_records(){ return this->num_of_records; };
    uint64_t get_num_of_columns(){ return this->num_of_columns; };
    //returns a the index of col_num-th column
    uint64_t* get_column(int col_num){ return this->columns + col_num * num_of_records;}
};
