#ifndef HISTJOB_H
#define HISTJOB_H
#include <stdint.h>
#include "Job.h"
#include "Joiner.h"
class Joiner;
class HistogramJob: public Job{
    Joiner* joiner;
    int column_num;
    uint64_t start_offset;
    uint64_t finish_offset;
    uint64_t* sub_histogram;
public:
    HistogramJob(uint64_t* hist, Joiner* j, int col_num, uint64_t start, uint64_t finish){
        sub_histogram = hist;
        joiner = j;
        column_num = col_num;
        start_offset = start;
        finish_offset = finish;
    }
    ~HistogramJob(){}

    int run(){
        //column that will be processed
        uint64_t* column = joiner->column[column_num];

        //calculate sub histogram
        for(uint64_t i = start_offset; i <= finish_offset; i++)
            sub_histogram[joiner->h1(column[i])]++;
    }
};

#endif
