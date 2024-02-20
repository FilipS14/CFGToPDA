#include "Grammar.h"

//READ && PRINT
void Grammar::readGrammar() noexcept {
    std::ifstream fin{ "fisier.txt" };
    if (!fin.is_open()) {
        std::cout << "Nu s-a putut deschide fisierul!\n";
    }

    {
        char word;
        uint32_t number;

        fin >> number;//citim numarul de elemente pentru multimea VN
        for (size_t i = 0; i < number; i++)
        {
            fin >> word;
            m_Vn.emplace_back(word);
        }

        fin >> number;//citim numarul de elemente pentru multimea VT
        for (size_t i = 0; i < number; i++)
        {
            fin >> word;
            m_Vt.emplace_back(word);
        }

        fin >> m_Start;//citim carcterul de start;


        std::string prod1, prod2;
        fin >> number;//citim numarul de productii;
        for (size_t i = 0; i < number; i++)
        {
            fin >> prod1 >> prod2;
            m_Production.push_back(std::make_pair(prod1, prod2));
        }
    }
    fin.close();
}

void Grammar::printGrammar() noexcept {
    for (const auto& word : m_Vn)
    {
        std::cout << word << ' ';
    }
    std::cout << '\n';

    for (const auto& word : m_Vt)
    {
        std::cout << word << ' ';
    }
    std::cout << '\n';

    std::cout << m_Start << '\n';

    for (const auto& word : m_Production)
    {
        std::cout << word.first << " -> " << word.second << '\n';
    }
}

//CHECK GRAMMAR
bool Grammar::intersectionVnWithVt() noexcept {
    std::vector<char> cVn(m_Vn.begin(), m_Vn.end()), cVt(m_Vt.begin(), m_Vt.end());
    std::vector<char>intersctionResult;
    std::sort(cVn.begin(), cVn.end());
    std::sort(cVt.begin(), cVt.end());

    auto it = std::set_intersection(cVn.begin(), cVn.end(), cVt.begin(), cVt.end(), std::back_inserter(intersctionResult));

    return !intersctionResult.empty();
}

bool Grammar::isStartInVn() noexcept {
    auto it = std::find(m_Vn.begin(), m_Vn.end(), m_Start);
    return it != m_Vn.end();
}

bool Grammar::hasNonterminalOnLeft() noexcept {
    for (const auto& word : m_Production)
    {
        bool ok{ false };
        for (const auto& element : word.first)
        {
            if (std::find(m_Vn.begin(), m_Vn.end(), element) != m_Vn.end()) {
                ok = true;
                break;
            }
        }
        if (ok == false)
            return false;
    }
    return true;
}

bool Grammar::hasSProduction() noexcept {
    for (const auto& word : m_Production) {
        if (word.first.size() == 1) {
            if (std::find(word.first.begin(), word.first.end(), m_Start) != word.first.end())
                return true;
        }
    }
    return false;
}

bool Grammar::onlyElementFromVnAndVt() noexcept {
    std::unordered_set<char> caractere;
    caractere.emplace('@');
    for (const auto& ch : m_Vn)
    {
        caractere.emplace(ch);
    }
    for (const auto& ch : m_Vt)
    {
        caractere.emplace(ch);
    }
    for (auto const& productie : m_Production)
    {
        for (auto const& letter : productie.first)
        {
            if (caractere.find(letter) == caractere.end())
                return false;
        }
        for (auto const& letter : productie.second)
        {
            if (caractere.find(letter) == caractere.end())
                return false;
        }
    }
    return true;
}

bool Grammar::verifyGrammar() noexcept {
    if (intersectionVnWithVt()) {
        return false;
    }

    if (!isStartInVn()) {
        return false;
    }

    if (!hasNonterminalOnLeft()) {
        return false;
    }

    if (!hasSProduction()) {
        return false;
    }

    if (!onlyElementFromVnAndVt()) {
        return false;
    }

    std::cout << "Grammar verification successful.\n";
    return true;
}

