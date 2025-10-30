//
// Created by ethan on 10/28/2025.
//
#include "header.h"

void AddToCategory(std::string category_name, const std::string& uniq_id, HashTable<std::string, std::vector<std::string>>& categories_database) {
    std::vector<std::string> current_category_ids;
    if (category_name.empty()) {
        category_name = "NA";
    }
    auto && i = categories_database.Find(category_name);
    if (i != categories_database.end()) {
    // Inserting into an existing category
        (*i).second.push_back(uniq_id);
    }
    current_category_ids.push_back(uniq_id);
    // Because of our insert implementation, this overrides previous category.
    categories_database.Insert(category_name, current_category_ids);
}

std::vector<std::string> SeparateIntoCategories(const std::string& input) {
    std::vector<std::string> categories;
    std::string current_string;
    for (char i : input) {
        if (i == ' ' && current_string.empty()) continue; // [ Ignore this space
        if (i == '|') { // Delimiter is " | " <--------------]
            current_string.pop_back(); //^pop this space
            categories.push_back(current_string);
            current_string.clear();
            continue;
        }
        current_string.push_back(i);
    }
    categories.push_back(current_string); // No '|' at end of categories list
    return categories;
}

void LoadDataFromFile(const std::string& filename, HashTable<std::string, Product>& product_database, HashTable<std::string, std::vector<std::string>>& categories_database) {
    std::ifstream file(filename);
    std::vector<std::string> header_line = csv::ReadLine(file);
    std::vector<std::string> data_line = csv::ReadLine(file);
    while (!data_line[0].empty()) {
        ///
        /// Insert into product database
        ///
        // create Product, fields cleared automatically by loop.
        Product this_product;
        for (int i = 0; i < header_line.size(); i++) {
            this_product.fields.Insert(header_line[i], data_line[i]);
        }
        // Insert Product into product database
        product_database.Insert(data_line[0],this_product); // data_line[0] is Uniq_ID

        ///
        /// Insert into categories database
        ///
        unsigned int constexpr kCategoryFieldIndex = 4;
        std::vector<std::string> categories = SeparateIntoCategories(data_line[kCategoryFieldIndex]);
        for (std::string& category : categories) {
            AddToCategory(category, data_line[0], categories_database);
        }

        data_line = csv::ReadLine(file);
    }
}