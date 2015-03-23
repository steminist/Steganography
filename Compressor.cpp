#include "provided.h"
#include "HashTable.h"
#include <string>
#include <vector>

//Function provided by Professor Carey Nachenberg
unsigned int computeHash(const std::string &key) {
    unsigned int total = 0;
    for (int i = 0; i < key.length(); i++)
        total += (i + 1) * key[i];
    return total;
}

unsigned int computeHash(const unsigned short &key) {
    return key;
}

void Compressor::compress(const std::string& s, std::vector<unsigned short>& numbers)
{
    unsigned int capacity = s.size()/2 + 512;
    if (capacity > 16384)
        capacity = 16384;
    numbers.erase(numbers.begin(), numbers.begin() + numbers.size()); //Erases all the elements in the vector
    unsigned short bucket = capacity/0.5;
    HashTable<std::string, unsigned short> H(bucket,capacity);
    for (unsigned short j = 0; j < 256; j++) {
        char c = static_cast<char>(j);
        std::string temp(1,c);
        H.set(temp, j, true);
    }
    unsigned short nextFreeID = 256;
    std::string runSoFar;
    for (int c = 0; c < s.size(); c++) {
        std::string expandedRun = runSoFar + s[c];
        unsigned short x;
        if (H.get(expandedRun, x)) {
            runSoFar = expandedRun;
            continue;
        }
        else {
            H.get(runSoFar, x);
        }
        numbers.push_back(x);
        H.touch(runSoFar);
        runSoFar = "";
        unsigned short cv;
        std::string t(1, s[c]);
        H.get(t, cv);
        numbers.push_back(cv);
        if (! H.isFull()) {
            H.set(expandedRun, nextFreeID);
            nextFreeID++;
        }
        else {
            std::string done;
            unsigned short id;
            H.discard(done, id);
            H.set(expandedRun,id);
        }
    }
    if (! runSoFar.empty()) {
        unsigned short x;
        H.get(runSoFar, x);
        numbers.push_back(x);
    }
    numbers.push_back(capacity); //Append capacity so compressor knows how big to make hash table when decompressing
}

bool Compressor::decompress(const std::vector<unsigned short>& numbers, std::string& s)
{
    if (numbers.empty())
        return false;
    unsigned int capacity = numbers.back(); //Find the capacity based on last element in vector
    unsigned int bucket = capacity/0.5;
    HashTable<unsigned short, std::string> H(bucket,capacity);
    for (unsigned short j = 0; j < 256; j++) {
        char c = static_cast<char>(j);
        std::string temp(1,c);
        H.set(j,temp, true);
    }
    unsigned short nextFreeID = 256;
    std::string runSoFar;
    std::string output;
    for (int k = 0; k < numbers.size() - 1; k++) {
        unsigned short us = numbers[k];
        if (us <= 255) {
            std::string temp;
            H.get(us, temp);
            output += temp;
            std::string expandedRun;
            if (runSoFar.empty()) {
                runSoFar += temp;
                continue;
            }
            else {
                expandedRun = runSoFar + temp;
            }
            if ( ! H.isFull()) {
                H.set(nextFreeID, expandedRun);
                nextFreeID++;
            }
            else {
                unsigned short temp;
                std::string b;
                H.discard(temp, b);
                H.set(temp, expandedRun);
            }
            runSoFar = "";
            continue;
        }
        else {
            std::string useless;
            if (! H.get(us, useless)) {
                return false;
            }
            else {
                H.touch(us);
            }
            std::string append;
            H.get(us, append);
            output += append;
            runSoFar = append;
            continue;
        }
    }
    s = output;
    return true;
}
