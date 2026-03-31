#include <iostream>
#include <set>
#include <vector>
#include "Parser.h"
#include "Generator.h"
#include "Executor.h"
#include "TreePrint.h"
using namespace std;


int main() {
    try {
        string input;

        while (true) {

            cout << "Enter expression (or exit): ";
            getline(cin, input);

            if (input == "exit") break;

            // 1. LEXER
            Lexer lexer(input);
            vector<Token> tokens = lexer.tokenize();

            // 2. PARSER → AST
            Parser parser;
            Node* root = parser.parse(tokens);

            //TREE PRINT
            cout << "\nTree (PrettyPrint):\n";
            PrettyPrint(root);

            cout << "\nTree (Side view):\n";
            printTree(root, 0);

            // 3. VARIABLES
            set<string> variables;
            collectVariables(root, variables);

            // 4. SYMBOL TABLE
            SymbolTable sym;

            for (const string& var : variables) {
                int value;
                cout << "enter " << var << ": ";
                cin >> value;
                sym.set(var, value);
            }

            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // FIX

            // 5. IR
            vector<Instruction> ir;
            generateIR(root, ir);

            // 6. EXECUTE
            int result = executeIR(ir, sym);

            cout << "Result: " << result << endl;
        }
    }
    catch (exception& e) {
        cout << "Error: " << e.what() << endl;
    }
}
