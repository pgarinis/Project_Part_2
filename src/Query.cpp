#include <iostream>
#include <fstream>
#include <stdint.h>
#include <cstdlib>
#include <cstring>
#include "../include/Query.h"
#define BUFF_SIZE 1024

Query::Query():
relations(NULL), num_of_relations(0),
predicates(NULL), num_of_predicates(0),
projections(NULL), num_of_projections(0), order(NULL)
{
}

Query::~Query(){
    if(relations != NULL)
        free(relations);
    for(int i = 0; i < num_of_predicates; i++)
        free(predicates[i]);
    if(predicates != NULL)
        free(predicates);
    for(int i = 0; i < num_of_projections; i++)
        free(projections[i]);
    if(projections != NULL)
        free(projections);
    if(order != NULL)
        free(order);
}

int Query::read_query(Relation** db_relations, int db_num_of_relations){
    //buffer to store the whole query
    char query[BUFF_SIZE];
    char* first;
    char* second;
    char* third;
    char* token;

    //read query from stdin
    if(cin.fail())
        cout << "cin Error" << endl;

    cin.getline(query, BUFF_SIZE);
    //cout << "Query : " << query << endl;
    //cout << "--------------------------------------------------------------------" << endl;

    if(cin.eof())
        return 2;

    if(!strcmp(query,"F")){
        //cout << "F read" << endl;
        return 1;
    }

    //0 2 4|0.1=1.2&1.0=2.1&0.1>3000|0.0 1.1
    first = strtok(query, "|");
    second = strtok(NULL, "|");
    third = strtok(NULL, "\n");
    //cout << first << endl;
    //cout << second << endl;
    //cout << third << endl;

    //FIRST FIELD
    token = strtok(first, " ");
    while(token != NULL){
        relations = (Relation**)realloc(relations, (++num_of_relations) * sizeof(Relation*));
        //cout << atoi(token) << endl;
        if(atoi(token) < 0 || atoi(token) > db_num_of_relations)
            cout << "WRONG INDEX" << endl;
        relations[num_of_relations - 1] = db_relations[atoi(token)];
        token = strtok(NULL, " ");
    }
    //initiliase order array; every element is initially -1
    order = (int*)malloc(num_of_relations * sizeof(int));
    for(int i = 0; i < num_of_relations; i++)
        order[i] = -1;
    order_index = 0;

    //0.1=1.2&1.0=2.1&0.1>3000
    //SECOND FIELD
    char buffer[BUFF_SIZE];
    char temp_buffer[BUFF_SIZE];
    strcpy(buffer, strtok(second, "&"));
    while(1){
        predicates = (Predicate**)realloc(predicates, (++num_of_predicates) * sizeof(Predicate*));
        predicates[num_of_predicates - 1] = (Predicate*)malloc(sizeof(Predicate));

        //cout << buffer << endl;
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
        //cout << temp_buffer << endl;

        for(int j = 0, limit = strlen(buffer); i < limit; i++,j++){
            if(buffer[i] == predicates[num_of_predicates -1]->op){
                temp_buffer[j] = '\0';
                i++;
                break;
            }
            temp_buffer[j] = buffer[i];
        }
        predicates[num_of_predicates - 1]->column1 = atoi(temp_buffer);
        //cout << temp_buffer << endl;

        //psaxnw an exei '.'
        for(int j = 0, limit = strlen(buffer); i < limit; i++,j++){
            if(buffer[i] == '.'){
                temp_buffer[j] = '\0';
                i++;
                break;
            }
            temp_buffer[j] = buffer[i];
        }
        if((unsigned int)i == strlen(buffer)){//i==limit shmenei pos einai stathera
            //temp_buffer[i] == '\0';
            predicates[num_of_predicates - 1]->value = atoi(temp_buffer);
            predicates[num_of_predicates - 1]->relation2 = -1;
            //cout <<  predicates[num_of_predicates - 1]->value << endl;
        }
        else{
            predicates[num_of_predicates - 1]->relation2 = atoi(temp_buffer);
            //cout << temp_buffer << endl;
            strcpy(temp_buffer, buffer + i);
            predicates[num_of_predicates - 1]->column2 = atoi(temp_buffer);
            //cout << temp_buffer << endl;
        }
        token = strtok(NULL, "&");
        if(token != NULL)
            strcpy(buffer, token);
        else
            break;

    }

    //THIRD FIELD
    token = strtok(third, " .");
    num_of_projections = 0;
    while(token != NULL){
        projections = (Projection**)realloc(projections, (++num_of_projections) * sizeof(Projection*));
        projections[num_of_projections - 1] = (Projection*)malloc(sizeof(Projection));

        //set new projection
        projections[num_of_projections - 1]->relation_index = atoi(token);
        projections[num_of_projections - 1]->column_index = atoi(strtok(NULL, " ."));
        token = strtok(NULL, " .\n");
    }

    return 0;

    //cout << "there are "<< num_of_relations << " relations in this query."<<endl;
    //cout << "there are "<< num_of_predicates << " predicates in this query" <<endl;
    //cout << "there are "<< num_of_projections << " projections in this query"<< endl;

}
