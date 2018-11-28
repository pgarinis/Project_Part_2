#include <iostream>
#include <fstream>
#include <stdint.h>
#include <cstdlib>
#include <cstring>


class BufferNode{
    uint64_t* tuple;
public:
    BufferNode(){
        this->tuple = (uint64_t*)malloc(4 * sizeof(uint64_t));
        this->tuple[0] = 0;
        this->tuple[1] = 0;
        this->tuple[2] = 0;
        this->tuple[3] = 0;
    };
    // BufferNode(uint64_t row_id, int relation_id){
    //     this->tuple = (uint64_t*)malloc(4 * sizeof(uint64_t));
    //     this->tuple[relation_id] = row_id;
    //
    // }
    ~BufferNode(){
        //free(tuple);
    }

    BufferNode(const BufferNode &bn) {

      // allocate variables
      BufferNode();

      // copy values
      operator = (bn);

    }

    uint64_t* get_row_ids(){
        return tuple;
    }

    const BufferNode& operator = (const BufferNode &bf) {
        this->tuple = (uint64_t*)malloc(4 * sizeof(uint64_t));
        //copy tuple
        tuple[0] = bf.tuple[0];
        tuple[1] =  bf.tuple[1];
        tuple[2] =  bf.tuple[2];
        tuple[3] =  bf.tuple[3];

        return *this;
    }
    //BufferNode(const uint64_t&,const int&);


    int add_row_id(uint64_t row_id, int relation_id){
        this->tuple[relation_id] = row_id;
    }
};
