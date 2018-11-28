#include "DatabaseSystem.h"
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <cstdlib>
#include <cstring>
#include "Relation.h"
#define BUFF_SIZE 256

DatabaseSystem::DatabaseSystem():
num_of_relations(0), relations(NULL), results(NULL)
{

}

DatabaseSystem::~DatabaseSystem(){
    for(int i = 0; i < num_of_relations; i++)
        delete relations[i];
    free(relations);
}

int DatabaseSystem::load_relations(){
    //temp buffer to store file name that is read from stdin
    char file_name[BUFF_SIZE];

    //read file names from stdin until "Done" is received
    while(cin.getline(file_name, BUFF_SIZE)){
        if(!strcmp(file_name, "Done"))
            break;

        //new file name was given; adjust num_of_relations and create new relation
        relations = (Relation**)realloc(relations, (++num_of_relations) * sizeof(Relation*));
        relations[num_of_relations - 1] = new Relation(file_name);
    }

    return 0;
}

int DatabaseSystem::construct_query(){
    this->query = new Query();
    this->query->read_query(relations, num_of_relations);
    //TODO : delete query after it has been executed
}

int DatabaseSystem::execute_query(){
    //get predicates from query in order to iterate over them
    Predicate** predicates = query->get_predicates();

    //is_processed[0] == 0 means that query's relation with relative index 0
    //hasn't been processed yet in this query
    int* is_processed = (int*)calloc(query->get_num_of_relations(), sizeof(int));

    //for every predicate
    for(int i = 0; i < query->get_num_of_predicates(); i++){
        if(predicates[i]->relation2 == -1){//FILTER
            filter(predicates[i]);
            is_processed[predicates[i]->relation1] = 1;
        }
        if(predicates[i]->relation1 == predicates[i]->relation2){//SELF JOIN
            self_join(predicates[i]);
            is_processed[predicates[i]->relation1] = 1;
        }
        else{//JOIN
            join(predicates[i], is_processed);
        }
        // results->print_list(predicates[i]->relation1);
        cout << "Predicate DONE\n";
    }
    //results->print_list(predicates[0]->relation1);

    free(is_processed);
    delete results;
    delete query;
}


int DatabaseSystem::self_join(Predicate* predicate){
    //get the columns the function will work with
    uint64_t* column1 = query->get_relations()[predicate->relation1]->get_column(predicate->column1);
    uint64_t* column2 = query->get_relations()[predicate->relation1]->get_column(predicate->column2);

    //temporal tuple used to hold current tuple while searching
    uint64_t* tuple = (uint64_t*)malloc(query->get_num_of_relations() * sizeof(uint64_t));

    //search either results or respective column, depending on previous predicates executed
    if(results == NULL){ //search whole original column
        for(uint64_t i = 0; i < query->get_relations()[predicate->relation1]->get_num_of_records(); i++){
            //if node is qualified to pass the filter, add it to the list
            if(column1[i] == column2[i]){
                //cout << column1[i] << endl;
                //cout << i + 1<< endl;
                tuple[predicate->relation1] = i;
                if(results == NULL)
                    results = new SingleLinkedList(query->get_num_of_relations());
                results->add_node(tuple);
            }
        }
    }
    else{ //search result list
        //prev_node is needed for removing a node
        Node* prev_node = NULL;
        //cur_node is needed to search the result list
        Node* cur_node = results->get_head();
        while(cur_node != NULL){
            //remove node from list if it fails to pass the filter
            if(column1[cur_node->tuple[predicate->relation1]] != column2[cur_node->tuple[predicate->relation1]]){
                Node* temp = cur_node->next;
                results->remove_node(prev_node, cur_node);
                cur_node = temp;
            }
            else{
                cout << "YOLOOO" << endl;
                prev_node = cur_node;
                cur_node = cur_node->next;
            }
        }
    }

    free(tuple);
}

int DatabaseSystem::filter(Predicate* predicate){
    //get column the function will work with
    uint64_t* column = query->get_relations()[predicate->relation1]->get_column(predicate->column1);

    //set operator function accordingly (< or > or = operators are possible)
    int (*op_fun)(uint64_t, uint64_t);
    if(predicate->op == '=')
        op_fun = &equal;
    else if(predicate->op == '>')
        op_fun = &greater_than;
    else if(predicate->op == '<')
        op_fun = &less_than;

    //temporal tuple used to hold current tuple while searching
    uint64_t* tuple = (uint64_t*)malloc(query->get_num_of_relations() * sizeof(uint64_t));

    //search either results or respective column, depending on previous predicates executed
    if(results == NULL){ //search whole original column
        for(uint64_t i = 0; i < query->get_relations()[predicate->relation1]->get_num_of_records(); i++){
            //if node is qualified to pass the filter, add it to the list
            if(op_fun(column[i], predicate->value)){
                //cout << column[i] << endl;
                //cout << i + 1<< endl;
                tuple[predicate->relation1] = i;
                if(results == NULL)
                    results = new SingleLinkedList(query->get_num_of_relations());
                results->add_node(tuple);
            }
        }
    }
    else{ //search result list
        //prev_node is needed for removing a node
        Node* prev_node = NULL;
        //cur_node is needed to search the result list
        Node* cur_node = results->get_head();
        while(cur_node != NULL){
            //remove node from list if it fails to pass the filter
            if(!op_fun(column[cur_node->tuple[predicate->relation1]], predicate->value)){
                Node* temp = cur_node->next;
                results->remove_node(prev_node, cur_node);
                cur_node = temp;
            }
            else{
                prev_node = cur_node;
                cur_node = cur_node->next;
            }
        }
    }

    free(tuple);
}

int DatabaseSystem::join(Predicate* predicate, int* is_processed){
    //both relatinos are processed already
    if(is_processed[predicate->relation1] && is_processed[predicate->relation2]){
        //metasximatismos listas parallhla se 2 pinakes r1' r2'

        //index ton mikrotero pinaka


    }
    //only relation1 is processed already
    else if(is_processed[predicate->relation1]){

    }
    //only relation2 is processed already
    else if(is_processed[predicate->relation2]){

    }
    //neiter of the 2 relations are processed, results is NULL
    else if(results == NULL)
    {

    }
}
