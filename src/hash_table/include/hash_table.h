#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "hash_table_container.h"
#include <functional> //std::hash
#include <stdexcept> // out_of_range error when dereferencing invalid iterator
#include <utility> //std::pair

template <typename Key, typename Value>
class Iterator;

template <typename Key, typename Value>
class HashTable {
public:
    HashTable();
    ~HashTable();

    Iterator<Key, Value> Find(Key& key);
    void Insert(Key& key, Value& value);
    void Delete(Key& key);
    Iterator<Key, Value> begin();
    Iterator<Key, Value> end();

    unsigned int size() const;
    unsigned int capacity() const;
    float GetLoadFactor() const;

private:
    friend Iterator<Key, Value>;

    /////// BEGIN SETTINGS
    // Rehash threshold
    static constexpr unsigned int kMaxContainerDepth = 2;
    static constexpr float kMaxLoadFactor = 0.7;
    static constexpr unsigned int kMaxTableCapacity = 32768;
    /////// END SETTINGS

    int FindValidNode_(int start_index);
    void Rehash_();
    void UpdateLoadFactor_();
    bool RequireRehash_(int new_node_index);
    unsigned int GetNextSize_() const;
    void InsertAt_(HashTableContainer<Key, Value>* destination_array, Key& key, Value& value);
    HashTableContainer<Key, Value>* Get(int index, int depth = 0);
    void DeleteAt_(int index, int depth);
    std::pair<int, int> Find_(Key& key);
    int GetPotentialIndex_(Key& key);

    std::hash<Key> hasher_;
    HashTableContainer<Key, Value>* container_array_;
    unsigned int size_;
    unsigned int capacity_;
    float load_factor_;
};


// Iterator Definition
template <typename Key, typename Value>
class Iterator {
public:
    Iterator() = delete; // No default constructor, reference to table is required
    ~Iterator() = default;    // destructor
    Iterator(Iterator& other) = default; // copy constructor

    // parameterized constructor for non-iterable iterator (returned by find())
    Iterator(HashTable<Key, Value>& main_table, HashTableContainer<Key, Value>* current_node);

    // parameterized constructor for iterable iterator (used by range-based for loops)
    explicit Iterator(HashTable<Key, Value>& main_table);
    //Iterator<Key, Value>& operator=(Iterator<Key,Value>& right); // Copy assignment
    Iterator<Key, Value>& operator++(); // pre-increment operator
    std::pair<Key, Value> operator*() const; // de-reference operator
    bool operator!=(const Iterator<Key, Value>& right) const; // inequality operator

private:
    // index of -1 indicates a non-iterable iter-inator (a platypus?)
    int index_;
    HashTableContainer<Key, Value>* current_node_;
    HashTable<Key, Value>& main_table_;
};

/*
template<typename Key, typename Value>
Iterator<Key, Value>::Iterator() {
    index_ = -1;
    current_node_ = nullptr;
    main_table_ = nullptr;
}
*/

template<typename Key, typename Value>
HashTable<Key, Value>::HashTable() {
    size_ = 0;
    capacity_ = 0;
    UpdateLoadFactor_();
    container_array_ = nullptr;
}

template<typename Key, typename Value>
HashTable<Key, Value>::~HashTable() {
    delete[] container_array_;
}

template<typename Key, typename Value>
Iterator<Key, Value> HashTable<Key, Value>::Find(Key &key) {
    std::pair<int, int> location = Find_(key);
    if (location.first != -1 && location.second != -1) {
        return Iterator<Key, Value>(*this, this->Get(location.first, location.second));
    }
    // else
    return this->end();
}

template<typename Key, typename Value>
void HashTable<Key, Value>::Insert(Key &key, Value &value) {
    InsertAt_(container_array_, key, value);
}

template<typename Key, typename Value>
void HashTable<Key, Value>::Delete(Key &key) {
    std::pair<int, int> location = Find_(key);
    if (location.first != -1 && location.second != -1) {
        DeleteAt_(location.first, location.second);
    }
}

// There is an edge case here where the old nodes, when inserted into the new array,
//  form a linked list longer than kMaxContainerDepth. This edge case will not be handled
//  automatically, and will instead be caught if another container is inserted at that index.
template<typename Key, typename Value>
void HashTable<Key, Value>::Rehash_() {
}

template<typename Key, typename Value>
void HashTable<Key, Value>::InsertAt_(HashTableContainer<Key, Value> *destination_array, Key& key, Value &value) {

}

// As this is an internal function, it is assumed that the index and depth values are already verified.
template<typename Key, typename Value>
void HashTable<Key, Value>::DeleteAt_(int index, int depth) {
    HashTableContainer<Key, Value>* head_node = this->Get(index);
    HashTableContainer<Key, Value>* current_node = head_node->GetIndex(depth);
    HashTableContainer<Key, Value>* next_node = current_node->GetNext();
    if (depth == 0) {
        // Node to delete is head of linked list. Need to swap with next item in list.
        if (next_node->isValid()) {
            // There is a next item
            *current_node = *next_node; // Replace current node with next node
            // Delete out-of-array copy of next_node.
            // I could have designed this better by having a dedicated linked-list class
            //  rather than linked nodes. As-is, a destructed node will call the destructor of the
            //  next node in the list if the current node is valid when destructed.
            next_node->SetInvalid();
            delete next_node;
        } else {
            // There is no next item. We cannot delete the current node, as it is part of the container array.
            current_node->SetInvalid();
        }
    } else {
        // Update previous node to point at next node
        HashTableContainer<Key, Value>* previous_node = head_node->GetIndex(depth - 1);
        previous_node->SetNext(next_node);

        // Delete current node
        current_node->SetInvalid();
        delete current_node;
    }
}

