#include "../include/Joiner.h"
#include "../include/JobScheduler.h"
#include <unordered_map>
#include "unistd.h"

using namespace std;

#include <time.h>
#include <sys/time.h>
double get_wall_time(){
    struct timeval time;
    if (gettimeofday(&time,NULL)){
        //  Handle error
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

Joiner::Joiner(vector<uint64_t>** result_buffer):
    result_buffer(result_buffer)
{
    //sysconf(_SC_LEVEL1_DCACHE_LINESIZE) get l1 cache size
    h1_num_of_buckets = 128;
    h1_num_of_bits = (int)log2(h1_num_of_buckets);
    h2_num_of_buckets = 16699;
    h2_num_of_bits = (int)log2(h2_num_of_buckets);
    new_column[0] = NULL;
    new_column[1] = NULL;
    index_array = NULL;
    temp_set = NULL;
    job_scheduler = new JobScheduler(4,this);

}

Joiner::~Joiner(){
}

int Joiner::handle_predicate(Query* query, Predicate* predicate){
    //set joiner's variables
    this->query = query;
    this->predicate = predicate;

    //set join_type accordingly
    //if only one relation is processed already
    if((query->find_offset(predicate->relation1) != -1) ||
       (query->find_offset(predicate->relation2) != -1)){
           join_type = 0;
           //cout << "Join type 0 is handled...";
    }
    //if neiter of the 2 relations are processed (if first predicate handled is join)
    else if((*result_buffer)->size() == 0){
        //cout << "Join type 2 is handled...";
        join_type = 2;
    }

    //put processed relation at first place and unprocessed at second place (if needed)
    if(join_type == 0){
        if(query->find_offset(predicate->relation1) == -1){
            int temp;
            //switch relations
            temp = predicate->relation1;
            predicate->relation1 = predicate->relation2;
            predicate->relation2 = temp;
            //switch columns
            temp = predicate->column1;
            predicate->column1 = predicate->column2;
            predicate->column2 = temp;
        }
    }

    //get the columns the join functions will work with
    column[0] = query->get_relations()[predicate->relation1]->get_column(predicate->column1);
    column[1] = query->get_relations()[predicate->relation2]->get_column(predicate->column2);

    //precedures to complete a join predicate
    //double t0 = get_wall_time();
    segmentation();
    //std::cerr << "Segmentation finished in: " << get_wall_time() - t0 << endl;
    //t0 = get_wall_time();
    indexing();
    //std::cerr << "Indexing finished in: " << get_wall_time() - t0 << endl;
    if(join_type == 0){
        //cerr << "join type 0" << endl;
        //t0 = get_wall_time();
        join();
        //std::cerr << "Join type 0 finished in: " << get_wall_time() - t0 << endl;
    }
    else{
        //cerr << "join type 2" << endl;
        //t0 = get_wall_time();
        job_scheduler->handle_join();
        //std::cerr << "Join type 2 (threads) finished in: " << get_wall_time() - t0 << endl;
    }

    if(new_column[0] != NULL)
        free(new_column[0]);
    if(new_column[1] != NULL)
        free(new_column[1]);
    if(index_array != NULL)
        delete[] index_array;
    if(hist_array[0] != NULL)
        free(hist_array[0]);
    if(hist_array[1] != NULL)
        free(hist_array[1]);
    if(psum_array[0] != NULL)
        free(psum_array[0]);
    if(psum_array[1] != NULL)
        free(psum_array[1]);
    if(temp_set != NULL){
        delete temp_set;
        temp_set = NULL;
    }

    return 0;
}

int Joiner::segmentation(){
    if(join_type == 2)
        job_scheduler->handle_segmentation();
    else{
        create_and_compute_hist_array();
        create_and_compute_psum_array();
        create_and_compute_new_column();
    }

    //cout << "Both relations segmentated successfully!" << endl;
    return 0;
}

int Joiner::create_and_compute_hist_array(){
    //to make code more readable
    int hash_value;

    //allocate both hist arrays and initialise them TODO:free
    hist_array[0] = (uint64_t*)calloc(h1_num_of_buckets, sizeof(uint64_t));
    hist_array[1] = (uint64_t*)calloc(h1_num_of_buckets, sizeof(uint64_t));

    //computing hist_array of relation1 (different calculation for each join_type)
    if(join_type == 0){
        //compute hist array for relation1 (which is already processed)
        //find relation1's offset in the result tuple
        int offset = query->find_offset(predicate->relation1);
        int tuple_size = query->get_tuple_size();

        //temporal set is needed to know the size of the relation that already exists in results TODO:free
        temp_set = new unordered_set<uint64_t>();

        //search whole result buffer and compute hist_array for relation1
        vector<uint64_t>::iterator it = (*result_buffer)->begin();
        vector<uint64_t>::iterator it0; //make code more efficient
        vector<uint64_t>::iterator it_end = (*result_buffer)->end();
        while( it != it_end){
            it0 = it + offset;
            //if it already exists in set, don't do anything
            if(temp_set->insert(*it0).second){
                hash_value = h1(column[0][*it0]);
                hist_array[0][hash_value]++;
            }

            //move to next tuple
            it += tuple_size;
        }
    }
    else if(join_type == 2){
        //to make code more readable
        int hash_value;

        //compute hist_array for relation1
        uint64_t num_of_rows_0 = query->get_relations()[predicate->relation1]->get_num_of_records();
        for(uint64_t j = 0; j < num_of_rows_0; j++){
            hash_value = h1(column[0][j]);
            hist_array[0][hash_value]++;
        }
    }

    //compute hist_array for relation2
    uint64_t num_of_rows =  query->get_relations()[predicate->relation2]->get_num_of_records();
    for(uint64_t j = 0; j < num_of_rows; j++){
        hash_value = h1(column[1][j]);
        hist_array[1][hash_value]++;
    }

    return 0;
}

int Joiner::create_and_compute_psum_array(){
    //allocate psum_arrays (for both relations) //TODO:free
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

    //temporal arrays needed to calculate new_column
    //these arrays are the copys of their respective psum_array
    uint64_t copy_of_psum_array[2][h1_num_of_buckets];
    for(int j = 0; j < h1_num_of_buckets; j++){
        copy_of_psum_array[0][j] = psum_array[0][j];
        copy_of_psum_array[1][j] = psum_array[1][j];
    }

    if(join_type == 0){
        //calculate new column for the processed relation(relation1)
        new_column[0] = (NewColumnEntry*)malloc(sizeof(NewColumnEntry) * temp_set->size());
        unordered_set<uint64_t>::iterator end = temp_set->end();
        for (unordered_set<uint64_t>::iterator it0 = temp_set->begin(); it0 != end; it0++) {
            hash_value = h1(column[0][*it0]);
            new_column[0][copy_of_psum_array[0][hash_value]].set(*it0 ,column[0][*it0]);
            copy_of_psum_array[0][hash_value]++;
        }
    }
    else if(join_type == 2){
        //calculate new_column for relation1
        uint64_t num_records1 = query->get_relations()[predicate->relation1]->get_num_of_records();
        new_column[0] = (NewColumnEntry*)malloc(num_records1 * sizeof(NewColumnEntry));
        for(uint64_t j = 0; j < num_records1; j++){
            hash_value = h1(column[0][j]);
            new_column[0][copy_of_psum_array[0][hash_value]].set(j, column[0][j]);
            copy_of_psum_array[0][hash_value]++;
        }
    }

    //calculate new column for relation2
    uint64_t num_of_records = query->get_relations()[predicate->relation2]->get_num_of_records();
    new_column[1] = (NewColumnEntry*)malloc(num_of_records * sizeof(NewColumnEntry));
    for(uint64_t j = 0; j < num_of_records; j++){
        hash_value = h1(column[1][j]);
        new_column[1][copy_of_psum_array[1][hash_value]].set(j, column[1][j]);
        copy_of_psum_array[1][hash_value]++;
    }

    return 0;
}

int Joiner::indexing(){
    //decide join index (relation1 or relation2)
    if(join_type == 0){
        //index the smallest new column (for effieciency reasons)
        uint64_t num_of_records = query->get_relations()[predicate->relation2]->get_num_of_records();
        join_index = num_of_records < temp_set->size();
    }
    else if(join_type == 2){
        //result buffer is null, both relations original
        //choose the smallest relation to index
        join_index =
            query->get_relations()[predicate->relation2]->get_num_of_records()
            <
            query->get_relations()[predicate->relation1]->get_num_of_records();

        //set order[] from query accordingly(push order : undindexed -> indexed)
        if(join_index == 1){
            query->get_order()[query->get_order_index()] = predicate->relation1;
            query->incr_order_index();
            query->get_order()[query->get_order_index()] = predicate->relation2;
        }
        else if(join_index == 0){
            query->get_order()[query->get_order_index()] = predicate->relation2;
            query->incr_order_index();
            query->get_order()[query->get_order_index()] = predicate->relation1;
        }
        query->incr_order_index();
    }
    // cout << "Join index is : "<<join_index << endl;

    //set variables accordingly to chosen join_index
    NewColumnEntry* column =  new_column[join_index];
    uint64_t* cur_hist_array = hist_array[join_index];
    uint64_t* cur_psum_array = psum_array[join_index];

    //create index for every bucket made at segmentation
    index_array = new Index[h1_num_of_buckets];

    //to make code more readable
    int hash_value;

    //for every bucket made at segmentation
    for(int j = 0; j < h1_num_of_buckets; j++){
        create_and_init_chain_and_bucket_array(&index_array[j], cur_hist_array[j]);
        //scan whole bucket in order to calculate its chain and bucket array
        int upper_limit = cur_psum_array[j] + cur_hist_array[j];
        for(int i = cur_psum_array[j]; i < upper_limit; i++){
            hash_value = h2(column[i].get_value());
            index_array[j].get_chain_array()[i - cur_psum_array[j]] = index_array[j].get_bucket_array()[hash_value];
            index_array[j].get_bucket_array()[hash_value] = i - cur_psum_array[j];
        }
    }

    //cout << "Indexing completed successfully!" << endl;
    return 0;
}

int Joiner::create_and_init_chain_and_bucket_array(Index* index, uint64_t hist_array_value){
    //create bucket array and initialise it
    index->set_bucket_array(h2_num_of_buckets);
    for(int i = 0; i < h2_num_of_buckets; i++)
        index->get_bucket_array()[i] = -1; //-1 means that there is not previous record in the bucket

    //create chain array and initialise it
    index->set_chain_array(hist_array_value);
    for(uint64_t i = 0; i < hist_array_value; i++)
        index->get_chain_array()[i] = 0;

    return 0;
}

int Joiner::join(){
    vector<uint64_t>* new_vector = new vector<uint64_t>();
    new_vector->reserve(8192);
    //int temp_set_init_size = temp_set->size();
    if(join_type == 0){
        NewColumnEntry* unindexed_relation = new_column[!join_index];
        //indexed_relation --> Indexed relation
        NewColumnEntry* indexed_relation = new_column[join_index];

        //using this array of vectors to prevent multiple same calculations TODO:free
        //every needed calculation wll be done only 1 time
        //double t0 = get_wall_time();
        vector<uint64_t>* array_of_vectors[temp_set->size()];
        for(uint64_t i = 0; i < temp_set->size(); i++)
            array_of_vectors[i] = new vector<uint64_t>();

        //create a mapping (from row_id to its respective vector from array_of_vectors)
        unordered_map<uint64_t, vector<uint64_t>*> map;



        //if unprocessed relation is indexed
        if(join_index == 1){
            //in this loop array of vectors is calculated
            //scan already processed column
            for(uint64_t i = 0; i < temp_set->size(); i++){
                //for easier reading of code
                NewColumnEntry cur_row = unindexed_relation[i];

                uint64_t val = cur_row.get_value();

                //take the bucket needed
                int bucket_num = h1(val);

                //search index for this record
                int index = index_array[bucket_num].get_bucket_array()[h2(val)];

                //traverse chain array and push qualified row_ids to respective vector
                while(index != -1){
                    if(indexed_relation[index + psum_array[join_index][bucket_num]].get_value() == val)
                        array_of_vectors[i]->push_back(indexed_relation[index + psum_array[join_index][bucket_num]].get_row_id());
                    index = index_array[bucket_num].get_chain_array()[index];
                }



                //if there was no match, delete vector
                if(array_of_vectors[i]->size() == 0){
                    delete array_of_vectors[i];
                    array_of_vectors[i] = NULL;
                    map[cur_row.get_row_id()] = NULL;
                }
                else//create respective mapping
                    map[cur_row.get_row_id()] = array_of_vectors[i];

            }
            //std::cerr << "join index 1 main loop finished in: " << get_wall_time() - t0 << endl;
        }
        //if processed relation is indexed
        else if (join_index == 0){
            //do the mapping
            for(uint64_t i = 0; i < temp_set->size(); i++)
                map[indexed_relation[i].get_row_id()] = array_of_vectors[i];



            //scanning UNINDEXED relation
            uint64_t limit = query->get_relations()[predicate->relation2]->get_num_of_records();
            for(uint64_t i = 0; i < limit; i++){
                //for easier reading of code
                NewColumnEntry cur_row = unindexed_relation[i];

                uint64_t val = cur_row.get_value();
                //take the bucket needed
                int bucket_num = h1(val);

                //search index for this record
                int index = index_array[bucket_num].get_bucket_array()[h2(val)];

                //traverse chain array and push qualified row_ids to respective vector
                while(index != -1){
                    if(indexed_relation[index + psum_array[join_index][bucket_num]].get_value() == val)
                        array_of_vectors[index + psum_array[join_index][bucket_num]]->push_back(cur_row.get_row_id());
                    index = index_array[bucket_num].get_chain_array()[index];
                }
            }
            //std::cerr << " ----join index 0 main loop finished in: " << get_wall_time() - t0 << endl;

            for(uint64_t i = 0; i < temp_set->size();i++)
                if(array_of_vectors[i]->size() == 0){
                    delete array_of_vectors[i];
                    array_of_vectors[i] = NULL;
                    map[indexed_relation[i].get_row_id()] = NULL;
                }
        }

        //t0 = get_wall_time();
        //get needed offset
        int offset = query->find_offset(predicate->relation1);
        int tuple_size = query->get_tuple_size();
        //now iterate over result buffer
        vector<uint64_t>::iterator it = (*result_buffer)->begin();
        //vector<uint64_t>::iterator it0;
        vector<uint64_t>* v;
        vector<uint64_t>::iterator vit;
        vector<uint64_t>::iterator res_buffer_end = (*result_buffer)->end();
        vector<uint64_t>::iterator vend;
        while(it != res_buffer_end){
            //it0 = it + offset;
            v = map[*(it + offset)];
            if(v){
                vit = v->begin();
                vend = v->end();
                while(vit != vend){
                    for(int i = 0;i < tuple_size; ++i)
                        new_vector->push_back(*(it+i));
                    new_vector->push_back(*vit);
                    ++vit;
                }
            }
            it += tuple_size;
        }

        for(uint64_t i = 0; i < temp_set->size(); i++)
            if(array_of_vectors[i] != NULL)
                delete array_of_vectors[i];

        //set order array since new tuple got bigger
        query->get_order()[query->get_order_index()] = predicate->relation2;
        query->incr_order_index();
        //delete previous result buffer
        //std::cerr << "rest finished in: " << get_wall_time() - t0 << endl;

    }
    else if(join_type == 2){
        NewColumnEntry* unindexed_relation = new_column[!join_index];
        NewColumnEntry* indexed_relation = new_column[join_index];

        //calculate unindexed relation's number of rows
        uint64_t num_of_rows;
        if(join_index == 0)
            num_of_rows = query->get_relations()[predicate->relation2]->get_num_of_records();
        else if(join_index == 1)
            num_of_rows = query->get_relations()[predicate->relation1]->get_num_of_records();

        //for every row in unindexed_relation
        for(uint64_t i = 0; i < num_of_rows; i++){
            //for easier reading of code
            NewColumnEntry cur_row = unindexed_relation[i];

            uint64_t val = cur_row.get_value();
            //take the bucket needed
            int bucket_num = h1(val);

            //search index for this record
            int index = index_array[bucket_num].get_bucket_array()[h2(val)];

            //traverse chain array and push qualified row_ids to respective vector
            while(index != -1){
                if(indexed_relation[index + psum_array[join_index][bucket_num]].get_value() == val){
                    uint64_t row1 = cur_row.get_row_id();
                    uint64_t row2 = indexed_relation[index + psum_array[join_index][bucket_num]].get_row_id();
                    //insert to results with correct order
                    //FIRST push unindexed row id, THEN indexed row id
                    new_vector->push_back(row1);
                    new_vector->push_back(row2);

                }
                index = index_array[bucket_num].get_chain_array()[index];
            }
        }

    }

    delete *result_buffer;
    // //set result_buffer to point to the new buffer that contains qualified tuples only
    *result_buffer = new_vector;


    return 0;
}
