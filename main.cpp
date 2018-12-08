#include <iostream>
#include <cstring>
#include "DatabaseSystem.h"

int main(int argc, char const *argv[]) {
    DatabaseSystem* db = new DatabaseSystem();
    db->load_relations();
    db->construct_query();
    db->execute_query();
    delete db;
    return 0;
}
