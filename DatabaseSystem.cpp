#include "DatabaseSystem.h"
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <cstdlib>
#include <cstring>
#include "Relation.h"
#define BUFF_SIZE 256

DatabaseSystem::DatabaseSystem():
num_of_relations(0), relations(NULL), result_buffer(NULL)
{

}

DatabaseSystem::~DatabaseSystem(){
    for(int i = 0; i < num_of_relations; i++)
        delete relations[i];
    free(relations);
}

void DatabaseSystem::print_result_buffer(){
    list<BufferNode>::iterator it = result_buffer->begin();
    while( it != result_buffer->end()) {
        cout << "= (" << it->get_row_ids()[0]<< ", "  << it->get_row_ids()[1] << ", " << it->get_row_ids()[2] << ", " << it->get_row_ids()[3] << ") =" << endl;
        it++;
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

    //for every predicate
    for(int i = 0; i < query->get_num_of_predicates(); i++){
        if(predicates[i]->relation2 == -1){//FILTER
            filter(predicates[i]);
        }
        if(predicates[i]->relation1 == predicates[i]->relation2){//SELF JOIN
            self_join(predicates[i]);
        }
        else{//JOIN
            join(predicates[i]);
        }
        // results->print_list(predicates[i]->relation1);
        cout << "Predicate DONE\n";
    }
    //results->print_list(predicates[0]->relation1);
    print_result_buffer();
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
            // if(column1[i] == column2[i]){
            //     //cout << column1[i] << endl;
            //     //cout << i + 1<< endl;
            //
            //     //if(result_buffer == NULL)
            //
            //
            // }
        }
    }
    else{ //search result list

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
                    result_buffer = new list<BufferNode>();
                BufferNode* bn = new BufferNode();
                bn->add_row_id(i,predicate->relation1);
                result_buffer->push_back(*bn);
                delete bn;
            }
        }
    }
    else{ //search result list
        list<BufferNode>::iterator it = result_buffer->begin();
        while( it != result_buffer->end()) {
            if(!op_fun(column[it->get_row_ids()[predicate->relation1]], predicate->value)){
                //list<BufferNode>::iterator it_temp = it;
                //it++;
                it = result_buffer->erase(it);
                continue;
            }
            it++;
        }

    }

}

int DatabaseSystem::join(Predicate* predicate){
    //both relatinos are processed already
    // if([predicate->relation1] && [predicate->relation2]){
    //     //metasximatismos listas parallhla se 2 pinakes r1' r2'
    //
    //     //index ton mikrotero pinaka
    //
    //
    // }
    // //only relation1 is processed already
    // else if(is_processed[predicate->relation1]){
    //
    // }
    // //only relation2 is processed already
    // else if(is_processed[predicate->relation2]){
    //
    // }
    //neiter of the 2 relations are processed, results is NULL
    // else if(result_buffer == NULL)
    // {
    //
    // }
}
