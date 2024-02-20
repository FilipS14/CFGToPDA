#include "PushDownAutomaton.h"


PushDownAutomaton::PushDownAutomaton(std::vector<char> States, std::vector<char> Alphabet, std::vector<char> StackState,
	char InitialState, char InitialStackState, std::vector<char> FinalStates)
	: m_States{ States },
	m_Alphabet{ Alphabet },
	m_StackState{ StackState },
	m_InitialState{ InitialState },
	m_InitialStackState{ InitialStackState },
	m_FinalStates{ FinalStates } 
{}

std::ostream& operator<<(std::ostream& cout, const PushDownAutomaton& G)
{
    cout << '\n';
    cout << "|AUTOMAT PUSH DOWN|\n";
    cout << "Multimea starilor (Q): ";
    for (const auto& state : G.getStates())
    {
        cout << state << ' ';
    }

    cout << '\n';
    cout << "Alfabetul de intrare: ";
    for (const auto& symbol : G.getAlphabet())
    {
        cout << symbol << " ";
    }

    cout << '\n';
    cout << "Alfabetul memoriei push-down: ";

    for (const auto& symbol : G.getStackState())
    {
        cout << symbol << " ";
    }

    cout << '\n';
    cout << "Starea initiala: " << G.getInitialState() << '\n';
    cout << "Starea initiala stiva: " << G.getInitialStackState() << '\n';

    cout << "Multimea starilor finale: ";
    for (const auto& symbol : G.getFinalStates())
    {
        cout << symbol << ' ';
    }

    cout << "\nFunctiile de tranzitie:\n";

    for (const auto& transition : G.getFinalTransition()) {
        const auto& initialState = std::get<0>(transition.first);
        const auto& inputSymbol = std::get<1>(transition.first);
        const auto& stackTop = std::get<2>(transition.first);

        const auto& finalStatesAndStackAdditions = transition.second;

        for (const auto& finalStateAndStackAddition : finalStatesAndStackAdditions) {
            const auto& finalState = finalStateAndStackAddition.first;
            const auto& stackAddition = finalStateAndStackAddition.second;

            cout << "(" << initialState << ", " << inputSymbol << ", " << stackTop << ") -> ";
            cout << "(" << finalState << ", " << stackAddition << ")\n";
        }
    }

    return cout;
}

void PushDownAutomaton::PrintAutomaton()
{
	std::cout << *this;
}

bool PushDownAutomaton::CheckWord(std::string word)
{
    for (auto& symbol : word)
    {
        if (std::find(m_Alphabet.begin(), m_Alphabet.end(), symbol) == m_Alphabet.end())
            return false;
    }
    char currentState;
    std::stack<char> stackState;
    stackState.push(m_InitialState);
    currentState = m_InitialState;
    bool ok = false;
    return CheckWordBacktraking( word,0 ,stackState);
}

bool PushDownAutomaton::CheckWordBacktraking(std::string word, int index, std::stack<char> stackState)
{
    if (index == word.size() && stackState.empty())
    {
        return true;
    }
    if (index >= word.size())
        return false;
    std::vector<std::pair<char, std::string>> result = m_FinalTransition[{m_InitialStackState, word[index], stackState.top()}];
    stackState.pop();
    for (auto it = result.begin(); it != result.end(); it++)
    {
        int n = it->second.size();
        for (int i = n - 1; i >= 0; i--)
        {
            if (it->second[i] == m_lambda)
                continue;
            stackState.push(it->second[i]);
        }
        bool valide= CheckWordBacktraking(word, index + 1 ,stackState);
        if (valide == true)
            return true;
        for (int i = n - 1; i >= 0; i--)
        {
            if (it->second[i] == m_lambda)
                continue;
            stackState.pop();
        }
    }
}

