//
// Created by ethan on 10/28/2025.
//

#ifndef INVENTORY_MANAGEMENT_PRODUCT_H
#define INVENTORY_MANAGEMENT_PRODUCT_H

#include "hash_table.h"

#include <string>

class Product {
public:
    Product() = default;
    ~Product() = default;
    HashTable<std::string, std::string> fields;
};

#endif //INVENTORY_MANAGEMENT_PRODUCT_H