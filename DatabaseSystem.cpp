#include "DatabaseSystem.h"
#define BUFF_SIZE 256

DatabaseSystem::DatabaseSystem():
num_of_relations(0), relations(NULL)
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

int DatabaseSystem::read_query(){
    //buffer to store the whole query
    char query[BUFF_SIZE];
    char* first;
    char* second;
    char* third;
    char* token;

    //read query from stdin
    cin.getline(query, BUFF_SIZE);

    //0 2 4|0.1=1.2&1.0=2.1&0.1>3000|0.0 1.1
    first = strtok(query, "|");
    second = strtok(NULL, "|");
    third = strtok(NULL, "\n");
    cout << first << endl;
    cout << second << endl;
    cout << third << endl;

    //FIRST FIELD
    token = strtok(first, " ");
    num_of_query_relations = 0;
    while(token != NULL){
        query_relations = (Relation**)realloc(query_relations, (++num_of_query_relations) * sizeof(Relation*));
        cout << atoi(token) << endl;
        query_relations[num_of_query_relations - 1] = relations[atoi(token)];
        token = strtok(NULL, " ");
    }

    //0.1=1.2&1.0=2.1&0.1>3000
    //SECOND FIELD
    token = strtok(second, "&.");
    num_of_predicates = 0;
    while(token != NULL){
        predicates = (Predicate**)realloc(predicates, (++num_of_predicates) * sizeof(Predicate*));
        predicates[num_of_predicates - 1] = (Predicate*)malloc(sizeof(Predicate));

        predicates[num_of_predicates - 1]->relation2 = -1;
        predicates->relation1 = atoi(strtok(token, "."));


    }

    //THIRD FIELD
    token = strtok(third, " .");
    num_of_projections = 0;
    while(token != NULL){
        projections = (Projection**)realloc(projections, (++num_of_projections) * sizeof(Projection*));
        projections[num_of_projections - 1] = (Projection*)malloc(sizeof(Projection));

        //set new projection
        projections[num_of_projections - 1]->relation_index = atoi(token);
        projections[num_of_projections - 1]->column_num = atoi(strtok(NULL, " ."));
        token = strtok(NULL, " .\n");
    }






}
