#include <iostream>
#include "../include/DatabaseSystem.h"

int main(int argc, char const *argv[]) {
    DatabaseSystem* db = new DatabaseSystem();
    db->load_relations();
    db->handle_load();
    delete db;
    return 0;
}
