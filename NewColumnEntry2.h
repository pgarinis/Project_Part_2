#pragma once
#include "NewColumnEntry.h"
#include <unordered_set>
using namespace std;

class NewColumnEntry2 : public NewColumnEntry{
    unordered_set<uint64_t>* related_row_ids;

public:
    NewColumnEntry2():
        related_row_ids(new unordered_set<uint64_t>())
    {
        NewColumnEntry();
    }

    ~NewColumnEntry2(){
        delete related_row_ids;
    }

    unordered_set<uint64_t>* get_related_row_ids(){ return related_row_ids;}

    int add_to_set(uint64_t row_id1){
        related_row_ids->insert(row_id1);
    }
};
