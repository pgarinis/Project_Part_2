#include "Joiner.h"

using namespace std;

Joiner::Joiner(Query* query, Predicate* predicate, int* is_processed, vector<uint64_t>** result_buffer,int type):
query(query), predicate(predicate),
is_processed(is_processed), result_buffer(result_buffer), join_type(type){
    //sysconf(_SC_LEVEL1_DCACHE_LINESIZE) get l1 cache size
    h1_num_of_buckets = 512;
    h1_num_of_bits = (int)log2(h1_num_of_buckets);
    h2_num_of_buckets = 16699;
    h2_num_of_bits = (int)log2(h2_num_of_buckets);
}

int Joiner::segmentation(){
    create_and_compute_hist_array();
    create_and_compute_psum_array();
    create_and_compute_new_column();
    cout << "Both relations segmentated successfully!" << endl;
    return 0;
}

int Joiner::create_and_compute_hist_array(){
    //to make code more readable
    int hash_value;
    uint64_t* column1 = query->get_relations()[predicate->relation1]->get_column(predicate->column1);
    uint64_t* column2 = query->get_relations()[predicate->relation2]->get_column(predicate->column2);

    //allocate relation's hist_array and initialise
    hist_array[0] = (uint64_t*)calloc(h1_num_of_buckets, sizeof(uint64_t));
    hist_array[1] = (uint64_t*)calloc(h1_num_of_buckets, sizeof(uint64_t));

    //temporal sets
    unordered_set<uint64_t>* temp_set_1 = new unordered_set<uint64_t>();
    unordered_set<uint64_t>* temp_set_2 = new unordered_set<uint64_t>();

    //compute hist_array
    if(join_type == 0){
        //pick iterator
        vector<uint64_t>::iterator it = (*result_buffer)->begin();

        //compute hist_array for both relations
        while( it != (*result_buffer)->end()) {
            //hist_array for relation 1
            if(temp_set_1->find(*(it + predicate->relation1)) != temp_set_1->end()){
                temp_set_1->insert(*(it + predicate->relation1));
                hash_value = h1(column1[*(it + predicate->relation1)]); //iterator has rowId, hash the value of that rowId
                hist_array[0][hash_value]++;
            }


            if(temp_set_2->find(*(it + predicate->relation2)) != temp_set_2->end()){
                temp_set_2->insert(*(it + predicate->relation2));
                //hist_array for relation 2
                hash_value = h1(column2[*(it + predicate->relation2)]); //iterator has rowId, hash the value of that rowId
                hist_array[1][hash_value]++;
            }

            //step
            it += query->get_num_of_processed_relations(); //move on
        }

    }
    else if(join_type == 1){

    }
    else if(join_type == 2){
        //to make code more readable
        int hash_value;

        //compute hist_array
        for(int j = 0, limit = query->get_relations()[predicate->relation1]->get_num_of_records(); j < limit; j++){
            hash_value = h1(column1[j]);
            hist_array[0][hash_value]++;
        }

        for(int j = 0, limit = query->get_relations()[predicate->relation2]->get_num_of_records(); j < limit; j++){
            hash_value = h1(column2[j]);
            hist_array[1][hash_value]++;
        }
    }
    return 0;
}

int Joiner::create_and_compute_psum_array(){
    //allocate psum_array
    psum_array[0] = (uint64_t*)malloc(h1_num_of_buckets * sizeof(uint64_t));
    psum_array[1] = (uint64_t*)malloc(h1_num_of_buckets * sizeof(uint64_t));

    //calculate psum_array for both relations
    psum_array[0][0] = 0;
    psum_array[1][0] = 0;
    for(int j = 1; j < h1_num_of_buckets; j++){
        psum_array[0][j] = psum_array[0][j-1] + hist_array[0][j-1];
        psum_array[1][j] = psum_array[1][j-1] + hist_array[1][j-1];
    }
    return 0;
}

