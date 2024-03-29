#ifndef QUERY_H
#define QUERY_H

#include "Relation.h"

typedef struct{
    int relation_index;
    int column_index;
}Projection;

typedef struct{
    int relation1;
    int column1;
    int relation2;
    int column2;
    char op; // '=' or '>' or '<'
    uint64_t value; //if
}Predicate;

class Query{
    //for first field
    Relation** relations; //holds query's first part
    int num_of_relations;
    //for second field
    Predicate** predicates;
    int num_of_predicates;
    //for third field
    Projection** projections;
    int num_of_projections;

    int num_of_processed_relations;
    //is_processed[0] == 0 means that query's relation with relative index 0
    //hasn't been processed yet in this query
    int* is_processed;
    int* order;
    int order_index; //order_index is essentially tuple's size
public:
    Query();
    ~Query();

    //getters
    Relation** get_relations(){ return this->relations;}
    int get_num_of_relations(){ return this->num_of_relations;}
    Predicate** get_predicates(){ return this->predicates;}
    int get_num_of_predicates(){ return this->num_of_predicates;}
    Projection** get_projections(){ return this->projections;}
    int get_num_of_projections(){ return this->num_of_projections;}

    int* get_order(){ return this->order;}
    int get_order_index(){ return this->order_index;}
    int get_tuple_size(){ return this->order_index;}//same function as the above, used for easier read of code
    void incr_order_index(){ this->order_index++;}
    int find_offset(int relation_id){
        for(int i = 0; i < order_index; i++){
            if(order[i] == relation_id)
                return i;
        }
        return -1; //return -1 if it is not found
    }

    //setters
    void set_num_of_relations(int num){ this->num_of_relations = num;}
    void set_num_of_predicates(int num){ this->num_of_predicates = num;}
    void set_num_of_projections(int num){ this->num_of_projections = num;}


    int read_query(Relation** db_relations, int db_num_of_relations);
};

#endif /* end of include guard: QUERY_H */
