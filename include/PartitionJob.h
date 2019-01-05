#ifndef PARTITIONJOB_H
#define PARTITIONJOB_H

#include "Job.h"


class PartitionJob: public Job{
public:
    PartitionJob(){
        cout << "Partition job created" << endl;
    }
    ~PartitionJob(){}

    int run(){
        cout << "Partition job run" << endl;
    }
};

#endif
