#include <iostream>
#include <cstring>
#include "DatabaseSystem.h"

int main(int argc, char const *argv[]) {
    // char* file_name = (char*)malloc(128);
    //
    // strcpy(file_name, "./small/r0");
    // Relation* r0 = new Relation(file_name);
    // strcpy(file_name, "./small/r1");
    // Relation* r1 = new Relation(file_name);
    // strcpy(file_name, "./small/r2");
    // Relation* r2 = new Relation(file_name);
    // strcpy(file_name, "./small/r3");
    // Relation* r3 = new Relation(file_name);
    //
    // //read predicate
    // char* query = (char*)malloc(256);
    // char* first = (char*)malloc(128);
    // char* second =(char*)malloc(128);
    // char* third=(char*)malloc(128);
    // cin.getline(query, 256);
    // strcpy(first, strtok(query,"|"));
    // strcpy(second, strtok(NULL,"|"));
    // strcpy(third, strtok(NULL,"|"));
    // cout << first << endl;
    // cout << second << endl;
    // cout << third << endl;
    //
    //
    // free(file_name);
    // delete r0, r1, r2, r3;
    DatabaseSystem* xd = new DatabaseSystem();
    xd->load_relations();
    xd->construct_query();
    delete xd;
    return 0;
}