//Check Determinist
bool PushDownAutomaton::IsDeterministic()
{
    //std::unordered_map < std::tuple<char, char, char>, std::vector<std::pair<char, std::string>>>m_FinalTransition;
    //first check
    for (auto& component : m_FinalTransition)
    {
        if (component.second.size() > 1) {
            return false;
        }
    }
    //second check
    bool found = false;
    char state;
    char stackState;
    for (const auto& componet : m_FinalTransition)
    {
        std::tuple<char, char, char>transition = componet.first;
        //imi iau prima componenta  verific daca char ul de pe a doua pozitie de  lambda δ(q,λ,Z)
        if (std::get<1>(transition) == m_lambda)
        {
            state = std::get<0>(transition);
            stackState = std::get<2>(transition);
            //acum caut o alta tranzitie de forma δ(q,a,Z) cu acleasi q si Z de mai sus
            for (const auto& componetSecond : m_FinalTransition){
                if (componetSecond == componet)
                    continue;
                char stateCurrentTransition = std::get<0>(componetSecond.first);
                char stackStateCurrentTransition = std::get<2>(componetSecond.first);
                //daca exista inseamna ca nu este determinist
                if (state == stateCurrentTransition &&
                    stackState == stackStateCurrentTransition)
                    return false;
            }
        }
    }
    return true;
}

// Operator de atribuire
PushDownAutomaton& PushDownAutomaton::operator=(const PushDownAutomaton& other) {
    if (this != &other) {  
        m_States = other.m_States;
        m_Alphabet = other.m_Alphabet;
        m_StackState = other.m_StackState;
        m_InitialState = other.m_InitialState;
        m_InitialStackState = other.m_InitialStackState;
        m_FinalStates = other.m_FinalStates;

       
        m_FinalTransition.clear();
        for (const auto& entry : other.m_FinalTransition) {
            auto& targetEntry = m_FinalTransition[entry.first];
            targetEntry = entry.second;
        }
    }
    return *this;
}


//Getters
std::unordered_map < std::tuple<char, char, char>, std::vector<std::pair<char, std::string>>> PushDownAutomaton::getFinalTransition() const noexcept
{
    return m_FinalTransition;
}

std::vector<char> PushDownAutomaton::getStates() const noexcept {
	return m_States;
}

std::vector<char> PushDownAutomaton::getAlphabet() const noexcept {
	return m_Alphabet;
}

std::vector<char> PushDownAutomaton::getStackState() const noexcept {
	return m_StackState;
}

std::vector<std::tuple<char, char, char>> PushDownAutomaton::getTransitions() const noexcept {
	return m_Transitions;
}

std::vector<std::pair<char, std::string>> PushDownAutomaton::getTransitionsResults() const noexcept {
	return m_TransitionsResults;
}

char PushDownAutomaton::getInitialState() const noexcept {
	return m_InitialState;
}

char PushDownAutomaton::getInitialStackState() const noexcept {
	return m_InitialStackState;
}

std::vector<char> PushDownAutomaton::getFinalStates() const noexcept {
	return m_FinalStates;
}

char PushDownAutomaton::getLambda() const noexcept {
	return m_lambda;
}


//Setters
void PushDownAutomaton::setFinalTranstion(std::unordered_map < std::tuple<char, char, char>, std::vector<std::pair<char, std::string>>> FinalTransition)
{
    m_FinalTransition = FinalTransition;
}

void PushDownAutomaton::setStates(const std::vector<char>& states) {
	m_States = states;
}

void PushDownAutomaton::setAlphabet(const std::vector<char>& alphabet) {
	m_Alphabet = alphabet;
}

void PushDownAutomaton::setStackState(const std::vector<char>& stackState) {
	m_StackState = stackState;
}

void PushDownAutomaton::setTransitions(const std::vector<std::tuple<char, char, char>>& transitions) {
	m_Transitions = transitions;
}

void PushDownAutomaton::setTransitionsResults(const std::vector<std::pair<char, std::string>>& transitionsResults) {
	m_TransitionsResults = transitionsResults;
}

void PushDownAutomaton::setInitialState(char initialState) {
	m_InitialState = initialState;
}

void PushDownAutomaton::setInitialStackState(char initialStackState) {
	m_InitialStackState = initialStackState;
}

void PushDownAutomaton::setFinalStates(const std::vector<char>& finalStates) {
	m_FinalStates = finalStates;
}