//CHECK IDC && REGULAR
bool Grammar::isRegular() noexcept {
    if (!verifyGrammar())
        return false;

    for (const auto& production : m_Production) {
        if (production.first.size() != 1 || production.second.size() > 2)
            return false;

        if (production.first.at(0) != m_Start && production.second.at(0) == m_lambda)
            return false;

        if (std::find(m_Vn.begin(), m_Vn.end(), production.first.at(0)) == m_Vn.end())
            return false;

        if (std::find(m_Vt.begin(), m_Vt.end(), production.second.at(0)) == m_Vt.end())
            return false;

        if (production.second.size() == 2) {
            if (production.second.at(0) == m_lambda ||
                std::find(m_Vn.begin(), m_Vn.end(), production.second.at(1)) == m_Vn.end()) {
                return false;
            }
        }
    }

    return true;
}

bool Grammar::isIDC() noexcept {
    if (!verifyGrammar())
        return false;
    for (const auto& production : m_Production) {
        if (production.first.size() != 1)
            return false;
        
        if (production.first.at(0) == m_Start && production.second.size() == 1 && production.second.at(0) == m_lambda)
            continue;
        
        if (production.first.at(0) != m_Start && production.second.at(0) == m_lambda && production.second.size() > 1)
            return false;

        if (std::find(m_Vn.begin(), m_Vn.end(), production.first.at(0)) == m_Vn.end())
            return false;

        for (const auto& symbol : production.second)
        {
            if (production.second.at(0) == m_lambda)
                continue;
            if (symbol == m_lambda)
                return false;
            if (std::find(m_Vn.begin(), m_Vn.end(), symbol) == m_Vn.end() &&
                std::find(m_Vt.begin(), m_Vt.end(), symbol) == m_Vt.end())
                return false;
        }
    }
    return true;
}

//SIMPLIFYING GRAMMAR
void Grammar::simplifyGrammar()
{
    if (!isIDC())
        throw std::logic_error("Grammar is not free context");
    std::vector<char> NewVn, VnBefore;
    std::vector<std::pair<std::string, std::string>> NewProductions;
    eliminateLambdaProduction(NewVn, NewProductions);
    eliminateRenamings();
    eliminateNonGeneratingNonterminals(NewVn, NewProductions);
    eliminateInaccessibleNonterminals(NewVn, NewProductions);
    printGrammar();
}

void Grammar::eliminateNonGeneratingNonterminals(std::vector<char>& NewVn, std::vector<std::pair<std::string, std::string>>& NewProductions)
{
    // Pas 1: V0 = vida
    std::vector<char> VnBefore;
    std::unordered_set<int> indiciProduction;
    NewVn.clear();

    do {
        // Pas 2: Vi = Vi-1 U {A | A->α ∈ P, α ∈ (Vi-1 U VT)*}
        VnBefore = NewVn;

        // Iterăm peste producții pentru a identifica neterminalele care pot genera cuvinte
        for (const auto& production : m_Production) {
            bool isGenerating = true;

            // Verificăm fiecare simbol din partea dreaptă a producției
            for (char symbol : production.second) {
                // Dacă simbolul nu este în VnBefore și nu este un terminal, producția nu generează cuvinte
                if ((std::find(VnBefore.begin(), VnBefore.end(), symbol) == VnBefore.end()) &&
                    (std::find(m_Vt.begin(), m_Vt.end(), symbol) == m_Vt.end())) {
                    isGenerating = false;
                    break;
                }
            }

            // Dacă producția generează cuvinte și neterminalul nu este încă în lista NewVn, îl adăugăm
            if (isGenerating && std::find(NewVn.begin(), NewVn.end(), production.first[0]) == NewVn.end()) {
                NewVn.push_back(production.first[0]);
            }
        }
    } while (NewVn != VnBefore);

    // Pas 3: P1 = {A->α | A ∈ V1N și α ∈ (V1N U VT)*}
    NewProductions.clear();
    // Iterăm peste producții pentru a construi noile producții și neterminale
    for (const auto& production : m_Production) {
        bool isValid = true;

        // Verificăm dacă neterminalul din partea stângă a producției este în NewVn
        for (char symbol : production.first) {
            if (std::find(NewVn.begin(), NewVn.end(), symbol) == NewVn.end()) {
                isValid = false;
                break;
            }
        }

        // Verificăm fiecare simbol din partea dreaptă a producției
        for (char symbol : production.second) {
            // Dacă simbolul nu este în NewVn și nu este un terminal, producția nu este validă
            if ((std::find(NewVn.begin(), NewVn.end(), symbol) == NewVn.end()) &&
                (std::find(m_Vt.begin(), m_Vt.end(), symbol) == m_Vt.end())) {
                isValid = false;
                break;
            }
        }

        // Dacă producția este validă, o adăugăm la lista NewProductions
        if (isValid) {
            NewProductions.push_back(production);
        }
    }
    if (NewProductions.size() != 0 && NewVn.size() != 0) {
        m_Vn.clear();
        m_Production.clear();
        m_Vn = NewVn;
        m_Production = NewProductions;
    }
}

