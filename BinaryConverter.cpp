#include "provided.h"
#include <string>
using namespace std;

const size_t BITS_PER_UNSIGNED_SHORT = 16;



string convertNumberToBitString(unsigned short number);
bool convertBitStringToNumber(string bitString, unsigned short& number);

string BinaryConverter::encode(const vector<unsigned short>& numbers)
{
    //Pop the numbers from the vector and user convertNumberToBitString
    //Take the resulting binary numbers (in string form) and convert to tabs and spaces
    string encoded;
    for (int n = 0; n < numbers.size(); n++) {
        string temp = convertNumberToBitString(numbers[n]);
        for (int o = 0; o < temp.size(); o++) {
            if (temp[o] == '0') //Convert 0 to a space
                temp[o] = ' ';
            else if (temp[o] == '1') //Convert 1 to a tab
                temp[o] = '\t';
        }
        encoded += temp;
    }
    return encoded;
}

bool BinaryConverter::decode(const string& bitString,
                             vector<unsigned short>& numbers) 
{
    string decode = bitString;
    for (int i = 0; i < decode.size(); i++) {
        if (decode[i] == ' ') //Convert a space to a 0
            decode[i] = '0';
        else if (decode[i] == '\t') //Convert a tab to a 1
            decode[i] = '1';
        else
            return false;
    }
    if (decode.size() % 16 != 0) //If the binary string is too long
        return false;
    //Checks if there was anything in numbers from before
    if (! numbers.empty()) {
        for (int k = 0; k < numbers.size(); k++)
            numbers.erase(numbers.begin() + k);
    }
    int j = 0;
    while (j + 16 <= decode.size()) { //only put numbers 16 digits long in the vector
        string temp = decode.substr(j, 16); //Binary numbers are a max 16 digits
        unsigned short n;
        convertBitStringToNumber(temp, n);
        numbers.push_back(n);
        j += 16; //Increment count so you go to next 16 characters
    }
    return true;
}

//These functions were provided by Professor Smallberg and Nachenberg
string convertNumberToBitString(unsigned short number)
{
    string result(BITS_PER_UNSIGNED_SHORT, '0');
    for (size_t k = BITS_PER_UNSIGNED_SHORT; number != 0; number /= 2)
    {
        k--;
        if (number % 2 == 1)
            result[k] = '1';
    }
    return result;
}

bool convertBitStringToNumber(string bitString, unsigned short& number)
{
    if (bitString.size() != BITS_PER_UNSIGNED_SHORT)
        return false;
    number = 0;
    for (size_t k = 0; k < bitString.size(); k++)
    {
        number *= 2;
        if (bitString[k] == '1')
            number++;
        else if (bitString[k] != '0')
            return false;
    }
    return true;
}
