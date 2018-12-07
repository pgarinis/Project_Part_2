#include "DatabaseSystem.h"
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <cstdlib>
#include <cstring>
#include "Relation.h"
#define BUFF_SIZE 256

DatabaseSystem::DatabaseSystem():
num_of_relations(0), relations(NULL),
result_buffer(new vector<uint64_t>()), joiner(new Joiner(&result_buffer))
{
}

DatabaseSystem::~DatabaseSystem(){
    for(int i = 0; i < num_of_relations; i++)
        delete relations[i];
    free(relations);
    delete joiner;
}

void DatabaseSystem::print_result_buffer(){
    if(result_buffer == NULL || result_buffer->size() == 0){
        cout << "NULL" << endl;
        return;
    }
    vector<uint64_t>::iterator it = result_buffer->begin();
    while( it != result_buffer->end()) {
        cout << *it + 1 << endl;
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

    //execute every predicate
    for(int i = 0; i < query->get_num_of_predicates(); i++){
        //filter predicate
        if(predicates[i]->relation2 == -1)
            filter(predicates[i]);
        //self_join predicate
        else if(predicates[i]->relation1 == predicates[i]->relation2)
            self_join(predicates[i]);
        //special case of join, where both relations are already processed
        //this function in similar to self join and Index is not needed
        else if((query->find_offset(predicates[i]->relation1) != -1) &&
                (query->find_offset(predicates[i]->relation2) != -1))
            pp_join(predicates[i]);
        //join predicate
        //2 cases :
        //1) both relations are unprocessed (if first predicate is join)
        //2) one of the two relations is already processed and the other isn't
        else
            joiner->handle_predicate(query, predicates[i]);
        cout << "Predicate handled successfully\n";
    }
    // print_result_buffer();
    for(int i = 0; i < query->get_tuple_size(); i++)
        cout << query->get_order()[i] << endl;

    delete result_buffer;
    delete query;
}

int DatabaseSystem::filter(Predicate* predicate){
    cout << "Filter predicate is handled...\n";
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

    //search whole original column (if first predicate is filter)
    if(result_buffer->size() == 0){
        //set order right
        query->get_order()[query->get_order_index()] = predicate->relation1;
        query->incr_order_index();

        //search whole original column
        uint64_t num_of_records = query->get_relations()[predicate->relation1]->get_num_of_records();
        for(uint64_t i = 0; i < num_of_records; i++)
            //if this row is qualified to pass the filter, push it in the result vector
            if(op_fun(column[i], predicate->value))
                result_buffer->push_back(i);
    }
    //search whole result_buffer since there are intermediate results in it
    else{
        //find relation's respective offset in tuple
        int offset = query->find_offset(predicate->relation1);

        //store new results in new_result_buffer
        vector<uint64_t>* new_result_buffer = new vector<uint64_t>;

        //iterate over result buffer and keep only tuples that are qualified
        vector<uint64_t>::iterator it = result_buffer->begin();
        while(it != result_buffer->end()) {
            if(op_fun(column[*(it + offset)], predicate->value))
                for(int i = 0; i < query->get_tuple_size(); i++) //push_back whole tuple
                    new_result_buffer->push_back(*(it+i));
            it += query->get_tuple_size();
        }

        //delete previous result buffer
        delete result_buffer;
        //set result_buffer to point to the new buffer that contains qualified tuples only
        result_buffer = new_result_buffer;
    }
}

int DatabaseSystem::self_join(Predicate* predicate){
    cout << "Self join is handled...\n";
    //get the columns the function will work with
    uint64_t* column1 = query->get_relations()[predicate->relation1]->get_column(predicate->column1);
    uint64_t* column2 = query->get_relations()[predicate->relation1]->get_column(predicate->column2);

    //search whole original column (if first predicate is filter)
    if(result_buffer->size() == 0){
        //set order right
        query->get_order()[query->get_order_index()] = predicate->relation1;
        query->incr_order_index();

        //search whole original column
        uint64_t num_of_records = query->get_relations()[predicate->relation1]->get_num_of_records();
        for(uint64_t i = 0; i < num_of_records; i++)
            //if this row is qualified to pass the filter, push it in the result vector
            if(column1[i] ==  column2[i])
                result_buffer->push_back(i);
    }
    //search whole result_buffer since there are intermediate results in it
    else{
        //find relation's respective offset in tuple
        int offset = query->find_offset(predicate->relation1);

        //store new results in new_result_buffer
        vector<uint64_t>* new_result_buffer = new vector<uint64_t>;

        //iterate over result buffer and keep only tuples that are qualified
        vector<uint64_t>::iterator it = result_buffer->begin();
        while(it != result_buffer->end()) {
            if(column1[*(it + offset)] == column2[*(it + offset)])
                for(int i = 0; i < query->get_tuple_size(); i++) //push_back whole tuple
                    new_result_buffer->push_back(*(it+i));
            it += query->get_tuple_size();
        }

        //delete previous result buffer
        delete result_buffer;
        //set result_buffer to point to the new buffer that contains qualified tuples only
        result_buffer = new_result_buffer;
    }
    return 0;
}

int DatabaseSystem::pp_join(Predicate* predicate){
    cout << "pp_join is handled...\n";
    //get the columns the function will work with
    uint64_t* column1 = query->get_relations()[predicate->relation1]->get_column(predicate->column1);
    uint64_t* column2 = query->get_relations()[predicate->relation2]->get_column(predicate->column2);

    //find right offsets
    int offset1 = query->find_offset(predicate->relation1);
    int offset2 = query->find_offset(predicate->relation2);

    //store new results in new_result_buffer
    vector<uint64_t>* new_result_buffer = new vector<uint64_t>;

    //search result_buffer
    vector<uint64_t>::iterator it = result_buffer->begin();
    while(it != result_buffer->end()) {
        if(column1[*(it + offset1)] == column2[*(it + offset2)]){
            for(int i = 0; i < query->get_tuple_size(); i++)//push_back whole tuple
                new_result_buffer->push_back(*(it+i));
        }
        it += query->get_tuple_size();
    }

    //delete previous result buffer
    delete result_buffer;
    //set result_buffer to point to the new buffer that contains qualified tuples only
    result_buffer = new_result_buffer;

    return 0;
}
