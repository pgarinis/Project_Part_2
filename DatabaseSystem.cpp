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
            filter(predicates[i]);

        }
        if(predicates[i]->relation1 == predicates[i]->relation2){//SELF JOIN
            self_join(predicates[i]);
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

/*
Decides if filter will be applied to initial column or result list
*/
int DatabaseSystem::filter(Predicate* predicate){
    if(result_lists[predicate->relation1] == NULL)
        filter_o(predicate);
    else
        filter_n(predicate);
}

/*
Applies filter to column
*/
int DatabaseSystem::filter_o(Predicate* predicate){
    //relation we are working with
    Relation* relation = query->get_relations()[predicate->relation1];
    uint64_t* column = relation->get_column(predicate->column1);
    uint64_t num_of_records = relation->get_num_of_records();

    //create new result list since there isn't any for this relation
    result_lists[predicate->relation1] = new ResultList();

    //set operator function accordingly (< or > or = operators are possible)
    int (*op_fun)(uint64_t, uint64_t);
    if(predicate->op == '=')
        op_fun = &equal;
    else if(predicate->op == '>')
        op_fun = &greater_than;
    else if(predicate->op == '<')
        op_fun = &less_than;

    //scan whole columnn and apply filter, then store ids at result list
    for(int i = 0; i < num_of_records; i++){
        if(op_fun(column[i], predicate->value)){
            // cout << column[i] << endl;
            // cout << i + 1<< endl;
            result_lists[predicate->relation1]->add_result(i);
        }
    }

    //if no id was added to result list, delete result
    if(result_lists[predicate->relation1]->get_head_node()->get_cur_index() == 0){
        delete result_lists[predicate->relation1];
        result_lists[predicate->relation1] = NULL;//TODO: return NULL to query
    }
}

/*
Applies filter to result list
*/
int DatabaseSystem::filter_n(Predicate* predicate){
    //result list we are working with
    ResultList* result_list = result_lists[predicate->relation1];
    ResultNode* cur_node = result_list->get_head_node();

    //column is needed to check values of rows
    uint64_t* column = query->get_relations()[predicate->relation1]->get_column(predicate->column1);

    //create new list to store new results
    ResultList* new_list = new ResultList();

    //set op fun accordingly
    int (*op_fun)(uint64_t, uint64_t);
    if(predicate->op == '=')
        op_fun = &equal;
    else if(predicate->op == '>')
        op_fun = &greater_than;
    else if(predicate->op == '<')
        op_fun = &less_than;

    //scan whole result_list and apply filter, then store new results to new_list
    while(cur_node != NULL){
        uint64_t* id_array = cur_node->get_id_array();
        int limit = cur_node->get_cur_index();
        for(int i = 0; i < limit; i++){
            if(op_fun(column[id_array[i]], predicate->value)){
                // cout << column[id_array[i]] << endl;
                // cout << id_array[i]+1 << endl;
                new_list->add_result(id_array[i]);
            }
        }
        cur_node = cur_node->get_next_node();
    }

    //delete previous result_list and set result_list to new_list
    delete result_lists[predicate->relation1];
    if(new_list->get_head_node()->get_cur_index() == 0){
        delete new_list;
        result_lists[predicate->relation1] = NULL;//TODO: return NULL to query
    }
    else
        result_lists[predicate->relation1] = new_list;
}

int DatabaseSystem::self_join(Predicate* predicate){
    if(result_lists[predicate->relation1] == NULL)
        self_join_o(predicate);
    else
        self_join_n(predicate);
}

int DatabaseSystem::self_join_o(Predicate* predicate){
    //relation we are working with
    Relation* relation = query->get_relations()[predicate->relation1];
    uint64_t* column1 = relation->get_column(predicate->column1);
    uint64_t* column2 = relation->get_column(predicate->column2);
    uint64_t num_of_records = relation->get_num_of_records();

    //create new result list since there isn't any for this relation
    result_lists[predicate->relation1] = new ResultList();

    //set operator function accordingly (< or > or = operators are possible)
    int (*op_fun)(uint64_t, uint64_t);
    if(predicate->op == '=')
        op_fun = &equal;
    else if(predicate->op == '>')
        op_fun = &greater_than;
    else if(predicate->op == '<')
        op_fun = &less_than;

    //scan whole columnn and apply filter, then store ids at result list
    for(int i = 0; i < num_of_records; i++){
        if(op_fun(column1[i], column2[i])){
            // cout << column1[i] << " vs " << column2[i]  << endl;
            // cout <<"g"<< i + 1<< endl;
            result_lists[predicate->relation1]->add_result(i);
        }
    }

    //if no id was added to result list, delete result
    if(result_lists[predicate->relation1]->get_head_node()->get_cur_index() == 0){
        delete result_lists[predicate->relation1];
        result_lists[predicate->relation1] = NULL;//TODO: return NULL to query
    }
}

int DatabaseSystem::self_join_n(Predicate* predicate){
    //result list we are working with
    ResultList* result_list = result_lists[predicate->relation1];
    ResultNode* cur_node = result_list->get_head_node();

    //column is needed to check values of rows
    uint64_t* column1 = query->get_relations()[predicate->relation1]->get_column(predicate->column1);
    uint64_t* column2 = query->get_relations()[predicate->relation1]->get_column(predicate->column2);

    //create new list to store new results
    ResultList* new_list = new ResultList();

    //set op fun accordingly
    int (*op_fun)(uint64_t, uint64_t);
    if(predicate->op == '=')
        op_fun = &equal;
    else if(predicate->op == '>')
        op_fun = &greater_than;
    else if(predicate->op == '<')
        op_fun = &less_than;

    //scan whole result_list and apply filter, then store new results to new_list
    while(cur_node != NULL){
        uint64_t* id_array = cur_node->get_id_array();
        int limit = cur_node->get_cur_index();
        for(int i = 0; i < limit; i++){
            if(op_fun(column1[id_array[i]], column2[id_array[i]])){
                // cout << column1[id_array[i]] << endl;
                // cout <<"g"<< id_array[i]+1 << endl;
                new_list->add_result(id_array[i]);
            }
        }
        cur_node = cur_node->get_next_node();
    }

    //delete previous result_list and set result_list to new_list
    delete result_lists[predicate->relation1];
    if(new_list->get_head_node()->get_cur_index() == 0){
        delete new_list;
        result_lists[predicate->relation1] = NULL;//TODO: return NULL to query
    }
    else
        result_lists[predicate->relation1] = new_list;
}
