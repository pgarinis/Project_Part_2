#include "DatabaseSystem.h"
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <cstdlib>
#include <cstring>

using namespace std;
#define BUFF_SIZE 256

DatabaseSystem::DatabaseSystem():
num_of_relations(0), relations(NULL), result_lists(NULL)
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
    //initialise lists
    result_lists = (ResultList**)malloc(query->get_num_of_relations() * sizeof(ResultList*));
    for(int i = 0; i < query->get_num_of_relations(); i++)
        result_lists[i] = NULL;

    //do the main job
    //0 2 4|0.1=1.2&1.0=2.1&0.1>3000|0.0 1.1
    /*
    int relation1;
    int column1;
    int relation2;
    int column2;
    char op; // '=' or '>' or '<'
    uint64_t value;
    */
    Predicate** predicates = query->get_predicates();
    int limit = query->get_num_of_predicates();
    for(int i = 0; i < limit; i++){
        if(predicates[i]->relation2 == -1){//FILTER
            if(result_lists[predicates[i]->relation1] == NULL){//original
                filter_o(predicates[i]);
            }

        }
        if(predicates[i]->relation1 == predicates[i]->relation2){//SELF JOIN
        }
        else{//JOIN

        }
    }

    for(int i = 0; i < query->get_num_of_relations(); i++)
        if(result_lists[i] != NULL)
            delete result_lists[i];
    free(result_lists);
    delete(query);
}

int DatabaseSystem::filter_o(Predicate* predicate){
    //relations we are working with
    Relation* relation = query->get_relations()[predicate->relation1];

    if(result_lists[predicate->relation1] == NULL)
        result_lists[predicate->relation1] = new ResultList();

    uint64_t* column = relation->get_column(predicate->column1);

    for(int i = 0; i < relation->get_num_of_records(); i++){
        if(predicate->op == '='){
            if(column[i] ==  predicate->value)
                result_lists[predicate->relation1]->add_result(column[i]);
        }
        else if(predicate->op == '>'){
            if(column[i] >  predicate->value)
                result_lists[predicate->relation1]->add_result(column[i]);
        }
        else if(predicate->op == '<'){
            if(column[i] < predicate->value)
                result_lists[predicate->relation1]->add_result(column[i]);
        }
    }

    if(result_lists[predicate->relation1]->get_head_node()->get_cur_index() == 0){
        delete result_lists[predicate->relation1];
        result_lists[predicate->relation1] = NULL;
    }

}
