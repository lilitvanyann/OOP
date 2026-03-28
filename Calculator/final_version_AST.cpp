#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>

using namespace std;

//----Variables--------
class SymbolTable {
private:
    map <char, int> table; // key->char, value-> int

public:
    void set(const char& name, int value) {
        table[name] = value;
    }

    int get(const char& name) const {
        auto it = table.find(name);
        if (it != table.end()) return it->second; //val
        throw std::runtime_error("Unknown variable: " + string(1, name));
    }

    bool exists(const char& name) const {
        return table.find(name) != table.end();
    }
};


//-------------------------------Lexer---------------------
//--String-to-described-vector--


//-------Lexer output language------(lexer)
enum class TokenType {
    NUMBER, IDENT,
    PLUS, MINUS, MUL, DIV,
    LPAREN, RPAREN,
    ASSIGN, END
};
//---Tokenizer-----(lexer)
struct Token {
    TokenType type;
    int value = 0;
    char name = '\0';
};

class Lexer {
private:
    string text;
    int pos;
    char current;

    void advance() {
        pos++;
        if (pos < text.size())
            current = text[pos];
        else
            current = '\0';
    }
    void skip_whitespace() {
        while (current == ' ')
            advance();
    }

    Token number() {
        string result = "";
        while (current != '\0' && isdigit(current))
        {
            result += current;
            advance();
        }
        return Token{ TokenType::NUMBER,(stoi(result)),'\0' };
    }


public:
    Lexer(string input_text) {
        text = input_text;
        pos = 0;

        if (!text.empty())
            current = text[0];
        else
            current = '\0';
    }



    vector<Token> tokenize() {
        vector<Token> tokens;

        while (current != '\0') {

            if (current == ' ') {
                skip_whitespace();
                continue;
            }

            if (isdigit(current)) {
                tokens.push_back(number());
                continue;
            }

            if (isalpha(current)) {
                char name = current;
                advance();

                tokens.push_back(Token{ TokenType::IDENT, 0, name });
                continue;
            }

            if (current == '+') {
                tokens.push_back(Token{ TokenType::PLUS, 0, '\0' });
                advance();
                continue;
            }

            if (current == '-') {
                tokens.push_back(Token{ TokenType::MINUS, 0, '\0' });
                advance();
                continue;
            }

            if (current == '*') {
                tokens.push_back(Token{ TokenType::MUL, 0, '\0' });
                advance();
                continue;
            }

            if (current == '/') {
                tokens.push_back(Token{ TokenType::DIV, 0, '\0' });
                advance();
                continue;
            }

            if (current == '(') {
                tokens.push_back(Token{ TokenType::LPAREN, 0, '\0' });
                advance();
                continue;
            }

            if (current == ')') {
                tokens.push_back(Token{ TokenType::RPAREN, 0, '\0' });
                advance();
                continue;
            }

            throw runtime_error("Invalid character");
        }

        tokens.push_back(Token{ TokenType::END, 0, '\0' });
        return tokens;
    }
};

//-----------------------PARSER--------------------------

//--AST-node-type--
enum class NodeType { NUMBER, VARIABLE, OPERATOR };

//--AST-node-operator--
enum class OpType { NONE, ADD, SUB, MUL, DIV };

struct Node {
    NodeType type;
    OpType op;
    char var;
    int value; //if digit
    Node* right;
    Node* left;

    //for operand node
    Node(int v) {
        type = NodeType::NUMBER;
        op = OpType::NONE;
        value = v;
        var = '\0';
        left = right = nullptr;
    }

    //for operator node
    Node(OpType o, Node* l, Node* r) {
        type = NodeType::OPERATOR;
        op = o;
        value = 0;
        var = '\0';
        left = l;
        right = r;
    }

    //for variable
    Node(char c) {
        type = NodeType::VARIABLE;
        op = OpType::NONE;
        value = 0;
        var = c;
        left = right = nullptr;
    }
};

//--Vector-to-Tree--

class Parser {
private:
    Node* root = nullptr;

    void deleteTree(Node* node) {
        if (!node) return;

        deleteTree(node->left);
        deleteTree(node->right);

        delete node;
    }

    vector<Token> tokens;
    int pos;
    Token current;

    void advance() {
        pos++;
        if (pos < tokens.size())
            current = tokens[pos];
        else
            current = Token{ TokenType::END, 0, '\0' };
    }

    //--PLUS_MINUS--

    Node* expr() {
        Node* node = term();

        while (current.type == TokenType::PLUS ||
            current.type == TokenType::MINUS) {

            OpType op = (current.type == TokenType::PLUS)
                ? OpType::ADD
                : OpType::SUB;

            advance();

            Node* right = term();

            node = new Node(op, node, right);
        }

        return node;
    }

    //--MUL_DIV--

    Node* term() {
        Node* node = factor();

        while (current.type == TokenType::MUL ||
            current.type == TokenType::DIV) {

            OpType op = (current.type == TokenType::MUL)
                ? OpType::MUL
                : OpType::DIV;

            advance();

            Node* right = factor();

            node = new Node(op, node, right);
        }

        return node;
    }

    //--NUMBER-VAR-LPAREN/RPAREN--

    Node* factor() {
        // number
        if (current.type == TokenType::NUMBER) {
            Node* node = new Node(current.value);
            advance();
            return node;
        }

        // variable
        if (current.type == TokenType::IDENT) {
            Node* node = new Node(current.name);
            advance();
            return node;
        }

        // parentheses
        if (current.type == TokenType::LPAREN) {
            advance();
            Node* node = expr();

            if (current.type != TokenType::RPAREN)
                throw runtime_error("Missing ')'");

            advance();
            return node;
        }

        throw runtime_error("Invalid factor");
    }

public:
    ~Parser() {
        deleteTree(root);
    }
    Node* parse(vector<Token> t) {
        tokens = t;
        pos = 0;
        current = tokens[0];

        if (root)
            deleteTree(root);

        root = expr();

        if (current.type != TokenType::END)
            throw runtime_error("Unexpected input after expression");

        return root;
    }
};

//--Tree-print--

void printTree(Node* node, int depth = 0) {
    if (!node) return;

    // right first
    printTree(node->right, depth + 1);

    // indent
    for (int i = 0; i < depth; i++)
        cout << "    ";

    // node value
    if (node->type == NodeType::NUMBER)
        cout << node->value;
    else if (node->type == NodeType::VARIABLE)
        cout << node->var;
    else {
        if (node->op == OpType::ADD) cout << "+";
        if (node->op == OpType::SUB) cout << "-";
        if (node->op == OpType::MUL) cout << "*";
        if (node->op == OpType::DIV) cout << "/";
    }

    cout << endl;

    // left
    printTree(node->left, depth + 1);
}

int main() {
    try {
        string input;

        cout << "Enter expression: ";
        getline(cin, input);

        // 1. Lexer
        Lexer lexer(input);
        vector<Token> tokens = lexer.tokenize();

        // 2. Parser
        Parser parser;
        Node* root = parser.parse(tokens);


        // 3. Just test AST (temporary print)
        cout << "Parsing successful! AST created." << endl;

        // (optional) debug check
        cout << "Root node created." << endl;

        cout << "\nAS Tree:\n";
        printTree(root);
        cout << endl;
    }
    catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }


    return 0;

}
