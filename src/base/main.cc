#include "header.h"

int main(void) {
  hash_table_test::TestAll();
  std::cout << "Loading Database..." << std::endl;
  HashTable<std::string, Product> product_database;
  HashTable<std::string, std::vector<std::string>> categories_database;
  LoadDataFromFile("../data/marketing_sample.csv", product_database, categories_database);
  std::cout << "Done!" << std::endl;

  std::cout << "Starting Repl..." << std::endl;

  bool exit = false;
  ReplManager my_repl_manager;
  ExitCommand my_exit(exit);
  FindCommand my_find(product_database);
  ListInventoryCommand my_list_inventory(product_database, categories_database);
  my_repl_manager.AddReplCommand(&my_exit);
  my_repl_manager.AddReplCommand(&my_find);
  my_repl_manager.AddReplCommand(&my_list_inventory);

  std::string line;
  const std::string kPrompt("> ");
  while (!exit) {
    std::cout << kPrompt << std::flush;
    std::getline(std::cin, line);
    my_repl_manager.Evaluate(line);
  }

  return 0;
}
