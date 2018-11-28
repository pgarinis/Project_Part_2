#include <iostream>
#include <fstream>
#include <stdint.h>
#include <cstdlib>
#include <cstring>

class Node{
public:
    Node* next;
    uint64_t* tuple;

    Node(uint64_t* rows_array, int tuple_size){
        //allocate tuple according to query's number of relations
        tuple = (uint64_t*)malloc(tuple_size * sizeof(uint64_t));
        //set new node when created
        for(int i = 0; i < tuple_size; i++)
            tuple[i] = rows_array[i];
    }
    ~Node(){
        free(tuple);
    }
};

class SingleLinkedList{
    Node* head;
    Node* tail;
    int tuple_size;
public:
    SingleLinkedList(int num_of_query_relations):
        tuple_size(num_of_query_relations), head(NULL), tail(NULL){
        cout << "Single linked list was created. Tuple size : " << tuple_size << endl;
    }

    ~SingleLinkedList(){
        cout << "Destroying single linked list" << endl;
        Node* temp;
        while(head != NULL){
            temp = head->next;
            delete head;
            head = temp;
        }
    }

    //getters
    Node* get_head(){ return head;}
    Node* get_tail(){ return tail;}

    int add_node(uint64_t* rows_array){
        if(head == NULL){ //first time adding
            head = new Node(rows_array, tuple_size);
            head->next = NULL;
            tail = head;
        }
        else{
            tail->next = new Node(rows_array, tuple_size);
            tail = tail->next;
            tail->next = NULL;
        }
    }

    int remove_node(Node* previous, Node* current){
        //current node will be deleted
        if(current == head){
            head = current->next;
            delete current;
        }
        else if(current == tail){
            delete current;
            previous->next = NULL;
            tail = previous;
        }
        else{
            previous->next = current->next;
            delete current;
        }
    }

    void print_list(int index){
        Node* cur_node = head;
        while(cur_node != NULL){
            cout << cur_node->tuple[index] << endl;
            cur_node = cur_node->next;
        }
    }
};
