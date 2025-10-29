#include "header.h"

int main(void) {
  /*
  hash_table_test::TestAll();
  /*/
  std::cout << "Beginning REPL" << std::endl;
  HashTable<std::string, Product> product_database;
  HashTable<std::string, std::vector<std::string>> categories_database;
  LoadDataFromFile("../data/marketing_sample.csv", product_database, categories_database);
  //*/
  std::cout << "Done!" << std::endl;
  return 0;
}
