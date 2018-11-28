#pragma once
#include <stdint.h>

class NewColumnEntry{
    int index;
    uint64_t value;
public:
    //sets tuple
    void set(int index, uint64_t value){
        this->index = index;
        this->value = value;
    }
    int get_index(){ return this->index; };
    uint64_t get_value() { return this->value; };
};
