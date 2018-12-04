#pragma once
#include "NewColumnEntry.h"
#include <unordered_set>
using namespace std;

class NewColumnEntry2 : public NewColumnEntry{
    unordered_set<uint64_t>* related_row_ids;
    char is_set;

public:
    NewColumnEntry2():
        related_row_ids(new unordered_set<uint64_t>()), is_set(0)
    {
    }

    ~NewColumnEntry2(){
        delete related_row_ids;
    }

    unordered_set<uint64_t>* get_related_row_ids(){ return related_row_ids;}

    int add_to_set(uint64_t row_id1){
        related_row_ids->insert(row_id1);
    }

    void set_dirty(){ is_set = 1;}
    char get_is_set(){ return is_set;}
};