void Grammar::eliminateInaccessibleNonterminals(std::vector<char>& NewVn, std::vector<std::pair<std::string, std::string>>& NewProductions)
{
    // Pasul 1: Găsește neterminalele accesibile
    NewProductions.clear();
    NewVn.clear();
    std::unordered_set<char> accessibleNonterminals;
    std::queue<char> nonterminalQueue;

    nonterminalQueue.push(m_Start);
    accessibleNonterminals.insert(m_Start);

    while (!nonterminalQueue.empty()) {
        char currentNonterminal = nonterminalQueue.front();
        nonterminalQueue.pop();

        // Parcurge producțiile pentru neterminalul curent
        for (const auto& production : m_Production) {
            if (production.first[0] == currentNonterminal) {
                // Adaugă neterminalele din partea dreaptă a producției în coadă și în setul de neterminale accesibile
                for (char symbol : production.second) {
                    if (std::find(m_Vn.begin(), m_Vn.end(), symbol) != m_Vn.end() &&
                        accessibleNonterminals.find(symbol) == accessibleNonterminals.end()) {
                        nonterminalQueue.push(symbol);
                        accessibleNonterminals.insert(symbol);
                    }
                }
            }
        }
    }

    // Pasul 2: Elimină neterminalele inaccesibile

    for (const auto& production : m_Production) {
        // Adaugă producțiile care au neterminale accesibile în lista de producții nouă
        if (accessibleNonterminals.find(production.first[0]) != accessibleNonterminals.end()) {
            NewProductions.push_back(production);
        }
    }

    // Actualizează lista de neterminale cu cele accesibile
    NewVn.assign(accessibleNonterminals.begin(), accessibleNonterminals.end());

    // Actualizează gramatica
    if (NewVn.size() != 0 && NewProductions.size() != 0)
    {
        m_Vn.clear();
        m_Production.clear();
        m_Vn = NewVn;
        m_Production = NewProductions;

    }
}

void Grammar::eliminateUnusableNonterminals(std::vector<char>& NewVn, std::vector<std::pair<std::string, std::string>>& NewProductions)
{
    NewProductions = m_Production;
    NewVn = m_Vn;

    for (const auto& production : m_Production)
    {
        for (const auto& symbol : production.first)
        {
            bool isSymbolUsed = false;
            for (const auto& secondProduction : m_Production)
            {
                if (std::find(secondProduction.second.begin(), secondProduction.second.end(), symbol) != secondProduction.second.end()) {
                    isSymbolUsed = true;
                    break;
                }
            }

            if (!isSymbolUsed)
            {
                auto it = std::remove_if(NewProductions.begin(), NewProductions.end(),
                    [production](const std::pair<std::string, std::string>& prod) {
                        return prod == production;
                    });
                NewProductions.erase(it, NewProductions.end());
            }
        }
    }
}

