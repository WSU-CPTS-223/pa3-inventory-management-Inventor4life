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

    HashTable(const HashTable& other);

    Iterator<Key, Value> Find(const Key &key);
    void Insert(const Key& key, const Value& value);
    void Delete(const Key& key);
    Iterator<Key, Value> begin() const;
    Iterator<Key, Value> end() const;

    unsigned int size() const;
    unsigned int capacity() const;
    float GetLoadFactor() const;

    HashTable& operator=(const HashTable& other);
    HashTable& operator=(HashTable&& other) noexcept;

private:
    friend Iterator<Key, Value>;

    /////// BEGIN SETTINGS
    // Rehash threshold
    static constexpr unsigned int kMaxContainerDepth = 2;
    static constexpr float kMaxLoadFactor = 0.7;
    static constexpr unsigned int kMaxTableCapacity = 32768;
    /////// END SETTINGS

    int FindValidNode_(int start_index) const;
    void Rehash_();
    void UpdateLoadFactor_();
    bool RequireRehash_(int new_node_index);
    unsigned int GetNextSize_() const;

    std::pair<int, bool> InsertAt_(HashTableContainer<Key, Value> *destination_array, unsigned int array_size,
                                   const Key &key, const Value &value);
    std::pair<int, bool> InsertAt_(HashTableContainer<Key, Value> *destination_array, unsigned int array_size, HashTableContainer<Key, Value>&& source);
    HashTableContainer<Key, Value>* Get(int index, int depth = 0) const;
    void DeleteAt_(int index, int depth);
    std::pair<int, int> Find_(const Key &key);
    int GetPotentialIndex_(const Key &key);
    int GetPotentialIndexUnsized(const Key &key, unsigned int table_capacity);

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
    Iterator(const Iterator& other) = default; // copy constructor

    // parameterized constructor for non-iterable iterator (returned by find())
    Iterator(const HashTable<Key, Value>& main_table, HashTableContainer<Key, Value>* current_node);

    // parameterized constructor for iterable iterator (used by range-based for loops)
    explicit Iterator(const HashTable<Key, Value>& main_table);
    //Iterator<Key, Value>& operator=(Iterator<Key,Value>& right); // Copy assignment
    Iterator<Key, Value>& operator++(); // pre-increment operator
    std::pair<const Key&,const Value&> operator*() const; // de-reference operator
    bool operator!=(const Iterator<Key, Value>& right) const; // inequality operator
    bool operator==(const Iterator<Key, Value>& right) const;
private:
    friend HashTable<Key, Value>;
    // index of -1 indicates a non-iterable iter-inator (a platypus?)
    int index_;
    HashTableContainer<Key, Value>* current_node_;
    const HashTable<Key, Value>& main_table_;
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
    for (int i = 0; i < capacity(); i++) {
        HashTableContainer<Key, Value>* current_node = &container_array_[i];
        HashTableContainer<Key, Value>* temp_ptr = nullptr;
        if (current_node->IsValid() && current_node->GetNext() != nullptr) {
                current_node = current_node->GetNext(); // Skip node in array
            while (current_node != nullptr) {
                temp_ptr = current_node->GetNext();
                delete current_node;
                current_node = temp_ptr;
            }
        }
    }
    //delete[] container_array_;
}

template<typename Key, typename Value>
HashTable<Key, Value>::HashTable(const HashTable &other) {
    *this = other;
}

template<typename Key, typename Value>
Iterator<Key, Value> HashTable<Key, Value>::Find(const Key &key) {
    std::pair<int, int> location = Find_(key);
    if (location.first != -1 && location.second != -1) {
        return Iterator<Key, Value>(*this, this->Get(location.first, location.second));
    }
    // else
    return this->end();
}

template<typename Key, typename Value>
void HashTable<Key, Value>::Insert(const Key &key, const Value &value) {
    if (capacity() == 0) Rehash_(); // Rehash on initial insertion, takes the form of solely allocating an initial table
    std::pair<int, bool> result = InsertAt_(container_array_, this->capacity(), key, value);
    if (!result.second /*if no key collision*/) {++size_;UpdateLoadFactor_();}
    if (RequireRehash_(result.first /*index*/)) {Rehash_();}
}

template<typename Key, typename Value>
void HashTable<Key, Value>::Delete(const Key &key) {
    std::pair<int, int> location = Find_(key);
    if (location.first != -1 && location.second != -1) {
        DeleteAt_(location.first, location.second);
        --size_;
        UpdateLoadFactor_();
    }
}

