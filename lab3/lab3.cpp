#include<iostream>
#include<vector>
#include<string>
#include<array>
#include <utility>
#include<algorithm>
#include<fstream>
#include<regex>

class ProgramInternalForm
{
    std::vector<std::pair<std::string, std::pair<int,int>>> pif;

    public:

    void add(std::string token, std::pair<int,int> position)
    {
        pif.push_back(std::pair<std::string, std::pair<int,int>>(token, position));
    }

    std::string toString()
    {
        std::string stringRepresentation;
        for(auto& tokenPositionPair : pif)
        {
            stringRepresentation += tokenPositionPair.first + ' ' +
                                    std::to_string(tokenPositionPair.second.first) + ' ' +
                                    std::to_string(tokenPositionPair.second.second) + '\n';
        }

        return stringRepresentation;
    }
};

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

    std::string toString()
    {
        std::string stringRepresentation;
        for(int i = 0; i<TABLE_SIZE; i++)
        {
            if(hashTable[i].size() == 0)
            {
                continue;
            }
            stringRepresentation += std::to_string(i) + ": ";
            for(auto& token : hashTable[i])
            {
                stringRepresentation += token + ' ';
            }

            stringRepresentation += '\n';
        }

        return stringRepresentation;
    }

};

std::vector<std::string> getProgramTokens(char* filename)
{
    std::ifstream programFile(filename);
    std::string programString;
    std::vector<std::string> programTokens;

    std::getline(programFile, programString, '\0');

    std::regex reg("((,)|(\\+)|(\\-)|(\\*)|(\\/)|(=)|(<)|(>)|(<=)|(==)|(>=)|(\\[)|(\\])|(\\()|(\\))|(;)|(\\{)|(\\})|(:)|(])|( ))");
    std::sregex_token_iterator srti(programString.begin(), programString.end(), reg, {-1, 0});
    
    std::remove_copy_if(srti, std::sregex_token_iterator(), 
                std::back_inserter(programTokens),
                [](std::string const &s) { return s.empty() || std::all_of(s.begin(),s.end(),isspace); });

    for( auto & p : programTokens )
    {
        p.erase(std::remove(p.begin(), p.end(), '\n'), p.cend());
    }

    return programTokens;
}

std::vector<std::string> getSpecialTokens(char* filename)
{
    std::ifstream specialTokensFile(filename);
    std::string specialTokensString;
    std::vector<std::string> specialTokens;


    while(std::getline(specialTokensFile, specialTokensString, '\n'))
    {
        specialTokens.push_back(specialTokensString);
        specialTokensString.clear();
    }

    return specialTokens;
}

bool isIdentifier(std::string token)
{
    return std::regex_match(token, std::regex("[a-zA-Z][a-zA-Z0-9_]*"));
}

bool isConstant(std::string token)
{
    return std::regex_match(token, std::regex("([1-9][0-9]*)|0")) ||
        std::regex_match(token, std::regex("\"[a-zA-Z0-9]*\""));
}

//Documentation: class diagram, regex explanation, pif data structure explanation
int main(int argc, char *argv[])
{
    ProgramInternalForm programInternalForm;
    SymbolTable symbolTable;

    if(argc != 3)
    {
        std::cout << "Two command line args must be provided. First the program then the tokens.\n";
        return -1;
    }
    
    std::vector<std::string> programTokens = getProgramTokens(argv[1]);
    std::vector<std::string> specialTokens = getSpecialTokens(argv[2]);

    for(auto const& token : programTokens)
    {
        if(std::find(specialTokens.begin(), specialTokens.end(), token) != specialTokens.end())
        {
            //std::cout << "'" << token << "' is special" << '\n';
            programInternalForm.add(token, std::pair<int,int>{-1,-1});
        }
        else if(isIdentifier(token))
        {
            //std::cout << "'" << token << "' is identifier" << '\n';
            auto position = symbolTable.add(token);
            programInternalForm.add("identifier", position);
        }
        else if(isConstant(token))
        {
            //std::cout << "'" << token << "' is constant" << '\n';
            auto position = symbolTable.add(token);
            programInternalForm.add("constant", position);
        }
        else
        {
            std::cout << "'" << token << "' is lexical error" << '\n';
        }
    }

    std::ofstream pifOutput("PIF.out");
    std::ofstream symbolTableOutput("ST.out");

    pifOutput << programInternalForm.toString();

    symbolTableOutput << "HashTable\n"<< symbolTable.toString();
}