template<typename Key, typename Value>
std::pair<int, int> HashTable<Key, Value>::Find_(Key &key) {
    //if (this->capacity() == 0) return std::make_pair(-1, -1); // State validation should occur in public functions
    int potential_index = GetPotentialIndex_(key);
    int depth = 0;
    HashTableContainer<Key,Value>* current_node = container_array_[potential_index];
    while (current_node != nullptr && current_node->IsValid()) {
        if (current_node->GetKey() == key) {return std::make_pair(potential_index, depth);}
        current_node = current_node->GetNext();
        depth++;
    }
    return std::make_pair(-1, -1);
}

template<typename Key, typename Value>
int HashTable<Key, Value>::GetPotentialIndex_(Key &key) {
    if (capacity_ == 0) {return 0;}
    //else
    return hasher_(key) % this->capacity();
}

template<typename Key, typename Value>
Iterator<Key, Value> HashTable<Key, Value>::begin() {
    return Iterator<Key, Value>(*this);
}

template<typename Key, typename Value>
Iterator<Key, Value> HashTable<Key, Value>::end() {
    return Iterator<Key, Value>(*this,nullptr);
}

template<typename Key, typename Value>
unsigned int HashTable<Key, Value>::size() const{
    return size_;
}

template<typename Key, typename Value>
unsigned int HashTable<Key, Value>::capacity() const{
    return capacity_;
}

template<typename Key, typename Value>
float HashTable<Key, Value>::GetLoadFactor() const{
    return load_factor_;
}

template<typename Key, typename Value>
int HashTable<Key, Value>::FindValidNode_(int start_index) {
    if (start_index < 0 || start_index >= this->size()) {return -1;}
    while (this->Get(start_index) != nullptr && !this->Get(start_index)->isValid()) {
        start_index++;
    }
    if (this->Get(start_index) != nullptr) {
        return start_index; // start_index is the index to a valid node
    } else {
        return -1; // start_index reached the end of the table and found no valid nodes
    }
}

template<typename Key, typename Value>
void HashTable<Key, Value>::UpdateLoadFactor_() {
    if (this->capacity() == 0) load_factor_ = 0;
    else load_factor_ = this->size()/static_cast<float>(this->capacity());
}

template<typename Key, typename Value>
bool HashTable<Key, Value>::RequireRehash_(int new_node_index) {
    // Verify most recent bucket size is less than max
    int bucket_size = 0;
    HashTableContainer<Key, Value>* current_node = this->Get(new_node_index);
    while (current_node != nullptr) {
        if (current_node->IsValid()) {
            bucket_size++;
            current_node = current_node->GetNext();
        } else {
            break;
        }
    }
    if (bucket_size > kMaxContainerDepth) {
        return true;
    }
    // Else
    // Verify new load factor is less than max
    if (GetLoadFactor() > kMaxLoadFactor) {
        return true;
    }
    return false;

}

template<typename Key, typename Value>
unsigned int HashTable<Key, Value>::GetNextSize_() const {
    if (capacity_ == 0) return 1;
    //else
    if (kMaxTableCapacity == 0) return this->capacity() * 2;
    //else
    if (this->capacity() * 2 <= kMaxTableCapacity) return this->capacity() * 2;
    //else
    return kMaxTableCapacity; // proposed capacity was greater than kMaxTableCapacity
}

template<typename Key, typename Value>
HashTableContainer<Key, Value> * HashTable<Key, Value>::Get(int index, int depth) {
    if (index < 0 || index >= this->size()) return nullptr;
    HashTableContainer<Key, Value> current_node = container_array_[index];
    for (int i = 0; i < depth; i++) {
        current_node = current_node->GetNext();
    }
    return current_node;
}

template<typename Key, typename Value>
Iterator<Key, Value>::Iterator(HashTable<Key, Value>& main_table, HashTableContainer<Key, Value> *current_node) : main_table_(main_table) {
    if (main_table.size() == 0 || current_node == nullptr) {
        index_ = -1;
    }
    current_node_ = current_node;
}

template<typename Key, typename Value>
Iterator<Key, Value>::Iterator(HashTable<Key, Value> &main_table) : main_table_(main_table) {
    index_ = main_table.FindValidNode_(0);
    if (index_ == -1) current_node_ = nullptr;
    else current_node_ = main_table_.Get(index_);
}

template<typename Key, typename Value>
Iterator<Key, Value> & Iterator<Key, Value>::operator++() {
    // If current iterator is non-iterable
    if (index_ == -1) {
        current_node_ = nullptr;
        return *this;
    }
    // else
    // If there are more nodes in the current linked list
    HashTableContainer<Key, Value>* next_node = current_node_->GetNext();
    if (next_node != nullptr && next_node->IsValid()) {
        current_node_ = next_node;
        return *this;
    }
    // else
    // Find the next valid node in the array. If none are found, invalidate the iterator and set current_node to nullptr
    index_ = main_table_.FindValidNode_(++index_);
    if (index_ == -1) current_node_ = nullptr;
    else current_node_ = main_table_.Get(index_);
    return *this;
}

template<typename Key, typename Value>
std::pair<Key, Value> Iterator<Key, Value>::operator*() const {
    if (current_node_ == nullptr) {throw std::out_of_range("Error, dereferencing invalid iterator");}
    return std::make_pair(current_node_->GetKey_(), current_node_->GetValue_());
}

template<typename Key, typename Value>
bool Iterator<Key, Value>::operator!=(const Iterator<Key, Value> &right) const {
    if (this == &right) return false;
    if (this->current_node_ == right.current_node_
        && this->index_ == right.index_
        && this->main_table_ == right.main_table_) return false;
    return true;
}
#endif // !HASH_TABLE_H
