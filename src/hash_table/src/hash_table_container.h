//
// Created by ethan on 10/27/2025.
//

#ifndef HASH_TABLE_CONTAINER_H
#define HASH_TABLE_CONTAINER_H

template <typename Key, typename Value>
class HashTableContainer {
public:
    HashTableContainer();  // Constructor
    ~HashTableContainer(); // Destructor
    HashTableContainer(HashTableContainer<Key, Value>& other); // Copy constructor
    HashTableContainer(Key& key, Value& value, HashTableContainer<Key, Value>* next_ptr = nullptr);  // Parameterized constructor
    HashTableContainer<Key, Value>& operator=(const HashTableContainer<Key, Value>& right); // Copy Assignment

    Key& GetKey();
    void SetKey(const Key& key);

    bool IsValid() const;
    void SetValidity(bool valid);
    void SetValid();
    void SetInvalid();

    Value& GetValue() const;
    void SetValue(const Value& value);

    HashTableContainer<Key, Value>* GetNext() const;
    void SetNext(HashTableContainer<Key, Value>* next);

    HashTableContainer<Key, Value>* GetIndex(unsigned int index) const;
    
private:
    bool is_valid_;
    Key key_;
    Value value_;
    HashTableContainer<Key, Value>* next_node_;
};

template<typename Key, typename Value>
HashTableContainer<Key, Value>::HashTableContainer() {
    SetNext(nullptr);
    SetInvalid();
}

template<typename Key, typename Value>
HashTableContainer<Key, Value>::~HashTableContainer() {
    if (this->IsValid()) delete next_node_;
    // No need to verify if next_node_ is valid,
    //  delete automagically handles nullptrs.
}

template<typename Key, typename Value>
HashTableContainer<Key, Value>::HashTableContainer(HashTableContainer<Key, Value>& other) {
    *this = other;
}

template<typename Key, typename Value>
HashTableContainer<Key, Value>::HashTableContainer(Key &key, Value &value, HashTableContainer<Key, Value> *next_ptr) {
    this->SetNext(next_ptr);
    this->SetKey(key);
    this->SetValue(value);
    this->SetValid();
}

template<typename Key, typename Value>
HashTableContainer<Key, Value> & HashTableContainer<Key, Value>::operator=(const HashTableContainer<Key, Value> &right) {
    if (this == &right) return *this;
    this->SetNext(right.GetNext());
    this->SetValue(right.GetValue());
    this->SetValidity(right.IsValid());
    return *this;
}

template<typename Key, typename Value>
Key & HashTableContainer<Key, Value>::GetKey() {
    return key_;
}

template<typename Key, typename Value>
void HashTableContainer<Key, Value>::SetKey(const Key &key) {
    key_ = key;
}

template<typename Key, typename Value>
bool HashTableContainer<Key, Value>::IsValid() const {
    return this->is_valid_;
}

template<typename Key, typename Value>
void HashTableContainer<Key, Value>::SetValidity(bool valid) {
    this->is_valid_ = valid;
}

template<typename Key, typename Value>
void HashTableContainer<Key, Value>::SetValid() {
    SetValidity(true);
}

template<typename Key, typename Value>
void HashTableContainer<Key, Value>::SetInvalid() {
    SetValidity(false);
}

template<typename Key, typename Value>
Value & HashTableContainer<Key, Value>::GetValue() const {
    return this->value_;
}

template<typename Key, typename Value>
void HashTableContainer<Key, Value>::SetValue(const Value &value) {
    this->value_ = value;
}

template<typename Key, typename Value>
HashTableContainer<Key, Value> * HashTableContainer<Key, Value>::GetNext() const {
    return this->next_node_;
}

template<typename Key, typename Value>
void HashTableContainer<Key, Value>::SetNext(HashTableContainer<Key, Value> *next) {
    next_node_ = next;
}

template<typename Key, typename Value>
HashTableContainer<Key, Value> * HashTableContainer<Key, Value>::GetIndex(unsigned int index) const {
    int temp_index = 0;
    HashTableContainer<Key, Value>* temp_node = this;
    while (temp_index != index) {
        if (temp_node == nullptr || !temp_node->IsValid()) return nullptr;
        temp_node = temp_node->GetNext();
        ++temp_index;
    }
    return temp_node;
}
#endif // !HASH_TABLE_CONTAINER_H
