#include "NewColumnEntry.h"
#include "Index.h"

class JoinColumn{
    NewColumnEntry* new_column;
    Index* index_array;
    uint64_t* psum_array;
    uint64_t* hist_array;
    uint64_t* column;
    uint64_t col_size;

public:
    JoinColumn(uint64_t* col, uint64_t col_sz){
        this->column = col;
        this->col_size = col_sz;
        this->new_column = NULL;
        this->hist_array = NULL;
        this->psum_array = NULL;
        this->index_array = NULL;
    };
    ~JoinColumn(){
        free(this->new_column);
        free(this->hist_array);
        free(this->psum_array);
        delete[] index_array;
    }

    //setters
    void set_new_column(uint64_t size){ this->new_column = (NewColumnEntry*)malloc(size * sizeof(NewColumnEntry)); }
    void set_hist_array(int size){ this->hist_array = (int*)malloc(size * sizeof(int)); }
    void set_psum_array(int size){ this->psum_array = (int*)malloc(size * sizeof(int)); }
    void set_index_array(int size){this->index_array = new Index[size]; }

    //getters
    uint64_t get_num_of_records(){ return this->col_size; };
    uint64_t* get_column(){ return this->column; };
    NewColumnEntry* get_new_column(){ return this->new_column; };
    int* get_hist_array(){ return this->hist_array; };
    int* get_psum_array(){ return this->psum_array; }
    Index* get_index_array(){ return this->index_array; };
};
