#pragma once
#include <stdint.h>
#include <unordered_set>
using namespace std;

class NewColumnEntry2{
    uint64_t row_id;
    uint64_t value;
    unordered_set<uint64_t>* related_row_ids;
    char is_set;

public:
    NewColumnEntry2():
        related_row_ids(new unordered_set<uint64_t>()), is_set(0)
    {
        cout << "constructor called!\n";
    }

    ~NewColumnEntry2(){
        delete related_row_ids;
    }

    //getters
    uint64_t get_row_id(){ return this->row_id;}
    uint64_t get_value() { return this->value;}
    char get_is_set(){ return is_set;}
    unordered_set<uint64_t>* get_related_row_ids(){ return related_row_ids;}

    void set(uint64_t row_id, uint64_t value){
        this->row_id = row_id;
        this->value = value;
        is_set = 1;
    }

    int add_to_set(uint64_t row_id1){
        related_row_ids->insert(row_id1);
    }
};
