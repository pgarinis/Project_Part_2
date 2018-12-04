#pragma once
#include <stdint.h>

class NewColumnEntry{
    uint64_t row_id;
    uint64_t value;
public:
    NewColumnEntry():row_id(-1){

    }
    //getter
    uint64_t get_row_id(){ return this->row_id;}
    uint64_t get_value() { return this->value;}

    //sets tuple
    void set(uint64_t row_id, uint64_t value){
        this->row_id = row_id;
        this->value = value;
    }
};
