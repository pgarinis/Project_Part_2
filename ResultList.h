#include <stdint.h>

class ResultNode{
    uint64_t* id_array;
    ResultNode* next_node;
    int cur_index; //index to node currently adding ids to
public:
    ResultNode();
    ~ResultNode();

    //getters
    ResultNode* get_next_node(){ return next_node;}
    uint64_t* get_id_array(){ return id_array;}
    int get_cur_index(){ return cur_index;}

    ResultNode* add_id(uint64_t new_id);
};

class ResultList{
    ResultNode* head_node;
    ResultNode* cur_node;
  public:
    ResultList();
    ~ResultList();

    int add_result(uint64_t new_id);
    ResultNode* get_head_node(){return this->head_node;}
};
