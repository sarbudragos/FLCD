#include<iostream>
#include<vector>
#include<string>
#include<array>
#include <utility>
#include<algorithm>
#include<fstream>
#include<regex>
#include<map>
#include<sstream>

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

class FiniteAutomaton{

    std::vector<std::string> states;
    std::string initialState;
    std::vector<std::string> finalStates;
    std::vector<std::string> alphabet;
    std::map<std::string, std::vector<std::pair<std::string, std::string>>> transitionFunction;

    void readFromFile(std::string fileName){
        std::fstream file(fileName);
        std::string lineBuffer;
        std::string tokenBuffer;
        std::getline(file, lineBuffer);
        std::stringstream bufferStream(lineBuffer);
        std::string state1, transition, state2;

        while(bufferStream >> tokenBuffer){
            states.push_back(tokenBuffer);
        }

        lineBuffer.clear();
        std::getline(file, lineBuffer);
        initialState = lineBuffer;

        lineBuffer.clear();
        std::getline(file, lineBuffer);
        bufferStream = std::stringstream(lineBuffer);
        while(bufferStream >> tokenBuffer){
            finalStates.push_back(tokenBuffer);
        }

        lineBuffer.clear();
        std::getline(file, lineBuffer);
        bufferStream = std::stringstream(lineBuffer);
        while(bufferStream >> tokenBuffer){
            alphabet.push_back(tokenBuffer);
        }

        while(std::getline(file, lineBuffer)){
            bufferStream = std::stringstream(lineBuffer);

            bufferStream >> state1 >> transition >> state2;

            transitionFunction[state1].push_back(std::make_pair(transition, state2));
        }

    }

public:

    FiniteAutomaton(std::string fileName){
        readFromFile(fileName);
    }

    std::vector<std::string> getStates(){
        return states;
    }

    std::string getInitialState(){
        return initialState;
    }

    std::vector<std::string> getFinalStates(){
        return finalStates;
    }

    std::vector<std::string> getAlphabet(){
        return alphabet;
    }

    std::map<std::string, std::vector<std::pair<std::string, std::string>>> getTransitionFunction(){
        return transitionFunction;
    }
    

    bool isAccepted(std::vector<std::string> input){
        std::string currentState = initialState;
        
        for(int i=0; i<input.size();i++){
            auto currentTransitionList = transitionFunction[currentState];
            auto nextTransition = std::find_if(currentTransitionList.begin(), 
                currentTransitionList.end(), 
                [&input, i](auto x){ 
                    return x.first == input[i];
                }
            );
            if(nextTransition == currentTransitionList.end()){
                return false;
            }
            currentState = (*nextTransition).second;
        }
        if(std::find(finalStates.begin(), finalStates.end(), currentState) != finalStates.end()){
            return true;
        }

        return false;
    }


};

std::vector<std::string> getProgramTokens(char* filename)
{
    std::ifstream programFile(filename);
    std::string programString;
    std::vector<std::string> programTokens;

    std::getline(programFile, programString, '\0');

    std::regex reg("((,)|(\\+)|(\\-)|(\\*)|(\\/)|(<=)|(==)|(>=)|(=)|(<)|(>)|(\\[)|(\\])|(\\()|(\\))|(;)|(\\{)|(\\})|(:)|(])|( ))");
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
    FiniteAutomaton identifierFiniteAutomaton("identifierFA.in");
    std::vector<std::string> v;
    for (auto c : token){
        v.push_back(std::string{c});
    }

    return identifierFiniteAutomaton.isAccepted(v);

    //return std::regex_match(token, std::regex("[a-zA-Z][a-zA-Z0-9_]*"));
}

bool isConstant(std::string token)
{
    FiniteAutomaton identifierFiniteAutomaton("integerConstantFA.in");
    std::vector<std::string> v;
    for (auto c : token){
        v.push_back(std::string{c});
    }

    return identifierFiniteAutomaton.isAccepted(v) ||
        std::regex_match(token, std::regex("\"[a-zA-Z0-9]*\""));

    //return std::regex_match(token, std::regex("([1-9][0-9]*)|0")) ||
        //std::regex_match(token, std::regex("\"[a-zA-Z0-9]*\""));
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