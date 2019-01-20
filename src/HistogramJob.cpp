#include "../include/HistogramJob.h"


int HistogramJob::run(){
    //column that will be processed
    uint64_t* column = joiner->column[column_num];

    //calculate sub histogram
    for(uint64_t i = start_offset; i <= finish_offset; i++)
        sub_histogram[joiner->h1(column[i])]++;
    return 0;
}
