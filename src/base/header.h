//
// Created by ethan on 10/28/2025.
//

#ifndef INVENTORY_MANAGEMENT_HEADER_H
#define INVENTORY_MANAGEMENT_HEADER_H

#include "csv_parser.h"
#include "hash_table.h"
#include "hash_table_test.h"
#include "product.h"
#include "repl_manager.h"
#include "my_commands.h"

#include <string>
#include <iostream>
#include <vector>
#include <fstream>

void LoadDataFromFile(
    const std::string& filename,
    HashTable<std::string, Product> & product_database,
    HashTable<std::string, std::vector<std::string>> & categories_database
    );

#endif //INVENTORY_MANAGEMENT_HEADER_H