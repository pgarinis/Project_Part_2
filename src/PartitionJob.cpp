#include "../include/PartitionJob.h"

int PartitionJob::run(){
    //column that will be processed
    uint64_t* column = joiner->column[column_num];
    NewColumnEntry* new_column = joiner->new_column[column_num];
    int hash_value;

    //place rows to new column
    for(uint64_t i = start_offset; i <= finish_offset; i++){
        hash_value = joiner->h1(column[i]);
        new_column[sub_indices[hash_value]].set(i, column[i]);
        sub_indices[hash_value]++;
    }
    return 0;
}