std::pair<std::string, std::string> Grammar::finalProduction(std::pair<std::string, std::string>& production, std::vector<std::pair<std::string, std::string>>& renameProductions, std::vector<std::pair<std::string, std::string>>& visited)
{
    for (auto& prod : renameProductions)
    {
        if (prod.first == production.second && std::find(visited.begin(), visited.end(), prod) == visited.end()) {
            return finalProduction(prod, renameProductions, visited);
        }
    }
    return production;
}

void Grammar::eliminateRenamings() {
    std::vector<std::pair<std::string, std::string>> renameProductions, renameProductionsCopy, NewProduction;
    std::vector<std::pair<std::string, std::string>>visited;
    // Identifică redenumirile și le adaugă în vectorul renameProductions
    for (const auto& production : m_Production) {
        if (production.second.size() == 1 && std::find(m_Vn.begin(), m_Vn.end(), production.second.at(0)) != m_Vn.end()) {
            renameProductions.emplace_back(production);
        }
    }

    renameProductionsCopy = renameProductions;

    while (!renameProductionsCopy.empty()) {
        std::vector<std::pair<std::string, std::string>> toRemove;

        for (auto& production : renameProductionsCopy) {
            if (std::find(visited.begin(), visited.end(), production) == visited.end()) {
                std::pair<std::string, std::string> prod = finalProduction(production, renameProductions, visited);

                if (!prod.first.empty() && !prod.second.empty()) {
                    visited.emplace_back(prod);
                    toRemove.emplace_back(prod);

                    for (auto& [left, right] : m_Production) {
                        if (left.at(0) == prod.second.at(0)) {
                            NewProduction.emplace_back(prod.first, right);
                        }
                    }
                    for (auto& [left, right] : NewProduction) {
                        auto it = std::find_if(m_Production.begin(), m_Production.end(),
                            [&](const auto& production) {
                                return production.first == left && production.second == right;
                            });

                        if (it == m_Production.end()) {
                            //daca nu exista adaugam
                            m_Production.emplace_back(left, right);
                        }
                    }


                }
            }
        }

        // Elimina productiile  din renameProductionsCopy
        for (const auto& removal : toRemove) {
            renameProductionsCopy.erase(std::remove(renameProductionsCopy.begin(), renameProductionsCopy.end(), removal), renameProductionsCopy.end());
        }
    }

    // Elimină regulile de redenumire din gramatica originală
    m_Production.erase(std::remove_if(m_Production.begin(), m_Production.end(),
        [&](const auto& production) {
            return production.second.size() == 1 && std::find(m_Vn.begin(), m_Vn.end(), production.second.at(0)) != m_Vn.end();
        }), m_Production.end());
}

char Grammar::returnNonterminal()
{
    char nonterminal = 'A';
    while (true) {
        nonterminal++;
        if (std::find(m_Vn.begin(), m_Vn.end(), nonterminal) == m_Vn.end()) {
            return nonterminal;
        }
    }
}

