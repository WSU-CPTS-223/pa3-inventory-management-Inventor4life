#include "hash_table.h"
#include "hash_table_test.h"
#include "hash_table_test_i.h"

// Anonymous namespace for helper functions
namespace {
// Guarantees a rehash as long as there are currently less than or equal to 11
//  items in the table.
// Comments assume that there are no items in table. A table with 11 items will
//  have a capacity of 16, and will not rehash until 12 items are present.
// 6 items inserted in case the table currently consists of 6 items (capacity 16).
// At 12 items (capacity 32), this function will not reach the threshold of 23
//  items required for another rehash.
void ForceRehash(HashTable<std::string,std::string>& table) {
    // Force an initial rehash. Table rehashes before, because capacity
    //  initializes to zero.
    // New table will have a load factor of 1 and will rehash on second insertion
    table.Insert("Hello", ", World"); // Will cause rehash before insertion

    // Force a second rehash. After calling ForceRehashOne and this function,
    //  the hash table will have a loadfactor of 0.75. This will cause a rehash.
    // Also doubles as a linked-list tester.
    table.Insert("Goodbye", ", Friend"); // Cause a rehash after insertion when table detects load factor of 2. New state: {size 2, capacity 2}

    // Cause another rehash when table detects load factor of 1.5. New state: {size 3, capacity 4}
    // Before rehash, at least one node will be linked to another due to pigeonhole principle. Same with above insertion.
    table.Insert("Perry"," the platypus?!");

    // Cause a rehash when table detects load factor of 1. New state: {size 4, capacity 8}.
    table.Insert("Your", " Mother was a hamster and your Father smelt of elderberries!");

    // Insert two more to force another rehash. New state: {size 6, capacity 16}
    table.Insert("Luke", ", I am your father.");
    table.Insert("NOOO", "OOOOOOOOOOOOOOOOOO!");
}

std::string Pass() {
    return " -- PASSED\n";
}

////                        ////////////////////////////////////////////////////
//// INSERT METHOD TESTING  ///////////////////////////////////////////////////
////                        //////////////////////////////////////////////////

void BasicInsertTest() {
    std::cout << "BasicInsertTest";

    HashTable<std::string,std::string> table;
    table.Insert("Basic", "Insert Test");
    auto && i = table.Find("Basic");
    assert((*i).first == "Basic" && (*i).second == "Insert Test");
    std::cout << Pass();
}

void InsertAfterRehashTest() {
    std::cout << "InsertAfterRehashTest";

    HashTable<std::string,std::string> table;
    table.Insert("Rehash", "Insert Test");
    ForceRehash(table);
    auto && i = table.Find("Rehash");
    assert((*i).first == "Rehash" && (*i).second == "Insert Test");
    std::cout << Pass();
}

void InsertDuplicateTest() {
    std::cout << "InsertDuplicateTest";

    HashTable<std::string,std::string> table;
    table.Insert("Duplicate", "Insert Test");
    table.Insert("Duplicate", "Pass Test");
    auto && i = table.Find("Duplicate");
    assert((*i).first == "Duplicate" && (*i).second == "Pass Test");
    std::cout << Pass();
}

void InsertDuplicateAfterRehashTest() {
    std::cout << "InsertDuplicateAndRehashTest";
    HashTable<std::string,std::string> table;
    table.Insert("Duplicate", "Rehash Test");
    table.Insert("Duplicate", "Duplicate Rehash");
    ForceRehash(table);
    auto && i = table.Find("Duplicate");
    assert((*i).first == "Duplicate" && (*i).second == "Duplicate Rehash");
    table.Insert("Duplicate", "Duplicate Post Rehash");
    auto && q = table.Find("Duplicate");
    assert((*q).first == "Duplicate" && (*q).second == "Duplicate Post Rehash");
    std::cout << Pass();
}


void RecursiveTableInsert() {
    std::cout << "RecursiveTableInsertTest";
    HashTable <std::string,std::string> table;
    ForceRehash(table);
    HashTable <std::string,typeof(table)> SuperTable;
    SuperTable.Insert("this sucks.", table);
}

////                        ////////////////////////////////////////////////////
//// FIND METHOD TESTING    ///////////////////////////////////////////////////
////                        //////////////////////////////////////////////////

// This functionality is covered by BasicInsertTest.
/*
void BasicFindTest() {
    std::cout << "BasicFindTest" << std::endl;
    HashTable<std::string,std::string> table;
    table.Insert("Basic", "Find Test");
    auto && i = table.Find("Basic");
    assert((*i).first == "Basic" && (*i).second == "Find Test");
    std::cout << Pass();
}
*/
void FindNonexistentTest() {
    std::cout << "FindNonexistentTest";
    HashTable<std::string,std::string> table;
    ForceRehash(table); // Populate the table
    auto && i = table.Find("Rehash");
    assert(i == table.end());
    std::cout << Pass();
}

void FindEmptyTest() {
    std::cout << "FindEmptyTest";
    HashTable<std::string,std::string> table;
    auto && i = table.Find("Rehash");
    assert(i == table.end());
    std::cout << Pass();
}

////                        ////////////////////////////////////////////////////
//// DELETE METHOD TESTING  ///////////////////////////////////////////////////
////                        //////////////////////////////////////////////////

void BasicDeleteTest() {
    std::cout << "BasicDeleteTest";
    HashTable<std::string,std::string> table;
    table.Insert("Basic", "Delete Test");
    table.Delete("Basic");
    auto && i = table.Find("Basic");
    assert(i == table.end());
    std::cout << Pass();
}

// This test isn't as strong, as our only hope for detecting a bug is if the
//  Delete method deletes another item in the table.
void DeleteNonexistentTest() {
    std::cout << "DeleteNonexistentTest";
    HashTable<std::string,std::string> table;
    ForceRehash(table);
    table.Delete("Nonexistent");
    assert(table.size() == 6);
    std::cout << Pass();
}

void DeleteEmptyTest() {
    std::cout << "DeleteEmptyTest";
    HashTable<std::string,std::string> table;
    table.Delete("Empty");
    // Testing if this crashes program. No assert needed.
    std::cout << Pass();
}

void DeleteAfterRehashTest() {
    std::cout << "DeleteAfterRehashTest";
    HashTable<std::string,std::string> table;
    table.Insert("Rehash", "Delete Test");
    ForceRehash(table);
    table.Delete("Rehash");
    auto && i = table.Find("Rehash");
    assert(i == table.end());
    std::cout << Pass();
}

////                        ////////////////////////////////////////////////////
//// ITERATION TESTING      ///////////////////////////////////////////////////
////                        //////////////////////////////////////////////////
void BasicIterationTest() {
    std::cout << "BasicIterationTest" << std::endl;
    HashTable<std::string,std::string> table;
    ForceRehash(table);
    int count = 0;
    for (auto&& i : table) {
        count++;
        std::cout << i.first << i.second << std::endl;
    }
    assert(count == 6); // Make sure we have at least 6 items in table.
    // While we should test that all items are present (no duplicate items),
    //  I'm running out of time to finish this assignment.
    std::cout << "BasicIterationTest" << Pass();
}
}

namespace hash_table_test {
void TestAll() {
    std::cout << "----- RUNNING ALL TESTS -----" << std::endl;
    InsertTest();
    FindTest();
    DeleteTest();
    IterationTest();
    std::cout << "ALL TESTS PASSED" << std::endl;
}
void InsertTest() {
    std::cout << "----- Insert Tests -----" << std::endl;
    BasicInsertTest();
    InsertAfterRehashTest();
    InsertDuplicateTest();
    InsertDuplicateAfterRehashTest();
    RecursiveTableInsert();
    std::cout << "Insert Tests passed" << std::endl;
}
void FindTest() {
    std::cout << "----- Find Tests -----" << std::endl;
    FindNonexistentTest();
    FindEmptyTest();

    std::cout << "----- Find Tests passed" << std::endl;
}
void DeleteTest() {
    std::cout << "----- Delete Tests -----" << std::endl;
    BasicDeleteTest();
    DeleteNonexistentTest();
    DeleteEmptyTest();
    DeleteAfterRehashTest();
    std::cout << "----- Delete Tests passed" << std::endl;
}
void IterationTest() {
    std::cout << "----- Iteration Tests -----" << std::endl;
    BasicIterationTest();
    std::cout << "----- Iteration Tests passed" << std::endl;
}
}