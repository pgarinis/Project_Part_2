#ifndef JOINJOB_H
#define JOINJOB_H

#include <stdint.h>
#include "Job.h"
#include "Joiner.h"

class Joiner;
class JoinJob: public Job{
    Joiner* joiner;
    int bucket_num;
public:
    JoinJob(Joiner* j, int bucket_numm){
        bucket_num = bucket_numm;
        joiner = j;
    }
    ~JoinJob(){}

    int run(){
        //pointers to whole columns that will be joined
        NewColumnEntry* unindexed_relation = joiner->new_column[!joiner->join_index];
        NewColumnEntry* indexed_relation = joiner->new_column[joiner->join_index];
        int hash_value;

        //join buckets
        uint64_t start = joiner->psum_array[!joiner->join_index];
        uint64_t finish = start + joiner->hist_array[!joiner->join_index];

        for(uint64_t i = start; i < finish; i++){
            //for easier reading of code
            NewColumnEntry cur_row = unindexed_relation[i];
            uint64_t val = cur_row.get_value();

            //search index for this record
            int index = (joiner->index_array[bucket_num]).get_bucket_array()[joiner->h2(val)];

            //traverse chain array and push qualified row_ids to respective vector
            while(index != -1){
                if(indexed_relation[index + (joiner->psum_array[joiner->join_index][bucket_num]]).get_value() == val){
                    uint64_t row1 = cur_row.get_row_id();
                    uint64_t row2 = indexed_relation[index + joiner->psum_array[join_index][bucket_num]].get_row_id();
                    //insert to results with correct order
                    //FIRST push unindexed row id, THEN indexed row id
                    result_vectors[thread_num]->push_back(row1);
                    result_vectors[thread_num]->push_back(row2);

                }
                index = index_array[bucket_num].get_chain_array()[index];
            }
        }


        return 0;
    }
};

#endif
