#include <iostream>
#include <cstring>
#include "DatabaseSystem.h"

int main(int argc, char const *argv[]) {
    DatabaseSystem* xd = new DatabaseSystem();
    xd->load_relations();
    xd->construct_query();
    xd->execute_query();
    delete xd;
    return 0;
}
