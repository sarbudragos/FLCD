#include<iostream>
#include<fstream>
#include<map>
#include<string>
#include<vector>
#include<algorithm>

class FiniteAutomaton
{
    std::string initialState;
    std::vector<std::string> finalStates;
    std::vector<std::string> alphabet;
    std::map<std::string, std::vector<std::pair<std::string, std::string>>> transitionFunction;
public:
    FiniteAutomaton(
        std::string initialState,
        std::vector<std::string> finalStates,
        std::vector<std::string> alphabet,
        std::map<std::string, std::vector<std::pair<std::string, std::string>>> transitionFunction
        )
    {
        this->initialState = initialState;
        this->finalStates = finalStates;
        this->alphabet = alphabet;
        this->transitionFunction = transitionFunction;
    }

    bool isAccepted(std::vector<std::string> input)
    {
        std::string currentState = initialState;
        
        for(int i=0; i<input.size();i++)
        {
            auto currentTransitionList = transitionFunction[currentState];
            auto nextTransition = std::find_if(currentTransitionList.begin(), 
                currentTransitionList.end(), 
                [&input, i](auto x)
                { 
                    return x.first == input[i];
                }
            );
            if(nextTransition == currentTransitionList.end())
            {
                return false;
            }
            currentState = (*nextTransition).second;
        }
        if(std::find(finalStates.begin(), finalStates.end(), currentState) != finalStates.end())
        {
            return true;
        }

        return false;
    }
};

int main()
{

}