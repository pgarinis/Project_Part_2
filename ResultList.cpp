#include "ResultList.h"
#include <iostream>
#include <cstdlib>

using namespace std;

ResultNode::ResultNode(){
    id_array = (uint64_t*) malloc(1048576 * sizeof(uint64_t));
    next_node = NULL;
    cur_index = 0;
}

ResultNode::~ResultNode(){
    free(id_array);
}

ResultNode* ResultNode::add_id(uint64_t new_id){
    id_array[cur_index++] = new_id;
    if(cur_index == 1048576){
        next_node = new ResultNode();
        cout << "new result node created" << endl;
        return next_node;
    }
    return this;
}


ResultList::ResultList(){
    head_node = new ResultNode();
    cur_node = head_node;
}

ResultList::~ResultList(){
    ResultNode* temp;
    while(head_node != NULL){
        temp = head_node;
        head_node = head_node->get_next_node();
        delete(temp);
    }
}

int ResultList::add_result(uint64_t new_id){
    cur_node = cur_node->add_id(new_id);
}
