#include <iostream>


template <typename KeyType, typename ValueType>
class HashTable {
public:
    HashTable(unsigned int numBuckets, unsigned int capacity) {
        m_table = new Node*[numBuckets]; //O(B) for constructing B Node*
        // Make pointers null
        for (int i = 0; i < numBuckets; i++) //O(B) for going through buckets and making sure the pointers are null
            m_table[i] = nullptr;
        m_capacity = capacity;
        m_size = numBuckets;
        m_occupied = 0;
        mostRecent = &leastRecent; //the recently written list is empty
    }
    ~HashTable() {
        for (int n = 0; n < m_size; n++) {
            Node *p = m_table[n];
            while (p != nullptr && p->bucket != nullptr) {
                Node* n = p->bucket;
                delete p; //Delete nodes front to back
                p = n; //Set temp to next node
            }
            if (p == nullptr)
                continue;
            delete p; //Delete the last node O(N)
        }
        delete [] m_table; //O(B)
    }
    bool isFull() const {
        return m_occupied >= m_capacity;
    }
    bool set(const KeyType& key, const ValueType& value, bool permanent = false) {
        unsigned int res = determineBucketFromKey(key);
        Node* temp = findKey(key, m_table[res]);         if (temp != nullptr) {
            temp->m_value = value;
            //If it was in the ordered list-so not permanent
            if (temp->orderPrev != nullptr || temp->orderNext != nullptr) {
                temp->orderPrev->orderNext = temp->orderNext;
                temp->orderNext->orderPrev = temp->orderPrev;
                temp->orderPrev = mostRecent;
                mostRecent->orderNext = temp;
                mostRecent = temp;
                temp->orderNext = nullptr;
            }
            return true;
        }
        else if (isFull())
            return false;
        else {
            //Add a node to the right bucket
            Node* p = m_table[res];
            if (p == nullptr) { //if it is the first node in the bucket
                p = new Node;
                p->m_key = key;
                p->m_value = value;
                m_table[res] = p; //join the array and the new node
                if (! permanent) {
                    p->orderPrev = mostRecent; //Add to recently written list
                    mostRecent->orderNext = p;
                    mostRecent = p;
                }
            }
            else {
                while (p->bucket != nullptr) {
                p = p->bucket; //p goes to the last node
                }
            Node* q = new Node;
            p->bucket = q; //links last node to new node
                q->m_value = value;
                q->m_key = key;
                if (! permanent) {
                    q->orderPrev = mostRecent;
                    mostRecent->orderNext = q;
                    mostRecent = q;
                }
            }
        }
        m_occupied++;
        return true;
    }
    bool get(const KeyType& key, ValueType& value) const {
        unsigned int res = determineBucketFromKey(key);
        //Check to see if key is in bucket
        Node* temp = findKey(key, m_table[res]); //Only goes through O(C/B) buckets worst case
        if (temp != nullptr) {
            value = temp->m_value;
            return true;
        }
        else //If findKey returns nullptr, it could not find the key in the hash table
            return false;
    }
    bool touch(const KeyType& key) {
        unsigned int res = determineBucketFromKey(key); //Only checks C/B buckets
        Node* temp = findKey(key, m_table[res]);
        //Check if key is in bucket, and check if key is non-permanent
        if (temp != nullptr && (temp->orderPrev != nullptr || temp->orderNext != nullptr)) {
            if (mostRecent != temp) { //If it is not already most recent
                temp->orderPrev->orderNext = temp->orderNext;
                temp->orderNext->orderPrev = temp->orderPrev;
                mostRecent->orderNext = temp; //temp will be the new last node in the most recent list
                temp->orderPrev = mostRecent; //adjust prev pointers
                mostRecent = temp; //Temp moves up to most recent
                temp->orderNext = nullptr;
            }
            return true;
        }
        return false;
    }
    bool discard(KeyType& key, ValueType& value) {
         if (leastRecent.orderNext == nullptr) {
            return false;
        }
        Node *p = leastRecent.orderNext; //O(1)
        key = p->m_key;
        value = p->m_value;
        leastRecent.orderNext = p->orderNext;
        unsigned int res = determineBucketFromKey(key);
        if (p->orderNext != nullptr) {
            p->orderNext->orderPrev = p->orderPrev;
        }
        if (m_table[res] == p) {
            m_table[res] = p->bucket;
        }
        else {
            Node* b = m_table[res];
            while (b->bucket->m_key != key) //Looks through O(C/B) buckets
                b = b->bucket;
            b->bucket = p->bucket;
        }
        delete p;
        m_occupied--;
        return true;
    }
private:
    HashTable(const HashTable&);
    HashTable& operator=(const HashTable&);
    struct Node {
        Node() {
            orderNext = nullptr;
            orderPrev = nullptr;
            bucket = nullptr;
        }
        Node* bucket;
        Node* orderNext;
        Node* orderPrev;
        ValueType m_value;
        KeyType m_key;
    };
    int m_capacity;
    int m_occupied;
    Node** m_table;
    int m_size;
    Node leastRecent; //Dummy node
    Node* mostRecent;
    //Takes a key, determines its hash value and then determines which array slot's linkedlist to put it in
    unsigned int determineBucketFromKey(const KeyType& key) const{
        unsigned int computeHash(const KeyType&);
        unsigned int result = computeHash(key);
        unsigned int bucketNum = result % m_size;
        return bucketNum;
    }
    //Returns a pointer to the node with that key
    Node* findKey(const KeyType& key, Node* cur) const {
        while (cur != nullptr) { //goes through nodes in a certain bucket
            if (cur->m_key == key)
                return cur;
            cur = cur->bucket;
        }
        return nullptr;
    }
};
