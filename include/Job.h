#ifndef JOB_H
#define JOB_H

// Abstract Class Job
class Job {
public:
    Job() = default;
    virtual ~Job() {}
    // This method should be implemented by subclasses.
    virtual int run() = 0;
};

#endif