int Joiner::create_and_compute_new_column(){
    //to make code more readable
    int hash_value;

    uint64_t* column1 = query->get_relations()[predicate->relation1]->get_column(predicate->column1);
    uint64_t* column2 = query->get_relations()[predicate->relation2]->get_column(predicate->column2);

    //temporal array needed to calculate new_columnentry. this array is copy of psum array initially
    int copy_of_psum_array0[h1_num_of_buckets];
    for(int j = 0; j < h1_num_of_buckets; j++)
        copy_of_psum_array0[j] = psum_array[0][j];

    int copy_of_psum_array1[h1_num_of_buckets];
    for(int j = 0; j < h1_num_of_buckets; j++)
        copy_of_psum_array1[j] = psum_array[1][j];


    if(join_type == 0){
        //create vectors
        new2_column[0] = new vector<NewColumnEntry2>();
        new2_column[1] = new vector<NewColumnEntry2>();

        //it1 is for result buffer
        vector<uint64_t>::iterator it1 = (*result_buffer)->begin();
        int hash_value;
        //it2 is for new column
        vector<NewColumnEntry2>::iterator it2;

        while(it1 != (*result_buffer)->end()){
            hash_value = h1(column1[*(it1 + predicate->relation1)]);
            //determine the index in the new column
            it2 = new2_column[0]->begin() + copy_of_psum_array0[hash_value];
            //insert rowId,value in new column
            new2_column[0]->insert(it2,NewColumnEntry2(*(it1 + predicate->relation1), column1[*(it1 + predicate->relation1)]));
            //add related rowId to new column entry set
            it2->add_to_set(*(it1 + predicate->relation2));
            copy_of_psum_array0[hash_value]++;

            hash_value = h1(column2[*(it1 + predicate->relation2)]);
            it2 = new2_column[1]->begin() + copy_of_psum_array1[hash_value];
            new2_column[1]->insert(it2,NewColumnEntry2(*(it1 + predicate->relation2), column2[*(it1 + predicate->relation2)]));
            it2->add_to_set(*(it1 + predicate->relation1));
            copy_of_psum_array1[hash_value]++;

            it1 += query->get_num_of_processed_relations();
        }
    }
    else if(join_type == 2){
        uint64_t num_records1 = query->get_relations()[predicate->relation1]->get_num_of_records();
        uint64_t num_records2 = query->get_relations()[predicate->relation2]->get_num_of_records();
        new_column[0] = malloc(num_records1 * sizeof(NewColumnEntry));
        new_column[1] = malloc(num_records2 * sizeof(NewColumnEntry));

        //calculate new_column. new column is an array of tuples (uint64_t index, uint64_t value)
        for(int j = 0; j < num_records1; j++){
            hash_value = h1(column1[j]);
            new_column[0][copy_of_psum_array0[hash_value]].set(j, new_column[0][j]);
            copy_of_psum_array0[hash_value]++;
        }

        for(int j = 0; j < num_records2; j++){
            hash_value = h1(column2[j]);
            new_column[1][copy_of_psum_array1[hash_value]].set(j, new_column[1][j]);
            copy_of_psum_array1[hash_value]++;
        }
    }
    return 0;
}


int Joiner::indexing(){
    if(join_type == 0){
        //choose the smallest relation to index
        vector<NewColumnEntry2>* column = NULL;
        uint64_t* cur_hist_array = NULL;
        uint64_t* cur_psum_array = NULL;
        if(new2_column[0]->size() < new2_column[1]->size()){
            column = new2_column[0];
            cur_hist_array = hist_array[0];
            cur_psum_array = psum_array[0];
        }
        else{
            column = new2_column[1];
            cur_hist_array = hist_array[1];
            cur_psum_array = psum_array[1];
        }


        //create index for every bucket made at segmentation
        index_array = new Index[h1_num_of_buckets];

        //to make code more readable
        int hash_value;

        //for every bucket made at segmentation
        for(int j = 0; j < h1_num_of_buckets; j++){
            //store current index to a variable to make code more readable
            create_and_init_chain_and_bucket_array(&index_array[j], cur_hist_array[j]);
            //scan whole bucket in order to calculate its chain and bucket array
            int upper_limit = cur_psum_array[j] + cur_hist_array[j];
            for(int i = cur_psum_array[j]; i < upper_limit; i++){
                vector<NewColumnEntry2>::iterator it = column->begin() + i;
                hash_value = h2(it->get_value());
                index_array[j].get_chain_array()[i - cur_psum_array[j]] = index_array[j].get_bucket_array()[hash_value];
                index_array[j].get_bucket_array()[hash_value] = i - cur_psum_array[j];
            }
        }
    }
    else if(join_type == 2){
        //choose the smallest relation to index
        Relation* relation = NULL;
        if(relations[0]->get_num_of_records() < relations[1]->get_num_of_records())
           relation = relations[0];
        else
           relation = relations[1];

        //create index for every bucket made at segmentation
        relation->set_index_array(h1_num_of_buckets);

        //to make code more readable
        int hash_value;

        //for every bucket made at segmentation
        for(int j = 0; j < h1_num_of_buckets; j++){
           //store current index to a variable to make code more readable
           Index& cur_index = relation->get_index_array()[j];
           create_and_init_chain_and_bucket_array(cur_index, relation->get_hist_array()[j]);
           //scan whole bucket in order to calculate its chain and bucket array
           int upper_limit = relation->get_psum_array()[j] + relation->get_hist_array()[j];
           for(int i = relation->get_psum_array()[j]; i < upper_limit; i++){
               hash_value = h2(relation->get_new_column()[i].get_value());
               cur_index.get_chain_array()[i - relation->get_psum_array()[j]] = cur_index.get_bucket_array()[hash_value];
               cur_index.get_bucket_array()[hash_value] = i - relation->get_psum_array()[j];
           }
        }
    }
    cout << "Indexing completed successfully!" << endl;
    return 0;
}

