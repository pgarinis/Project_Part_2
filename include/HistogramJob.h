#ifndef HISTJOB_H
#define HISTJOB_H

#include "Job.h"

class HistogramJob: public Job{

public:
    HistogramJob(){
        cout << "Hist job created" << endl;
    }
    ~HistogramJob(){}

    int run(){
        cout << "Hist job run" << endl;
    }
};

#endif
