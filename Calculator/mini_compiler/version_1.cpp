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
            if (current == '=') {
                tokens.push_back(Token{ TokenType::ASSIGN, 0, '\0' });
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

    Node* statement() {
        if (current.type == TokenType::IDENT &&
            tokens[pos + 1].type == TokenType::ASSIGN) {

            char varName = current.name;
            advance(); // IDENT
            advance(); // =

            Node* right = expr();
            Node* left = new Node(varName);

            return new Node(OpType::NONE, left, right); // assignment node
        }

        return expr();
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

        root = statement();

        if (current.type != TokenType::END)
            throw runtime_error("Unexpected input after expression");

        return root;
    }
};

//------------------------------------Intermediate Representation------------------

enum class InstrType {
    PUSH, ADD, SUB, MUL, DIV,
    LOAD, STORE
};

struct Instruction {
    InstrType type;
    int value = 0;
    char name = '\0'; // variable
};

void generateIR(Node* node, vector<Instruction>& ir) {
    if (!node) return;

    //  assignment (=)
    if (node->type == NodeType::OPERATOR &&
        node->op == OpType::NONE &&
        node->left && node->left->type == NodeType::VARIABLE) {

        generateIR(node->right, ir);

        ir.push_back({ InstrType::STORE, 0, node->left->var });
        return;
    }

    // normal post-order
    generateIR(node->left, ir);
    generateIR(node->right, ir);

    if (node->type == NodeType::NUMBER) {
        ir.push_back({ InstrType::PUSH, node->value });
    }
    else if (node->type == NodeType::VARIABLE) {
        ir.push_back({ InstrType::LOAD, 0, node->var });
    }
    else if (node->type == NodeType::OPERATOR) {
        switch (node->op) {
        case OpType::ADD: ir.push_back({ InstrType::ADD }); break;
        case OpType::SUB: ir.push_back({ InstrType::SUB }); break;
        case OpType::MUL: ir.push_back({ InstrType::MUL }); break;
        case OpType::DIV: ir.push_back({ InstrType::DIV }); break;
        default: break;
        }
    }
}

//------------------------------ExecuteIR---------------------------------------------------

int executeIR(vector<Instruction>& ir, SymbolTable& sym) {
    vector<int> stack;

    for (auto& ins : ir) {
        
        switch (ins.type) {

        case InstrType::PUSH:
            stack.push_back(ins.value);
            break;

        case InstrType::LOAD:
            stack.push_back(sym.get(ins.name));
            break;

        case InstrType::ADD: {
            int b = stack.back(); stack.pop_back();
            int a = stack.back(); stack.pop_back();
            stack.push_back(a + b);
            break;
        }

        case InstrType::SUB: {
            int b = stack.back(); stack.pop_back();
            int a = stack.back(); stack.pop_back();
            stack.push_back(a - b);
            break;
        }

        case InstrType::MUL: {
            int b = stack.back(); stack.pop_back();
            int a = stack.back(); stack.pop_back();
            stack.push_back(a * b);
            break;
        }

        case InstrType::DIV: {
            int b = stack.back(); stack.pop_back();
            int a = stack.back(); stack.pop_back();
            stack.push_back(a / b);
            break;
        }

        case InstrType::STORE: {
            int val = stack.back(); stack.pop_back();
            sym.set(ins.name, val);
            break;
        }
        }
    }

    return stack.empty() ? 0 : stack.back();
}

//-----------------------------

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


void PrettyPrint(Node* node, string indent = "", bool isLast = true)
{
    if (node == nullptr) return;

    cout << indent;

    if (isLast) {
        cout << "└──";
        indent += "   ";
    }
    else {
        cout << "├──";
        indent += "|  ";
    }

    // Տպում ենք node-ի բովանդակությունը
        if (node->type == NodeType::NUMBER) {
            cout << "(" << node->value << ")";
        }
        else if (node->type == NodeType::VARIABLE) {
            cout << "(" << node->var << ")";
        }
        else {
            switch (node->op) {
                case OpType::ADD: cout << "(+)"; break;
                case OpType::SUB: cout << "(-)"; break;
                case OpType::MUL: cout << "(*)"; break;
                case OpType::DIV: cout << "(/)"; break;
                default: cout << "?";
            }
        }
    cout << endl;

    // children
    if (node->left || node->right) {
        PrettyPrint(node->left, indent, false);
        PrettyPrint(node->right, indent, true);
    }
}

int main() {
    try {
        string input;

        cout << "Enter expression: (note! for variables use only x and y)"<<endl;
        getline(cin, input);

        // 1️ LEXER
        Lexer lexer(input);
        vector<Token> tokens = lexer.tokenize();

        // 2️ PARSER → AST
        Parser parser;
        Node* root = parser.parse(tokens);

        // 3️  IR GENERATION
        vector<Instruction> ir;
        generateIR(root, ir);

        // 4️  SYMBOL TABLE
        SymbolTable sym;
        int x,y;

        for (int i = 0; i < input.size(); i++) {
            if (input[i] == 'x' ) 
            {
                int num1;
                cout<<"enter x:";cin>>num1;
            
                sym.set('x', num1);
            }
        }
        for (int i = 0; i < input.size(); i++) {
            if (input[i] == 'y') 
            {
                int num2;
                cout<<"enter y:";cin>>num2;
                sym.set('y', num2);
            }
        }

        // 5️  EXECUTION
        int result = executeIR(ir, sym);
        
        // Print tree
        cout<<endl<<"Tree :"<<endl;
        printTree(root);
        cout<<endl<<endl;
        
        cout<<"Tree structur :"<<endl;
        PrettyPrint(root);
        cout<<endl<<endl;
        //Result
        cout << "Result: " << result << endl;

    }
    catch (exception& e) {
        cout << "Error: " << e.what() << endl;
    }

    return 0;
}
