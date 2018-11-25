#include "Relation.h"
#include "Query.h"
#include "ResultList.h"

class DatabaseSystem{
    int num_of_relations;
    Relation** relations;

    ResultList** result_lists;

    //holds the current query given
    Query* query;
public:
    DatabaseSystem();
    ~DatabaseSystem();

    /*
    reads(from stdin) the names of the binary files that contain each relation
    and loads relations to heap dynamically
    */
    int load_relations();
    /*
    constructs query  given (from stdin) through query's read_query() function
    */
    int construct_query();
    /*
    executes query
    */
    int execute_query();

    /*
    3 categories of predicates
    o : original
    n : not original (take 'column' from temp results)
    */
    int join(Predicate predicate);
        int join_oo(Predicate predicate);
        int join_nn(Predicate predicate);
        int join_no(Predicate predicate);
    int self_join(Predicate* predicate);
        int self_join_o(Predicate* predicate);
        int self_join_n(Predicate* predicate);
    int filter(Predicate* predicate);
        int filter_o(Predicate* predicate);
        int filter_n(Predicate* predicate);
        static inline int equal(uint64_t num1, uint64_t num2){ return num1 == num2;}
        static inline int greater_than(uint64_t num1, uint64_t num2){ return num1 > num2;}
        static inline int less_than(uint64_t num1, uint64_t num2){ return num1 < num2;}

};
