#include "DatabaseSystem.h"
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <cstdlib>
#include <cstring>
#include "Relation.h"
#define BUFF_SIZE 256

DatabaseSystem::DatabaseSystem():
num_of_relations(0), relations(NULL), result_buffer(NULL), joiner(NULL)
{
}

DatabaseSystem::~DatabaseSystem(){
    for(int i = 0; i < num_of_relations; i++)
        delete relations[i];
    free(relations);
}

void DatabaseSystem::print_result_buffer(){
    if(result_buffer == NULL || result_buffer->size() == 0){
        cout << "NULL" << endl;
        return;
    }
    vector<uint64_t>::iterator it = result_buffer->begin();
    while( it != result_buffer->end()) {
        cout << *it << endl;
        it+=1;
    }
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
            if(is_processed[predicates[i]->relation1] == 0){
                is_processed[predicates[i]->relation1] = 1;
                query->incr_num_of_processed_relations();
            }
        }
        if(predicates[i]->relation1 == predicates[i]->relation2){//SELF JOIN
            self_join(predicates[i]);
            if(is_processed[predicates[i]->relation1] == 0){
                is_processed[predicates[i]->relation1] = 1;
                query->incr_num_of_processed_relations();
            }
        }
        else{//JOIN
            join(predicates[i], is_processed);
        }
        // results->print_list(predicates[i]->relation1);
        cout << "Predicate DONE\n";
    }
    //results->print_list(predicates[0]->relation1);
    print_result_buffer();

    free(is_processed);
    delete result_buffer;
    delete query;
}


int DatabaseSystem::self_join(Predicate* predicate){
    //get the columns the function will work with
    uint64_t* column1 = query->get_relations()[predicate->relation1]->get_column(predicate->column1);
    uint64_t* column2 = query->get_relations()[predicate->relation1]->get_column(predicate->column2);

    //search either results or respective column, depending on previous predicates executed
    if(result_buffer == NULL){ //search whole original column
        for(uint64_t i = 0; i < query->get_relations()[predicate->relation1]->get_num_of_records(); i++){
            //if node is qualified to pass the filter, add it to the list
            if(column1[i] ==  column2[i]){
                //cout << column[i] << endl;
                //cout << i + 1<< endl;
                if(result_buffer == NULL)
                    result_buffer = new vector<uint64_t>();
                result_buffer->push_back(i);
            }
        }
    }
    else{ //search result_buffer
        vector<uint64_t>* new_result_buffer = new vector<uint64_t>();
        vector<uint64_t>::iterator it = result_buffer->begin() + predicate->relation1;
        while(it != result_buffer->end()) {
            if(column1[*it] == column2[*it]){
                //push_back whole tuple
                it -= predicate->relation1;
                for(int i = 0; i < query->get_num_of_processed_relations(); i++)
                    new_result_buffer->push_back(*(it+i));
                it += predicate->relation1;
            }
            it += query->get_num_of_processed_relations();
        }
        delete result_buffer;
        result_buffer = new_result_buffer;
        //TODO : TON BUFFER NA TON DIMIOURGOUME EKSW APO TO FOR
    }


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

    //search either results or respective column, depending on previous predicates executed
    if(result_buffer == NULL){ //search whole original column
        for(uint64_t i = 0; i < query->get_relations()[predicate->relation1]->get_num_of_records(); i++){
            //if node is qualified to pass the filter, add it to the list
            if(op_fun(column[i], predicate->value)){
                //cout << column[i] << endl;
                //cout << i + 1<< endl;
                if(result_buffer == NULL)
                    result_buffer = new vector<uint64_t>();
                result_buffer->push_back(i);
            }
        }
    }
    else{ //search result_buffer
        vector<uint64_t>* new_result_buffer = new vector<uint64_t>();
        vector<uint64_t>::iterator it = result_buffer->begin() + predicate->relation1;
        while(it != result_buffer->end()) {
            if(op_fun(column[*it], predicate->value)){
                //push_back whole tuple
                it -= predicate->relation1;
                for(int i = 0; i < query->get_num_of_processed_relations(); i++)
                    new_result_buffer->push_back(*(it+i));
                it += predicate->relation1;
            }
            it += query->get_num_of_processed_relations();
        }
        delete result_buffer;
        result_buffer = new_result_buffer;
    }

}

int DatabaseSystem::join(Predicate* predicate, int* is_processed){
    //both relatinos are processed already
    if(is_processed[predicate->relation1] && is_processed[predicate->relation2]){
        //metasximatismos listas parallhla se 2 pinakes r1' r2'
        //join_nn()
        //index ton mikrotero pinaka

        //new joiner to handle join predicate with type 0 (not original - not original)
        this->joiner = new Joiner(this->query, predicate, is_processed, this->result_buffer,0);


    }
    //only relation1 is processed already
    else if(is_processed[predicate->relation1]){
        //join_on()
    }
    //only relation2 is processed already
    else if(is_processed[predicate->relation2]){

    }
    //neiter of the 2 relations are processed, results is NULL
    else if(result_buffer == NULL)
    {
        //join_oo()
    }
}
//
// int DatabaseSystem::join_on(Predicate* predicate){
//     //relation1->original
//     //relation2->not original
//
//     //relation1
//
// }
