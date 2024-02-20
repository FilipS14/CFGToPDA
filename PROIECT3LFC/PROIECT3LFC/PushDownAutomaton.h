#pragma once
#include "Grammar.h"
#include <tuple>
#include <unordered_map>
#include <functional>

namespace std {
	template <typename T>
	struct hash<std::tuple<T, T, T>> {
		size_t operator()(const std::tuple<T, T, T>& t) const {
			auto hash1 = std::hash<T>{}(std::get<0>(t));
			auto hash2 = std::hash<T>{}(std::get<1>(t));
			auto hash3 = std::hash<T>{}(std::get<2>(t));
			return hash1 ^ (hash2 << 1) ^ (hash3 << 2);
		}
	};
}

class PushDownAutomaton
{

private:
	std::vector<char> m_States; //multimea starilor: q0, q1
	std::vector<char> m_Alphabet; //alfabetul de intrare (contine toate literele)
	std::vector<char> m_StackState; //alfabetul de simboluri al memoriei push-down
	// S(q0, a, A) = (q1, AB)
	std::vector<std::tuple<char, char, char>> m_Transitions;
	std::vector<std::pair<char, std::string>> m_TransitionsResults;
	std::unordered_map < std::tuple<char, char, char>, std::vector<std::pair<char, std::string>>>m_FinalTransition;
	char m_InitialState; //stare initiala
	char m_InitialStackState; //simbol de start al memoriei push-down
	std::vector<char> m_FinalStates; // multimea starilor finale
	char m_lambda = '@';

public:
	// Constructor de copiere
	PushDownAutomaton(const PushDownAutomaton& other);

	// Operator de atribuire
	PushDownAutomaton& operator=(const PushDownAutomaton& other);

	//Constructors
	PushDownAutomaton() = default;
	PushDownAutomaton(std::vector<char> States, std::vector<char> Alphabet, std::vector<char> StackState,
		char InitialState, char InitialStackState, std::vector<char> FinalStates);

	//OVERLOADING PRINT OPERATOR
	friend std::ostream& operator<<(std::ostream& cout, const PushDownAutomaton& G);

	//Read & Print
	void ReadAutomaton();
	void PrintAutomaton();

	bool CheckWord(std::string word);
	bool CheckWordBacktraking( std::string word, int index, std::stack<char> states_stack);


	bool IsDeterministic();

	// Getters
	std::unordered_map < std::tuple<char, char, char>, std::vector<std::pair<char, std::string>>> getFinalTransition() const noexcept;
	std::vector<char> getStates() const noexcept;
	std::vector<char> getAlphabet() const noexcept;
	std::vector<char> getStackState() const noexcept;
	std::vector<std::tuple<char, char, char>> getTransitions() const noexcept;
	std::vector<std::pair<char, std::string>> getTransitionsResults() const noexcept;
	char getInitialState() const noexcept;
	char getInitialStackState() const noexcept;
	std::vector<char> getFinalStates() const noexcept;
	char getLambda() const noexcept;

	// Setters
	void setFinalTranstion(std::unordered_map < std::tuple<char, char, char>, std::vector<std::pair<char, std::string>>> FinalTransition);
	void setStates(const std::vector<char>& states);
	void setAlphabet(const std::vector<char>& alphabet);
	void setStackState(const std::vector<char>& stackState);
	void setTransitions(const std::vector<std::tuple<char, char, char>>& transitions);
	void setTransitionsResults(const std::vector<std::pair<char, std::string>>& transitionsResults);
	void setInitialState(char initialState);
	void setInitialStackState(char initialStackState);
	void setFinalStates(const std::vector<char>& finalStates);
};