// There is an edge case here where the old nodes, when inserted into the new array,
//  form a linked list longer than kMaxContainerDepth. This edge case will not be handled
//  automatically, and will instead be caught if another container is inserted at that index.
//
// This implementation is very inefficient, as it de-allocates and re-allocates the out-of-array nodes when they could
//  simply be re-used. However, doing so (with my current implementation) would require traversing a singly-linked list backwards and
//  would only affect the efficiency during re-hashes (of which there are only 15 that are relevant given current max table size)
template<typename Key, typename Value>
void HashTable<Key, Value>::Rehash_() {
    unsigned int new_size = GetNextSize_();
    if (new_size == capacity()) return; // Don't rehash if new table size is same as old one.
    HashTableContainer<Key, Value>* new_table = new HashTableContainer<Key, Value>[new_size];
    if (new_table == nullptr) return; // Dynamic allocation failed, don't rehash
    for (Iterator<Key, Value> item = this->begin(); item != this->end(); ++item)
    {
        this->InsertAt_(new_table, new_size, std::move(*(item.current_node_)));
    }
    this->capacity_ = new_size;
    delete[] container_array_;
    this->container_array_ = new_table;

}

// Returns std::pair<int, bool>.
// int (first) is index of head of linked list containing inserted node.
// bool (second) is true iff a valid node containing that key already exists (and has been overwritten)
template<typename Key, typename Value>
std::pair<int, bool> HashTable<Key, Value>::InsertAt_(HashTableContainer<Key, Value> *destination_array,
                                                      unsigned int array_size, const Key &key, const Value &value) {
    // Internal function. Does not verify inputs. (array_size being 0, destination_array being nullptr...)
    unsigned int potential_index = GetPotentialIndexUnsized(key, array_size);
    HashTableContainer<Key, Value>* current_node = &destination_array[potential_index];
    HashTableContainer<Key, Value>* previous_node = nullptr;

    while (current_node != nullptr) {
        if (current_node->GetKey() == key || !current_node->IsValid()) {
            // Override previous value if node with same key already exists,
            //  or if an invalid node that can contain the new node is found.
            bool node_was_valid = current_node->IsValid();
            *current_node = HashTableContainer<Key, Value>(key, value);
            return std::make_pair(potential_index, node_was_valid);
        }

        previous_node = current_node;
        current_node = current_node->GetNext();
        /* redesign rendered this unnecessary
        // True if we reach the end of the linked list (or if linked list is empty)
        if (!current_node->IsValid()) {
            // Current node is linked list HEAD
            if (previous_node == nullptr) {
                *current_node = HashTableContainer<Key, Value>(key, value);
                return std::make_pair(potential_index, false);
            } else if
        }
        */
    }
    // True iff we reach the end of a non-zero-length linked list. previous_node will not be nullptr.
    previous_node->SetNext(new HashTableContainer<Key, Value>(key, value));
    return std::make_pair(potential_index, false);
}

template<typename Key, typename Value>
std::pair<int, bool> HashTable<Key, Value>::InsertAt_(HashTableContainer<Key, Value> *destination_array, unsigned int array_size, HashTableContainer<Key, Value>&& source) {
    // Internal function. Does not verify inputs. (array_size being 0, destination_array being nullptr...)
    unsigned int potential_index = GetPotentialIndexUnsized(source.GetKey(), array_size);
    HashTableContainer<Key, Value>* current_node = &destination_array[potential_index];
    HashTableContainer<Key, Value>* previous_node = nullptr;

    while (current_node != nullptr) {
        if (current_node->GetKey() == source.GetKey() || !current_node->IsValid()) {
            // Override previous value if node with same key already exists,
            //  or if an invalid node that can contain the new node is found.
            bool node_was_valid = current_node->IsValid();
            *current_node = std::move(source);
            return std::make_pair(potential_index, node_was_valid);
        }

        previous_node = current_node;
        current_node = current_node->GetNext();
        /* redesign rendered this unnecessary
        // True if we reach the end of the linked list (or if linked list is empty)
        if (!current_node->IsValid()) {
            // Current node is linked list HEAD
            if (previous_node == nullptr) {
                *current_node = HashTableContainer<Key, Value>(key, value);
                return std::make_pair(potential_index, false);
            } else if
        }
        */
    }
    // True iff we reach the end of a non-zero-length linked list. previous_node will not be nullptr.
    previous_node->SetNext(new HashTableContainer<Key, Value>(source));
    return std::make_pair(potential_index, false);
}

