//
// Created by ethan on 10/29/2025.
//

#ifndef INVENTORY_MANAGEMENT_MY_COMMANDS_H
#define INVENTORY_MANAGEMENT_MY_COMMANDS_H

#include "repl_command.h"
#include "product.h"
#include "hash_table.h"

#include <string>
#include <vector>
#include <iostream>

class ExitCommand : public ReplCommand {
public:
    explicit ExitCommand(bool& exit) : exit_(exit) {};
    ~ExitCommand() = default;
    std::string GetCommand() const override { return {"exit"}; }
    std::string GetHelpText() const override { return {"quit the program."};}
    void Execute(std::string argument) const override {exit_ = true;}
private:
    bool& exit_;
};

class FindCommand : public ReplCommand {
    public:
    explicit FindCommand(HashTable<std::string, Product>& product_database) : product_database_(product_database) {};
    ~FindCommand() = default;
    std::string GetCommand() const override {
        return {"find"};
    }
    std::string GetHelpText() const override {
        return {"find product details from inventory ID. Usage: find <uniq_id>"};
    }
    void Execute(std::string argument) const override {
        std::string product_id = argument.substr(argument.find(' ')+1);
        auto && i = product_database_.Find(product_id);
        if (i != product_database_.end()) {
            for (auto && q : (*i).second.fields) {
                std::cout << q.first << ": " << q.second << std::endl;
            }
        } else {
            // Invalid product ID
            std::cout << "Inventory/Product not found." << std::endl;
        }
    }

private:
    HashTable<std::string, Product>& product_database_;
};

class ListInventoryCommand : public ReplCommand {
public:
    explicit ListInventoryCommand(HashTable<std::string, Product>& product_database, HashTable<std::string, std::vector<std::string>>& categories_database)
        : product_database_(product_database), categories_database_(categories_database) {};
    ~ListInventoryCommand() = default;
    std::string GetCommand() const override {
        return {"list_inventory"};
    }
    std::string GetHelpText() const override {
        return {"returns a list of product names and uniq_ids in a category. Usage: list_inventory <category>"};
    }
    void Execute(std::string argument) const override {
        std::string category = argument.substr(argument.find(' ')+1);
        auto && i = categories_database_.Find(category);
        if (i != categories_database_.end()) {
            for (const std::string & q : (*i).second) {
                auto && j = product_database_.Find(q);
                if (j != product_database_.end()) {
                    std::cout << (*j).first << ": " << (*((*j).second.fields.Find("Product Name"))).second << std::endl;
                }
            }
        } else {
            std::cout << "Invalid Category." << std::endl;
        }
    }
private:
    HashTable<std::string, Product>& product_database_;
    HashTable<std::string, std::vector<std::string>>& categories_database_;
};
#endif //INVENTORY_MANAGEMENT_MY_COMMANDS_H