void Grammar::chomskyNormalForm()
{
    //pasul1
    eliminateRenamings();
    std::vector<std::pair<std::string, std::string>> newProductionsChomsky;
    newProductionsChomsky.clear();
    //pasul 2
    for (auto& production : m_Production)
    {
        if (production.second.size() > 1) {
            bool ok  = false;
            for (char symbol : production.second)
            {
                if (std::find(m_Vt.begin(), m_Vt.end(), symbol) != m_Vt.end())
                {
                    ok = true;
                }
            }
            if (ok) {
                for (auto &symbol : production.second)
                {
                    if (std::find(m_Vt.begin(), m_Vt.end(), symbol) != m_Vt.end())
                    {
                        std::string Csymbol = std::string(1, symbol);
                        symbol = returnNonterminal();
                        m_Vn.push_back(symbol);
                        newProductionsChomsky.emplace_back(std::string(1, symbol), Csymbol);
                    }
                }
            }
        }
    }
    for (const auto& production : newProductionsChomsky)
    {
        m_Production.emplace_back(production);
    }
    //pasul 3
    newProductionsChomsky.clear();
    std::vector<std::pair<std::string, std::string>> newProductions{ m_Production };
    for (auto& production : newProductions)
    {
        if (production.second.size() > 2)
        {
            std::string A = production.first; // S
            std::string B1 = production.second.substr(0, 1); // C
            std::string B2m = production.second.substr(1); // DAB

            char D1 = returnNonterminal();
            m_Vn.push_back(D1);

            // A -> B1D1
            newProductionsChomsky.emplace_back(A, B1 + std::string(1, D1));

            // Adăugăm regulile D1 -> B2D2, D2 -> B3D3, ..., Dm-1 -> Bm-1Bm
            for (size_t i = 1; i < B2m.size(); ++i)
            {

                char Di = returnNonterminal();
                if(i != B2m.size() - 1)
                    m_Vn.push_back(Di);

                // Di-1 -> BiDi
                if(i == B2m.size() - 1)
                    newProductionsChomsky.emplace_back(std::string(1, Di - 1), B2m.substr(i - 1, 1) + std::string(1, B2m[i]));
                else
                    newProductionsChomsky.emplace_back(std::string(1, Di - 1), B2m.substr(i - 1, 1) + std::string(1, Di));
            }

            // Eliminăm producția originală
            m_Production.erase(std::remove(m_Production.begin(), m_Production.end(), production), m_Production.end());
        }

    }
    // Pasul 3: Adăugăm variabilele noi în G'
    for (const auto& production : newProductionsChomsky)
    {
        m_Production.emplace_back(production);
    }
}

void Grammar::eliminateLambdaProduction(std::vector<char>& NewVn, std::vector<std::pair<std::string, std::string>>& NewProduction)
{
    NewVn.clear();
    std::unordered_set<char>newNonterminal;
    for (const auto& production : m_Production)
    {
        if (production.second.at(0) == '@' && production.second.size() == 1)//este lambda
        {
            newNonterminal.emplace(production.first.at(0));
        }
    }
    NewVn.assign(newNonterminal.begin(), newNonterminal.end());
    NewProduction.clear();
    for (const auto& Nonterminal : NewVn)
    {
        for (const auto& production : m_Production)
        {
            if (production.second.size() == 1 && production.second.at(0) == m_lambda)
                continue;
            if (production.second.size() > 1) {
                if (std::find(production.second.begin(), production.second.end(), Nonterminal) != production.second.end()) {
                    std::string aux = production.second;
                    aux.erase(std::remove(aux.begin(), aux.end(), Nonterminal), aux.end());
                    NewProduction.emplace_back(production);
                    NewProduction.emplace_back(production.first, aux);
                }
            }
        }
    }
    //avem cazul S -> lambda
    if(newNonterminal.find(m_Start) != newNonterminal.end())
    {
        NewVn.emplace_back('$');
        NewProduction.emplace_back("$", std::string(1,m_Start));
        NewProduction.emplace_back("$", std::string(1,m_lambda));
        m_Vn.emplace_back('$');
    }
    if(newNonterminal.size() != 0 && NewProduction.size() != 0)
        m_Production = NewProduction;
}

//CONVERT TO FNG
void Grammar::lema1(int indexProduction, int indexNonterminal)
{
    std::string left = m_Production[indexProduction].first;//obtin partea din stanga
    std::string right = m_Production[indexProduction].second;//obtin partea din dreapta
    std::vector<std::pair<std::string, std::string>> NewProductions = m_Production;
    std::vector<std::pair<std::string, std::string>> elementsADD;
    auto itToDelete = m_Production.begin() + indexProduction;
    for (auto [prodLeft, prodRight] : NewProductions)
    {
        if (prodLeft == NewProductions[indexProduction].first &&
            prodRight == NewProductions[indexProduction].second)
            continue;

        if (prodLeft[0] == right[indexNonterminal] )
        {
            if (prodRight[0] == m_lambda)
            {
                continue;
            }
            std::string aux = right;
            auto beginIt = aux.begin() + indexNonterminal;
            aux.replace(beginIt, beginIt + 1, prodRight);
            elementsADD.emplace_back( left, aux );
        }
    }
    m_Production.erase(itToDelete);
    for (auto& prod : elementsADD)
    {
        m_Production.emplace_back(prod);
    }
    
}

