#include "gtest/gtest.h"
#include <fstream>
#include <iostream>
#include <cinttypes>
#include <string>
#include <cstring>
#include <cstdlib>
#include <math.h>
#include "../../include/JoinEngine.h"


using namespace std;

//TODO: more automation and data generation functions

TEST(LoadRelationsTest, LoadRelationsToMemory) {
  char const *argv[5];
  argv[1] = "../datasets/r0";
  argv[2] = "0";
  argv[3] = "../datasets/r1";
  argv[4] = "1";

  JoinEngine *joinEngine = new JoinEngine(argv);
  EXPECT_EQ(joinEngine->load_relations(),0);

  EXPECT_TRUE(joinEngine->get_relations()[0] != NULL);
  EXPECT_TRUE(strcmp("../datasets/r1",joinEngine->get_relations()[1]->get_name().c_str()) == 0);
  EXPECT_TRUE(joinEngine->get_relations()[1] != NULL);
  EXPECT_TRUE(joinEngine->get_relations()[1]->get_num_of_records() > 0);
  EXPECT_TRUE(joinEngine->get_relations()[1]->get_column() != NULL);
  EXPECT_TRUE(joinEngine->get_relations()[1]->get_column()[0] >= 0);
  delete joinEngine;
}

TEST(SegmentationTest, ComputeHistArray) {
  char const *argv[5];
  argv[1] = "relation0";
  argv[2] = "0";
  argv[3] = "relation1";
  argv[4] = "1";

  /* Initialize join engine with a mock relation */
  JoinEngine *joinEngine = new JoinEngine(argv,16);
  joinEngine->get_relations()[0]->set_num_of_records(3);
  size_t column_size = sizeof(uint64_t) * joinEngine->get_relations()[0]->get_num_of_records();
  joinEngine->get_relations()[0]->set_column(column_size);
  joinEngine->get_relations()[0]->get_column()[0] = (uint64_t) 17; // 10001
  joinEngine->get_relations()[0]->get_column()[1] = (uint64_t) 16; // 10000
  joinEngine->get_relations()[0]->get_column()[2] = (uint64_t) 15; // 01111

  // hist array expectations
  EXPECT_EQ(joinEngine->create_and_compute_hist_array(joinEngine->get_relations()[0]),0);
  EXPECT_TRUE(joinEngine->get_relations()[0]->get_hist_array() != NULL);
  EXPECT_TRUE(joinEngine->get_relations()[0]->get_hist_array()[0] == 1);
  EXPECT_TRUE(joinEngine->get_relations()[0]->get_hist_array()[1] == 1);
  EXPECT_TRUE(joinEngine->get_relations()[0]->get_hist_array()[15] == 1);

  delete joinEngine;
}

TEST(SegmentationTest, ComputePsumArray) {
  char const *argv[5];
  argv[1] = "relation0";
  argv[2] = "0";
  argv[3] = "relation1";
  argv[4] = "1";

  /* Initialize join engine with a mock hist array */
  JoinEngine *joinEngine = new JoinEngine(argv,16);
  joinEngine->get_relations()[0]->set_num_of_records(3);
  joinEngine->get_relations()[0]->set_hist_array(16);
  joinEngine->get_relations()[0]->get_hist_array()[0] = 2;
  joinEngine->get_relations()[0]->get_hist_array()[1] = 1;
  joinEngine->get_relations()[0]->get_hist_array()[2] = 0;

  // psum array expectations
  EXPECT_EQ(joinEngine->create_and_compute_psum_array(joinEngine->get_relations()[0]),0);
  EXPECT_TRUE(joinEngine->get_relations()[0]->get_psum_array() != NULL);
  EXPECT_TRUE(joinEngine->get_relations()[0]->get_psum_array()[0] == 0);
  EXPECT_TRUE(joinEngine->get_relations()[0]->get_psum_array()[1] == 2);
  EXPECT_TRUE(joinEngine->get_relations()[0]->get_psum_array()[2] == 3);
  EXPECT_TRUE(joinEngine->get_relations()[0]->get_psum_array()[3] == 3);

  delete joinEngine;
}

TEST(SegmentationTest, ComputeNewColumn) {
  char const *argv[5];
  argv[1] = "relation0";
  argv[2] = "0";
  argv[3] = "relation1";
  argv[4] = "1";

  /* Initialize join engine with a mock relation */
  JoinEngine *joinEngine = new JoinEngine(argv,16);
  joinEngine->get_relations()[0]->set_num_of_records(3);
  size_t column_size = sizeof(uint64_t) * joinEngine->get_relations()[0]->get_num_of_records();
  joinEngine->get_relations()[0]->set_column(column_size);
  joinEngine->get_relations()[0]->get_column()[0] = (uint64_t) 17; // h1 -> 1
  joinEngine->get_relations()[0]->get_column()[1] = (uint64_t) 16; // h1 -> 0
  joinEngine->get_relations()[0]->get_column()[2] = (uint64_t) 32; // h1 -> 0

  joinEngine->get_relations()[0]->set_psum_array(4);
  joinEngine->get_relations()[0]->get_psum_array()[0] = 0;
  joinEngine->get_relations()[0]->get_psum_array()[1] = 2;
  joinEngine->get_relations()[0]->get_psum_array()[2] = 3;
  joinEngine->get_relations()[0]->get_psum_array()[3] = 3;

  // new column expectations
  EXPECT_EQ(joinEngine->create_and_compute_new_column(joinEngine->get_relations()[0]),0);
  EXPECT_TRUE(joinEngine->get_relations()[0]->get_new_column() != NULL);
  EXPECT_TRUE(joinEngine->get_relations()[0]->get_new_column()[0].get_value() == 16);
  EXPECT_TRUE(joinEngine->get_relations()[0]->get_new_column()[0].get_index() == 1);
  EXPECT_TRUE(joinEngine->get_relations()[0]->get_new_column()[1].get_value() == 32);
  EXPECT_TRUE(joinEngine->get_relations()[0]->get_new_column()[1].get_index() == 2);
  EXPECT_TRUE(joinEngine->get_relations()[0]->get_new_column()[2].get_value() == 17);
  EXPECT_TRUE(joinEngine->get_relations()[0]->get_new_column()[2].get_index() == 0);

  delete joinEngine;
}

