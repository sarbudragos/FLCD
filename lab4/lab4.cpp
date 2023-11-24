#include<iostream>
#include<fstream>
#include<map>
#include<string>
#include<vector>
#include<algorithm>
#include<sstream>

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

int main(){
    FiniteAutomaton finiteAutomaton("FA.in");

    std::cout << "Please input:\n"
    << "1 - states\n"
    << "2 - initial state\n"
    << "3 - final states\n"
    << "4 - alphabet\n"
    << "5 - transition function\n"
    << "6 - verify\n";

    int option;

    while(std::cin >> option){
        switch (option)
        {
        case 1:
            for (auto x : finiteAutomaton.getStates()){
                std::cout << x << "\n";
            }
            break;
        case 2:
            std::cout << finiteAutomaton.getInitialState() << "\n";
            break;
        case 3:
            for (auto x : finiteAutomaton.getFinalStates()){
                std::cout << x << "\n";
            }
            break;
        case 4:
            for (auto x : finiteAutomaton.getAlphabet()){
                std::cout << x << "\n";
            }
            break;
        case 5:
            for(auto x: finiteAutomaton.getTransitionFunction()){
                for(auto y: x.second){
                    std::cout << x.first << " " << y.first << " " << y.second << "\n";
                }
            }
            break;
        case 6:
            std::cout << finiteAutomaton.isAccepted(std::vector<std::string>({"a", "0", "1","_","1", "1"})) << "\n";
            break;
        default:
            std::cout << "Please input a valid option.\n";
            break;
        }
    }
}