void Grammar::lema2(int indexProduction, int indexNonterminal) {
    std::vector<std::pair<std::string, std::string>> NewProduction;
    std::string left = m_Production[indexProduction].first;
    std::string right = m_Production[indexProduction].second;
    auto itToDelete = m_Production.begin() + indexProduction;
    std::string alpha = right.substr(1);
    NewProduction.emplace_back(std::string(1, Z), alpha);
    alpha += Z;
    m_Vn.emplace_back(Z);
    NewProduction.emplace_back(std::string(1, Z), alpha);
    for (auto& [prodLeft, prodRight] : m_Production)
    {
        if (prodLeft == left &&
            prodRight == right)
            continue;
        if (prodRight == std::string(1,m_lambda))
            continue;
        if (prodLeft == left) {
            NewProduction.emplace_back(left,prodRight + Z);
        }
    }
    m_Production.erase(itToDelete);
    for (const auto& production : NewProduction)
    {
        m_Production.emplace_back(production);
    }
    Z += 1;
}

bool Grammar::isApplicableLema1(const char& VnLeft, const char& VnRight)
{
    //ambele trebuie sa fie neterminale
    if (VnLeft == VnRight)
        return false;
    if (std::find(m_Vn.begin(), m_Vn.end(), VnLeft) == m_Vn.end() ||
        std::find(m_Vn.begin(), m_Vn.end(), VnRight) == m_Vn.end())
        return false;
    if (getIndexNonTerminal(VnLeft) < getIndexNonTerminal(VnRight))
        return false;
    return true;
}

int Grammar::getIndexNonTerminal(const char& Vn)
{
    for (int i = 0; i < m_Vn.size(); i++)
    {
        if (m_Vn[i] == Vn)
            return i;
    }
}

bool Grammar::isApplicableLema1ForStep2(const char& VnLeft, const char& VnRight) {
    if (VnLeft == VnRight)
        return false;
    if (std::find(m_Vn.begin(), m_Vn.end(), VnLeft) == m_Vn.end() ||
        std::find(m_Vn.begin(), m_Vn.end(), VnRight) == m_Vn.end())
        return false;
    if (getIndexNonTerminal(VnLeft) > getIndexNonTerminal(VnRight))
        return false;
    return true;
}

void Grammar::transformGrammarToFNG()
{
    chomskyNormalForm();
    //pasul 1
    bool modify = true;
    while(modify)
    {
        modify = false;
        for (int i = 0; i < m_Production.size(); ++i)
        {
            if (isApplicableLema1(m_Production[i].first.at(0), m_Production[i].second.at(0)) && m_Production[i].first.at(0) != 'Z')
            {
                lema1(i, 0);
                modify = true;
                break;
            }
            if (m_Production[i].first.at(0) == m_Production[i].second.at(0)) {
                lema2(i, 0);
                modify = true;
                break;
            }
           
        }
    }
    //pasul 2
    
        for (int i = getIndexNonTerminal(Zanterior) - 1; i >= 0; i--)
        {
            for (int j = 0; j < m_Production.size(); ++j)
            {
                if (isApplicableLema1ForStep2(m_Production[j].first.at(0), m_Production[j].second.at(0)) &&
                    m_Production[j].first.at(0) == m_Vn[i])
                {
                    lema1(j, 0);
                }
            }
        }
    

    //pasul 3
    bool ok = true;
    while (ok)
    {
        ok = false;
        for (int i = 0; i < m_Production.size(); ++i)
        {
            if (getIndexNonTerminal(m_Production[i].first.at(0)) >= getIndexNonTerminal(Zanterior) &&
                std::find(m_Vn.begin(), m_Vn.end(), m_Production[i].second.at(0)) != m_Vn.end())
            {
                lema1(i, 0);
                ok = true;
            }
        }
    }
    
}   

