#include "../include/Relation.h"
#define N 40000000

Relation::Relation(char* file_name){
    //open relation's binary file
    ifstream infile;
    infile.open(file_name, ios::binary | ios::in);
    if(infile.fail()){
        cout<<"error opening "<<file_name<<endl;
    }
    //format of binary file ('|' do not exist in the file):
    //uint64_t numTuples|uint64_t numColumns|uint64_t T0C0|uint64_t T1C0|..|uint64_t TnC0|uint64_t T0C1|..|uint64_t TnC1|..|uint64_t TnCm

    //read & set metadata of relation
    infile.read((char*)&num_of_records, sizeof(uint64_t));
    infile.read((char*)&num_of_columns, sizeof(uint64_t));

    //dynamically allocate space to store the columns & store them there
    uint64_t size = num_of_records * num_of_columns * sizeof(uint64_t);
    columns = (uint64_t*)malloc(size);
    infile.read((char*)columns, size);

    //allocate space for statistics
    l = (uint64_t*)malloc(num_of_records * sizeof(uint64_t));
    u = (uint64_t*)malloc(num_of_records * sizeof(uint64_t));
    f = (uint64_t*)malloc(num_of_records * sizeof(uint64_t));
    d = (uint64_t*)calloc(num_of_records , sizeof(uint64_t));

    //intialise bool array



    //calculate initial statistics
    //search every column to find min(l), max(h) and dinstictvalues(d). f is known already
    //for every column
    for(uint64_t j = 0; j < num_of_columns; j++){
        uint64_t* cur_column = columns + j * num_of_records;
        //standard for each column
        f[j] = num_of_records;

        //initialise min and max to first row value of the column
        l[j] = cur_column[0];
        u[j] = cur_column[0];

        //for every record in the column
        for(uint64_t i = 0; i < num_of_records; i++){
            //min
            if(cur_column[i] < l[j])
                l[j] = cur_column[i];

            //max //TODO: use else if ?
            if(cur_column[i] > u[j])
                u[j] = cur_column[i];
        }

        //allocate bool_array to calculate d[j]
        uint64_t num = u[j] - l[j] + 1;
        uint64_t bool_size = (num > N) ? N : num;
        bool* bool_array = (bool*)calloc(bool_size, sizeof(bool));

        //main loop (depends on bool_size)
        if(bool_size == N){
            //for every record in the column
            for(uint64_t i = 0; i < num_of_records; i++){
                if(bool_array[cur_column[i] - l[j] % N] == false){
                    bool_array[cur_column[i] - l[j] % N] = true;
                    d[j]++;
                }
            }
        }
        else if(bool_size == num_of_records){
            //for every record in the column
            for(uint64_t i = 0; i < num_of_records; i++){
                if(bool_array[cur_column[i] - l[j]] == false){
                    bool_array[cur_column[i] - l[j]] = true;
                    d[j]++;
                }
            }
        }

        //clean bool_array
        free(bool_array);

    }

    //close relation's binary file
    infile.close();
    //cout << "Relation " << file_name << " (" << num_of_records <<  ") loaded successfully." << endl;
};

Relation::~Relation(){
    //free columns(data)
    free(columns);

    //free statistics
    free(l);
    free(u);
    free(f);
    free(d);
}
