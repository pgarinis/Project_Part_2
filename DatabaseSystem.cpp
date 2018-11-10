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
    char buffer[BUFF_SIZE];
    char temp_buffer[BUFF_SIZE];
    strcpy(buffer, strtok(second, "&"));
    num_of_predicates = 0;
    while(1){
        predicates = (Predicate**)realloc(predicates, (++num_of_predicates) * sizeof(Predicate*));
        predicates[num_of_predicates - 1] = (Predicate*)malloc(sizeof(Predicate));

        cout << buffer << endl;
        for(int i = 0, limit = strlen(buffer); i < limit; i++)
            if(buffer[i] == '=' || buffer[i] == '>' || buffer[i] == '<'){
                predicates[num_of_predicates - 1]->op = buffer[i];
                break;
            }
        int i = 0;
        for(int limit = strlen(buffer); i < limit; i++){
            if(buffer[i] == '.'){
                temp_buffer[i] = '\0';
                i++;
                break;
            }
            temp_buffer[i] = buffer[i];
        }
        predicates[num_of_predicates - 1]->relation1 = atoi(temp_buffer);
        cout << temp_buffer << endl;

        for(int j = 0, limit = strlen(buffer); i < limit; i++,j++){
            if(buffer[i] == predicates[num_of_predicates -1]->op){
                temp_buffer[j] = '\0';
                i++;
                break;
            }
            temp_buffer[j] = buffer[i];
        }
        predicates[num_of_predicates - 1]->column1 = atoi(temp_buffer);
        cout << temp_buffer << endl;

        //psaxnw an exei '.'
        for(int j = 0, limit = strlen(buffer); i < limit; i++,j++){
            if(buffer[i] == '.'){
                temp_buffer[j] = '\0';
                i++;
                break;
            }
            temp_buffer[j] = buffer[i];
        }
        if(i == strlen(buffer)){//i==limit shmenei pos einai stathera
            temp_buffer[i] == '\0';
            predicates[num_of_predicates - 1]->value = atoi(temp_buffer);
            predicates[num_of_predicates - 1]->relation2 = -1;
            cout << temp_buffer << endl;
        }
        else{
            predicates[num_of_predicates - 1]->relation2 = atoi(temp_buffer);
            cout << temp_buffer << endl;
            strcpy(temp_buffer, buffer + i);
            predicates[num_of_predicates - 1]->column2 = atoi(temp_buffer);
            cout << temp_buffer << endl;
        }
        token = strtok(NULL, "&");
        if(token != NULL)
            strcpy(buffer, token);
        else
            break;


        // predicates[num_of_predicates - 1]->relation2 = -1;
        // predicates->relation1 = atoi(token);
        // predicates->column1 = atoi(strtok(NULL, "=><"));



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