//GETTERS
std::vector<char> Grammar::getVn() const noexcept {
    return m_Vn;
}

std::vector<char> Grammar::getVt() const noexcept {
    return m_Vt;
}

char Grammar::getStart() const noexcept {
    return m_Start;
}

std::vector<std::pair<std::string, std::string>> Grammar::getProduction() const noexcept {
    return m_Production;
}

//SETTERS
void Grammar::setVn(std::vector<char> Vn) noexcept
{
    m_Vn = Vn;
}

void Grammar::setVt(std::vector<char> Vt) noexcept
{
    m_Vt = Vt;
}

void Grammar::setStart(char Start) noexcept
{
    m_Start = Start;
}

void Grammar::setProduction(std::vector<std::pair<std::string, std::string>> Production) noexcept
{
    m_Production = Production;
}

char Grammar::getLambda() const {
    return m_lambda;
}

std::string Grammar::generateWord() {
    // Verificam daca gramatica este valida
    if (verifyGrammar()) {
        // Initializam un generator de numere aleatoare
        std::random_device rd;
        std::mt19937 eng(rd());
        // Distributie pentru alegerea unei productii
        std::uniform_int_distribution<> distr_Prod(0, m_Production.size() - 1);

        // Pair de tipul (partea stanga, partea dreapta) pentru o productie
        std::pair<std::string, std::string> Prod;
        auto& [P_left_member, P_right_member] = Prod;

        // Afisarea regulilor de productie inainte de inceperea generarii cuvantului
        std::cout << "Regulile de productie:" << std::endl;
        for (const auto& rule : m_Production) {
            std::cout << rule.first << " -> " << rule.second << std::endl;
        }

        // Incepem constructia cuvantului cu neterminalul initial
        std::string word;
        word += m_Start;
        std::cout << "\nInceput: " << word << std::endl;

        bool check_valid = true;
        while (check_valid) {
            check_valid = false;

            // Verificam daca exista vreun neterminal in cuvant
            for (auto& a : m_Production) {
                if (word.find(a.first) != std::string::npos) {
                    check_valid = true;
                    break;
                }
            }

            if (check_valid) {
                // Set pentru a stoca indicii pentru inlocuiri posibile
                std::unordered_set<int> indices;

                // Alegem o productie aleatorie
                Prod = m_Production[distr_Prod(eng)];
                //std::cout << Prod.first << " ->> " << Prod.second << "\n";

                int index = 0;
                // Gasim toate aparitiile neterminalului din partea stanga
                while ((index = word.find(P_left_member, index)) != std::string::npos) {
                    indices.insert(index);
                    index += P_left_member.length();
                }

                if (indices.size()) {
                    // Alegem aleator un indice pentru inlocuire
                    std::uniform_int_distribution<> distr_Indices(0, indices.size() - 1);
                    auto it = indices.begin();
                    std::advance(it, distr_Indices(eng));

                    if (P_right_member.find(m_lambda) != std::string::npos) {
                        // Tratam inlocuirea cu lambda
                        std::string aux = P_right_member;
                        const auto& pos = aux.find(m_lambda);
                        aux.erase(pos, 1); // Stergem o aparitie de lambda
                        word.replace(*it, P_left_member.size(), aux);
                    }
                    else {
                        word.replace(*it, P_left_member.size(), P_right_member);
                    }
                    std::cout << "Inlocuire: " << P_left_member << " -> " << P_right_member << std::endl;
                    std::cout << "Rezultat: " << word << "\n";
                }
            }
        }

        std::cout << "Cuvantul generat: " << word << std::endl;
        return word;
    }
    return "";
}