TEST(IndexingTest, InitIndex) {
  char const *argv[5];
  argv[1] = "relation0";
  argv[2] = "0";
  argv[3] = "relation1";
  argv[4] = "1";

  /* Initialize join engine with a mock relation */
  JoinEngine *joinEngine = new JoinEngine(argv,4);
  joinEngine->get_relations()[0]->set_num_of_records(3);
  size_t column_size = sizeof(uint64_t) * joinEngine->get_relations()[0]->get_num_of_records();
  joinEngine->get_relations()[0]->set_index_array(4);
  Index& cur_index = joinEngine->get_relations()[0]->get_index_array()[0];

  //index expectations
  EXPECT_EQ(joinEngine->create_and_init_chain_and_bucket_array(cur_index,2),0);
  EXPECT_TRUE(cur_index.get_bucket_array()[0] == -1);
  EXPECT_TRUE(cur_index.get_bucket_array()[1] == -1);
  EXPECT_TRUE(cur_index.get_chain_array()[0] == 0);
  EXPECT_TRUE(cur_index.get_chain_array()[1] == 0);

  delete joinEngine;
}

TEST(IndexingTest, CalculateIndex) {
  char const *argv[5];
  argv[1] = "relation0";
  argv[2] = "0";
  argv[3] = "relation1";
  argv[4] = "1";

  /* Initialize join engine with a mock relation */
  JoinEngine *joinEngine = new JoinEngine(argv,4);
  joinEngine->get_relations()[0]->set_num_of_records(3); //will choose to index the smallest
  joinEngine->get_relations()[1]->set_num_of_records(10);
  joinEngine->get_relations()[0]->set_hist_array(16);
  joinEngine->get_relations()[0]->get_hist_array()[0] = 2;
  joinEngine->get_relations()[0]->get_hist_array()[1] = 1;
  joinEngine->get_relations()[0]->get_hist_array()[2] = 0;
  joinEngine->get_relations()[0]->get_hist_array()[3] = 0;

  joinEngine->get_relations()[0]->set_psum_array(16);
  joinEngine->get_relations()[0]->get_psum_array()[0] = 0;
  joinEngine->get_relations()[0]->get_psum_array()[1] = 2;
  joinEngine->get_relations()[0]->get_psum_array()[2] = 3;
  joinEngine->get_relations()[0]->get_psum_array()[3] = 3;

  joinEngine->get_relations()[0]->set_new_column(16);
  joinEngine->get_relations()[0]->get_new_column()[0].set(1,16699);
  joinEngine->get_relations()[0]->get_new_column()[1].set(2,2*16699);
  joinEngine->get_relations()[0]->get_new_column()[2].set(0,16700);

  //index expectations
  EXPECT_EQ(joinEngine->indexing(),0);
  EXPECT_TRUE(joinEngine->get_relations()[0]->get_index_array() != NULL);
  EXPECT_TRUE(joinEngine->get_relations()[0]->get_index_array()[0].get_chain_array() != NULL);
  EXPECT_TRUE(joinEngine->get_relations()[0]->get_index_array()[0].get_chain_array()[0] == -1);
  EXPECT_TRUE(joinEngine->get_relations()[0]->get_index_array()[0].get_chain_array()[1] == 0);
  EXPECT_TRUE(joinEngine->get_relations()[0]->get_index_array()[0].get_bucket_array()[joinEngine->h2(16699)] == 1);

  EXPECT_TRUE(joinEngine->get_relations()[0]->get_index_array()[1].get_chain_array()[0] == -1);
  EXPECT_TRUE(joinEngine->get_relations()[0]->get_index_array()[1].get_bucket_array()[joinEngine->h2(16700)] == 0);

  delete joinEngine;
}

TEST(OutputListTest, InsertData) {
  OutputList *outList = new OutputList();
  outList->InsertData((uint64_t) 10, (uint64_t) 100);
  outList->InsertData((uint64_t) 5, (uint64_t) 50);

  EXPECT_TRUE(outList->curNode->data[0] == 10);
  EXPECT_TRUE(outList->curNode->data[1] == 100);
  EXPECT_TRUE(outList->curNode->data[2] == 5);
  EXPECT_TRUE(outList->curNode->data[3] == 50);

  //fill the 1st node
  uint64_t row1 = 99;
  uint64_t row2 = 99;
  while(outList->totalBytes < 1024*1024)
      outList->InsertData(row1, row2);

  //should be on 2nd node
  outList->InsertData((uint64_t) 7, (uint64_t) 34);
  outList->InsertData((uint64_t) 2, (uint64_t) 22);

  //test current node
  EXPECT_TRUE(outList->numBuckets == 2);
  EXPECT_TRUE(outList->curNode->data[0] == 7);
  EXPECT_TRUE(outList->curNode->data[1] == 34);
  EXPECT_TRUE(outList->curNode->data[2] == 2);
  EXPECT_TRUE(outList->curNode->data[3] == 22);

  //test headnode
  EXPECT_TRUE(outList->headNode->data[0] == 10);
  EXPECT_TRUE(outList->headNode->data[1] == 100);
  EXPECT_TRUE(outList->headNode->data[2] == 5);

  delete outList;

}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
