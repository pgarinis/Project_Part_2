#include "stdint.h"
#include <bits/stdc++.h> //unordered_set

class NewColumnEntry2{
    uint64_t row_id;
    uint64_t value;
    std::unordered_set<uint64_t>* related_row_ids;

public:
    NewColumnEntry2(uint64_t row_id, uint64_t value):
    row_id(row_id), value(value), related_row_ids(new std::unordered_set<uint64_t>())
    {}
    ~NewColumnEntry2(){
        delete related_row_ids;
    }

    NewColumnEntry2(const NewColumnEntry2 &entry) {
      // allocate variables
      NewColumnEntry2(entry.row_id,entry.value);
      // copy values
      operator = (entry);
    }

    uint64_t get_row_id(){
        return row_id;
    }

    uint64_t get_value(){
        return value;
    }

    const NewColumnEntry2& operator = (const NewColumnEntry2 &entry) {
        this->related_row_ids = entry.related_row_ids;
        this->row_id = entry.row_id;
        this->value = entry.value;
        return *this;
    }

    int add_to_set(uint64_t row_id){
        related_row_ids->insert(row_id);
    }
};
