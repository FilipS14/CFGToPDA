#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <queue>
#include <ranges>
#include <random>

class Grammar
{
public:
	//CONSTRUCOTR && DESSTRUCTOR
	Grammar() = default;
	~Grammar() = default;

	//GETTERS
	std::vector<char> getVn() const noexcept;
	std::vector<char> getVt() const noexcept;
	std::vector<std::pair<std::string, std::string>> getProduction() const noexcept;
	char getStart() const noexcept;

	//SETTERS
	void setVn(std::vector<char> Vn) noexcept;
	void setVt(std::vector<char> Vt) noexcept;
	void setStart(char Start) noexcept;
	void setProduction(std::vector<std::pair<std::string, std::string>>Production) noexcept;

	//OVERLOADING OPERATORS
	friend std::ostream& operator<<(std::ostream& cout, Grammar& G);
	friend std::istream& operator>>(std::istream& cin, Grammar& G);

	//CHECK GRAMMAR
	bool verifyGrammar() noexcept;
	bool intersectionVnWithVt() noexcept;//1) VN intersecat VT = vida 
	bool isStartInVn() noexcept;//2) Start apartine Vn;
	bool hasNonterminalOnLeft() noexcept;//3) pentru fiecare regulă, membrul stâng conține cel puțin un neterminal 
	bool hasSProduction() noexcept;//4) există cel puțin o producție care are în stânga doar S 
	bool onlyElementFromVnAndVt() noexcept; //5) fiecare producție conține doar elemente din VN și VT

	//CHECK IDC && REGULAR
	bool isRegular() noexcept;
	bool isIDC() noexcept;

	//SIMPLIFYING GRAMMAR
	void simplifyGrammar();
	//Eliminarea neterminalelor care nu generează cuvinte
	void eliminateNonGeneratingNonterminals(std::vector<char>& NewVn, std::vector<std::pair<std::string, std::string>>& NewProductions);
	//Eliminarea simbolurilor inaccesibile
	void eliminateInaccessibleNonterminals(std::vector<char>& NewVn, std::vector<std::pair<std::string, std::string>>& NewProductions);
	//Eliminarea redenumirilor
	void eliminateRenamings(std::vector<char>& NewVn, std::vector<std::pair<std::string, std::string>>& NewProductions);
	//Eliminarea simbolurilor neutilizabile
	void eliminateUnusableNonterminals(std::vector<char>& NewVn, std::vector<std::pair<std::string, std::string>>& NewProductions);
	//Eliminarea redenumirilor
	void eliminateRenamings();
	//Functie in plus pentru eliminarea redenumirilor
	std::pair<std::string, std::string> finalProduction(std::pair<std::string, std::string>& production, std::vector<std::pair<std::string, std::string>>& renameProductions, std::vector<std::pair<std::string, std::string>>& visited);
	//Froma Normala Chomsky 
	char returnNonterminal();
	void chomskyNormalForm();
	//Eliminarea lambda productii 
	void eliminateLambdaProduction(std::vector<char>& NewVn, std::vector<std::pair<std::string, std::string>>& NewProduction);
	
	//Forma Normala Graibach
	void lema2(int indexProduction, int indexNonterminal);
	void lema1(int indexProduction, int indexNonterminal);
	bool isApplicableLema1ForStep2(const char& VnLeft, const char& VnRight);
	bool isApplicableLema1(const char& VnLeft, const char& VnRight);
	int getIndexNonTerminal(const char& Vn);
	void transformGrammarToFNG();

	//READ && PRINT
	void readGrammar() noexcept;
	void printGrammar() noexcept;

	//GENERATE WORD
	std::string generateWord();

	//Getter
	char getLambda() const;
private:
	char Z = 'X';
	char Zanterior = 'X';
	const char m_lambda = '@';
	std::vector<char>m_Vn, m_Vt;
	std::vector<std::pair<std::string, std::string>>m_Production;
	char m_Start;
};