int Joiner::create_and_init_chain_and_bucket_array(Index* index, uint64_t hist_array_value){
    //create bucket array and initialise it
    index->set_bucket_array(h2_num_of_buckets);
    for(int i = 0; i < h2_num_of_buckets; i++)
        index->get_bucket_array()[i] = -1; //-1 means that there is not previous record in the bucket

    //create chain array and initialise it
    index->set_chain_array(hist_array_value);
    for(int i = 0; i < hist_array_value; i++)
        index->get_chain_array()[i] = 0;

    return 0;
}
//
// int Joiner::join(){
//     //r0 --> NOT Indexed relation
//     Relation* r0 = NULL;
//     //r1 --> Indexed relation
//     Relation* r1 = NULL;
//
//     if(relations[0]->get_index_array() != NULL){
//         r1 = relations[0];
//         r0 = relations[1];
//     }
//     else{
//         r1 = relations[1];
//         r0 = relations[0];
//     }
//
//
//     int counter = 0;
//     //for every row in r0
//     for(int i = 0; i < r0->get_num_of_records(); i++){
//         //for easier reading of code
//         NewColumnEntry cur_row = r0->get_new_column()[i];
//
//         //take the bucket needed
//         int bucket_num = h1(cur_row.get_value());
//
//         //search index for this record
//         int index = r1->get_index_array()[bucket_num].get_bucket_array()[h2(cur_row.get_value())];
//
//         //cout << "Value : " << cur_row.get_value() << endl;
//         while(index != -1){
//             //cout << r1.new_column[index].value << " vs "  << cur_row.value << endl;
//             if(r1->get_new_column()[index + r1->get_psum_array()[bucket_num]].get_value() == cur_row.get_value()){
//                 //tuple [cur_row.get_index() + 1, r1->get_new_column()[index + r1->get_psum_array()[bucket_num]].get_index() + 1]
//                 //cout <<"[" << cur_row.get_index() + 1 << " : "<<r1->get_new_column()[index + r1->get_psum_array()[bucket_num]].get_index() + 1 <<"]"<< endl;
//                 uint64_t row1 = cur_row.get_index() + 1;
//                 uint64_t row2 = r1->get_new_column()[index + r1->get_psum_array()[bucket_num]].get_index() + 1;
//                 //printf("%lu == %lu\n",row1,row2);
//                 this->result->InsertData(row1, row2);
//             }
//             index = r1->get_index_array()[bucket_num].get_chain_array()[index];
//         }
//         //cout << " -----------------------------" << endl;
//     }
//     return 0;
// }
//
// Joiner::~Joiner(){
//     Index* index;
//     if(index_array != NULL)
//         index = relations[0]->get_index_array();
//     else
//         index = relations[1]->get_index_array();
//
//     if(relations[0] != NULL)
//       delete(relations[0]);
//     if(relations[1] != NULL)
//       delete(relations[1]);
//     if(result != NULL)
//       delete(result);
// }
