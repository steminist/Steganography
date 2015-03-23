#include "provided.h"
#include <string>
using namespace std;

bool Steg::hide(const string& hostIn, const string& msg, string& hostOut)
{
    if(hostIn.empty())
        return false;
    vector<string> intermediary; //Temporary vector to hold hidden strings
    int beg = 0; //Index to keep track of beginning of a substring
    int end; //Index to keep track of end of a substring
    //O(L)
    for (end = 0; end < hostIn.size(); end++) {
        if ( end+1<hostIn.size() && hostIn[end] == '\r' && hostIn[end+1] == '\n') { //Split by newlines
            intermediary.push_back(hostIn.substr(beg,end-beg));
            beg = end+2;
            end++;
            continue;
        }
        else if (hostIn[end] == '\n') {
            intermediary.push_back(hostIn.substr(beg,end-beg));
            beg = end + 1;
            continue;
        }
    }
    //Checks if string does not end in new line
    if (end == hostIn.size() && hostIn[end-1] != '\n') {
        intermediary.push_back(hostIn.substr(beg,end-beg));
    }
    //Look through vector
    for (int count = 0; count < intermediary.size(); count++) {
        //Look through characters in each string in vector
        string temp = intermediary[count];
        for (int end = temp.size() - 1; end >= 0; end--) { 
            if (temp[end] != ' ' && temp[end] != '\t') { //Substring is from end to before first non-space/non-tab
                intermediary[count] = temp.substr(0, end+1);
                break;
            }
            else {
                intermediary[count] = temp.substr(0,end);
            }
        }
    }
    vector<unsigned short> compressed;
    //Compress and convert msg
    Compressor::compress(msg, compressed);
    string t = BinaryConverter::encode(compressed);
    unsigned long L = t.size();
    unsigned long N = intermediary.size();
    //ap will go through the first L%N substrings
    int ap = 0;
    //Index for when to start the substring
    int first = 0;
    while (ap < L%N) {
        //Set the substring-starting index, length
        string substring = t.substr(first,(L/N)+1);
        //Add the substring to the end of intermediary
        intermediary[ap] += substring + '\n'; //Add a new line to every substring
        first += (L/N) + 1;
        ap++;
    }
    while (ap < N) { //Look through the rest of the substrings
        string substring = t.substr(first,L/N);
        intermediary[ap] += substring + '\n'; //Add a new line to every substring
        first += L/N;
        ap++;
    }
    for (int p = 0; p < intermediary.size(); p++) { //Append all the vector elements to the output string
        hostOut += intermediary[p];
    }
    return true;
}

bool Steg::reveal(const string& hostIn, string& msg)
{
    if(hostIn.empty())
        return false;
    vector<string> intermediary;
    int beg = 0; //Index for checking beginning of substring
    int end; //Index for checking end of substring
    //O(L)
    for (end = 0; end < hostIn.size(); end++) {
        if ( end+1<hostIn.size() && hostIn[end] == '\r' && hostIn[end+1] == '\n') { //Check for newlines
            intermediary.push_back(hostIn.substr(beg,end-beg));
            beg = end+2;
            end++;
            continue;
        }
        else if (hostIn[end] == '\n') { //Check for new lines
            intermediary.push_back(hostIn.substr(beg,end-beg));
            beg = end + 1;
            continue;
        }
    }
    //Checks if string does not end in new line
    if (end == hostIn.size() && hostIn[end-1] != '\n') {
        intermediary.push_back(hostIn.substr(beg,end-beg));
    }
    //Gather the trailing spaces and tabs
    string trailing;
    for (int count = 0; count < intermediary.size(); count++) {
        string temp = intermediary[count]; //Each string in vector
        for (int end = temp.size() - 1; end >= 0; end--) {
            if (temp[end] != ' ' && temp[end] != '\t') { //Stop when the spaces or tabs end
                if (end + 1 != temp.size()) {
                    trailing += temp.substr(end +1); //Substring should be from past first non-space/non-tab to end
                    break;
                }
                else {
                    trailing += temp;
                    break;
                }
            }
            else if (end == 0) {
                trailing += temp;
            }
        }
    }
    //Decode trailing
    vector<unsigned short> numbers;
    if (! BinaryConverter::decode(trailing, numbers)) {
        return false;
    }
    if (! Compressor::decompress(numbers, msg)) {
        return false;
    }
    return true;  
}
