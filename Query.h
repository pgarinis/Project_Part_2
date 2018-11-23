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

    //setters
    void set_num_of_relations(int num){ this->num_of_relations = num;}
    void set_num_of_predicates(int num){ this->num_of_predicates = num;}
    void set_num_of_projections(int num){ this->num_of_projections = num;}




    int read_query(Relation** db_relations, int db_num_of_relations);
};
