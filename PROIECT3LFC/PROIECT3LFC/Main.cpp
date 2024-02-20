#include "Grammar.h"
#include "PushDownAutomaton.h"

PushDownAutomaton TransformIDCGrammarToPushDownAutomaton(Grammar G) {
    if (!G.isIDC()) {
        throw std::invalid_argument("Gramatica nu este independenta de context");
    }
    G.simplifyGrammar();
    G.transformGrammarToFNG();
    std::vector<char> states{ 'Q' };
    std::vector<char> alphabet = G.getVt();
    std::vector<char> stackState = G.getVn();
    char initialState = G.getStart();
    char initialStackState = 'Q';
    std::vector<char> finalStates = G.getVn();
    std::unordered_map<std::tuple<char, char, char>, std::vector<std::pair<char, std::string>>> map;

    for (const auto& production : G.getProduction()) {
        char currentState = production.first[0];
        char inputSymbol = production.second[0];

        std::string nextState = production.second.size() > 1 ? production.second.substr(1) : std::string(1, G.getLambda());

        std::tuple<char, char, char> first{initialStackState, inputSymbol, currentState };

        std::pair<char, std::string> second{ initialStackState, nextState };
        map[first].emplace_back(second);
    }

    PushDownAutomaton A = { states, alphabet, stackState, initialState, initialStackState, finalStates };
    A.setFinalTranstion(map);
    return A;
}

int main()
{
   Grammar G;
   G.readGrammar();
   if (G.isIDC()) {
        while (true) {
            std::cout << std::endl;
            std::cout << "=== Meniu ===" << std::endl;
            std::cout << "1. Afisarea gramaticii G" << std::endl;
            std::cout << "2. Generarea unui cuvant in gramatica" << std::endl;
            std::cout << "3. Afisarea rezultatului gramaticii simplificate" << std::endl;
            std::cout << "4. Afisarea gramaticii in FNG" << std::endl;
            std::cout << "5. Generarea unui cuvant in G si verificarea daca e acceptat de automat" << std::endl;
            std::cout << "6. Verificarea daca un cuvant citit de la tastatura e acceptat de automat" << std::endl;
            std::cout << "0. Iesire" << std::endl;
            std::cout << std::endl;
            short n, number;
            std::string s;
            std::unordered_set<std::string> set;
            PushDownAutomaton APD;
            std::cout << "Introduceti o comanda: ";
            std::cin >> n;
            Grammar CopyG = G;
            switch (n)
            {
            case 1:
                G.printGrammar();
                break;
            case 2:
                s.clear();
                s = G.generateWord();
                break;
            case 3:
                CopyG.simplifyGrammar();
                break;
            case 4:
                G.transformGrammarToFNG();
                G.printGrammar();
                break;
            case 5:
                s.clear();
                s = G.generateWord();
                APD = TransformIDCGrammarToPushDownAutomaton(G);
                if (APD.CheckWord(s))
                    std::cout << "VALID" << '\n';
                else
                    std::cout << "INVALID" << '\n';
                break;
            case 6:
                std::cout << "'\nINTRODUCETI UN CUVANT DE LA TASTATURA: ";
                std::cin >> s;
                if (APD.CheckWord(s))
                    std::cout << "VALID" << '\n';
                else
                    std::cout << "INVALID" << '\n';
                break;
            case 0:
                std::cout << "LA REVEDERE:))" << std::endl;
                return 0;

            default:
                std::cout << "Comanda invalida. Incercati din nou." << std::endl;
            }
        }
   }
    return 0;
}