#include<iostream>
#include<vector>
#include<string>
#include<array>
#include <utility>
#include<algorithm>

class SymbolTable
{
    static const int TABLE_SIZE = 100;
    std::array<std::vector<std::string>, TABLE_SIZE> hashTable;

    int hash(std::string token){
        int hash = 0;
        for(char c : token){
            hash += (int)c;
            hash %= TABLE_SIZE;
        }

        return hash;
    }

public:

    std::pair<int,int> add(std::string token){
        int hashValue = hash(token);
        auto result = std::find(hashTable[hashValue].begin(),hashTable[hashValue].end(), token);

        if (result != hashTable[hashValue].end())
        {
            return std::pair<int, int>(hashValue, result - hashTable[hashValue].begin());
        }

        hashTable[hashValue].push_back(token);
        size_t pos = hashTable[hashValue].size() - 1;
        return std::pair<int, int>(hashValue, pos);
    }

};

int main()
{
    std::cout<<"Hello";
}