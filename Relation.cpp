#include "Relation.h"

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

    //close relation's binary file
    infile.close();
    cout << "Relation " << file_name << " (" << num_of_records <<  ") loaded successfully." << endl;
};

Relation::~Relation(){
    free(columns);
}