// As this is an internal function, it is assumed that the index and depth values are already verified.
template<typename Key, typename Value>
void HashTable<Key, Value>::DeleteAt_(int index, int depth) {
    HashTableContainer<Key, Value>* head_node = this->Get(index);
    HashTableContainer<Key, Value>* current_node = head_node->GetIndex(depth);
    HashTableContainer<Key, Value>* next_node = current_node->GetNext();
    if (depth == 0) {
        // Node to delete is head of linked list. Need to swap with next item in list.
        // Note on the second term, next_node->IsValid(): This theoretically
        //  should never be false and have the first condition be true. I'm
        //  leaving it here anyway just in case.
        if (next_node != nullptr && next_node->IsValid()) {
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
std::pair<int, int> HashTable<Key, Value>::Find_(const Key &key) {
    //if (this->capacity() == 0) return std::make_pair(-1, -1); // State validation should occur in public functions
    int potential_index = GetPotentialIndex_(key);
    int depth = 0;
    HashTableContainer<Key,Value>* current_node = this->Get(potential_index);
    while (current_node != nullptr && current_node->IsValid()) {
        if (current_node->GetKey() == key) {return std::make_pair(potential_index, depth);}
        current_node = current_node->GetNext();
        depth++;
    }
    return std::make_pair(-1, -1);
}

template<typename Key, typename Value>
int HashTable<Key, Value>::GetPotentialIndex_(const Key &key) {
    if (capacity_ == 0) {return 0;}
    //else
    return this->GetPotentialIndexUnsized(key, this->capacity());
}

template <typename Key, typename Value>
int HashTable<Key, Value>::GetPotentialIndexUnsized(const Key &key, unsigned int table_capacity)
{
    return hasher_(key) % table_capacity;
}

template<typename Key, typename Value>
Iterator<Key, Value> HashTable<Key, Value>::begin() const {
    return Iterator<Key, Value>(*this);
}

template<typename Key, typename Value>
Iterator<Key, Value> HashTable<Key, Value>::end() const {
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
HashTable<Key, Value> & HashTable<Key, Value>::operator=(const HashTable &other) {
    if (this == &other) {return *this;}
    size_ = other.size();
    capacity_ = other.capacity();
    UpdateLoadFactor_();
    container_array_ = new HashTableContainer<Key, Value>[capacity()];
    memcpy(container_array_, other.container_array_, capacity() * sizeof(HashTableContainer<Key, Value>));
    for (int i = 0; i < capacity(); i++) {
        HashTableContainer<Key, Value>* current_node = this->Get(i);
        for (int depth = 0;; depth++) {
         if (current_node->IsValid() && current_node->GetNext() != nullptr) {
             HashTableContainer<Key, Value>* next_node = current_node->GetNext();
             HashTableContainer<Key, Value>* new_node = new HashTableContainer<Key, Value>(*next_node);
             current_node->SetNext(new_node);
             current_node = new_node;
         }   else {
             break;
         }
        }
    }
    return *this;
}

template<typename Key, typename Value>
HashTable<Key, Value> & HashTable<Key, Value>::operator=(HashTable &&other) noexcept {
    this->~HashTable();
    this->container_array_ = other.container_array_;
    other.container_array_ = nullptr;
    capacity_ = other.capacity();
    size_ = other.size();
    other.capacity_ = 0;
    other.size_ = 0;
    UpdateLoadFactor_();
    return *this;
}

template<typename Key, typename Value>
int HashTable<Key, Value>::FindValidNode_(int start_index) const {
    if (start_index < 0 || start_index >= this->capacity()) {return -1;}
    while (this->Get(start_index) != nullptr) {
        if (this->Get(start_index)->IsValid()) {
            return start_index;
        }
        start_index++;
    }
    // Loop breaks if a valid node is found. If no valid nodes are found, return -1.
    return -1;
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
HashTableContainer<Key, Value> * HashTable<Key, Value>::Get(int index, int depth) const {
    if (index < 0 || index >= this->capacity()) return nullptr;
    HashTableContainer<Key, Value>* current_node = &container_array_[index];
    for (int i = 0; i < depth; i++) {
        current_node = current_node->GetNext();
    }
    return current_node;
}

template<typename Key, typename Value>
Iterator<Key, Value>::Iterator(const HashTable<Key, Value>& main_table, HashTableContainer<Key, Value> *current_node) : main_table_(main_table) {
    if (main_table.size() == 0 || current_node == nullptr) {
        index_ = -1;
        current_node_ = nullptr;
        return;
    }
    current_node_ = current_node;
}

template<typename Key, typename Value>
Iterator<Key, Value>::Iterator(const HashTable<Key, Value> &main_table) : main_table_(main_table) {
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
std::pair<const Key&, const Value&> Iterator<Key, Value>::operator*() const {
    if (current_node_ == nullptr) {throw std::out_of_range("Error, dereferencing invalid iterator");}
    return std::pair<const Key&,const Value&>(current_node_->GetKey(), current_node_->GetValue());
}

template<typename Key, typename Value>
bool Iterator<Key, Value>::operator!=(const Iterator<Key, Value> &right) const {
    if (this == &right) return false;
    if (this->current_node_ == right.current_node_
        && this->index_ == right.index_
        && this->main_table_.container_array_ == right.main_table_.container_array_) return false;
    return true;
}

template<typename Key, typename Value>
bool Iterator<Key, Value>::operator==(const Iterator<Key, Value> &right) const {
    return !(this->operator!=(right));
}
#endif // !HASH_TABLE_H
