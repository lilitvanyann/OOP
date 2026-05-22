#include "Parser.h"
#include <stdexcept>
#include <string>

//for operand node
Node::Node(int v)
    : type(NodeType::NUMBER),
    op(OpType::NONE),
    value(v),
    var(""),
    left(nullptr),
    right(nullptr) {
}

//for operator node
Node::Node(OpType o, Node* l, Node* r)
    : type(NodeType::OPERATOR),
    op(o),
    value(0),
    var(""),
    left(l),
    right(r) {
}
//for variable
Node::Node(std::string c)
    : type(NodeType::VARIABLE),
    op(OpType::NONE),
    value(0),
    var(c),
    left(nullptr),
    right(nullptr) {
}

void SymbolTable::set(std::string name, int value) {
    table[name] = value;
}

int SymbolTable::get(std::string name) const {
    auto it = table.find(name);
    if (it != table.end()) return it->second;
    throw std::runtime_error("Unknown variable:" + name);
}

bool SymbolTable::exists(std::string name) const {
    return table.find(name) != table.end();
}

void collectVariables(Node* root, std::set<std::string>& variables) {
    if (!root)return;

    if (root->type == NodeType::VARIABLE)
        variables.insert(root->var);

    collectVariables(root->left, variables);
    collectVariables(root->right, variables);
}

//-------------------------------Lexer---------------------
//--String-to-described-vector--



    void Lexer::advance() {
        pos++;
        if (pos < text.size())
            current = text[pos];
        else
            current = '\0';
    }
    void Lexer::skip_whitespace() {
        while (current == ' ')
            advance();
    }

    Token Lexer::number() {
        std::string result = "";
        while (current != '\0' && isdigit(current))
        {
            result += current;
            advance();
        }
        return Token{ TokenType::NUMBER,(stoi(result)),"" };
    }

    Lexer::Lexer(std::string input_text) {
        text = input_text;
        pos = 0;

        if (!text.empty())
            current = text[0];
        else
            current = '\0';
    }



    std::vector<Token> Lexer::tokenize() {
        std::vector<Token> tokens;

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
                std::string result;

                while (current != '\0' &&
                    (isalpha(current) || isdigit(current) || current == '_')) {
                    result += current;
                    advance();
                }

                tokens.push_back(Token{ TokenType::IDENT, 0, result });
                continue;
            }

            if (current == '+') {
                tokens.push_back(Token{ TokenType::PLUS, 0, ""});
                advance();
                continue;
            }

            if (current == '-') {
                tokens.push_back(Token{ TokenType::MINUS, 0, ""});
                advance();
                continue;
            }

            if (current == '*') {
                tokens.push_back(Token{ TokenType::MUL, 0, "" });
                advance();
                continue;
            }

            if (current == '/') {
                tokens.push_back(Token{ TokenType::DIV, 0, "" });
                advance();
                continue;
            }

            if (current == '(') {
                tokens.push_back(Token{ TokenType::LPAREN, 0, "" });
                advance();
                continue;
            }

            if (current == ')') {
                tokens.push_back(Token{ TokenType::RPAREN, 0, "" });
                advance();
                continue;
            }
            if (current == '=') {
                tokens.push_back(Token{ TokenType::ASSIGN, 0, "" });
                advance();
                continue;
            }

            throw std::runtime_error("Invalid character");
        }

        tokens.push_back(Token{ TokenType::END, 0, "" });
        return tokens;
    }


//-----------------------PARSER--------------------------

//--Vector-to-Tree--


    void Parser::deleteTree(Node* node) {
        if (!node) return;

        deleteTree(node->left);
        deleteTree(node->right);

        delete node;
    }

    void Parser::advance() {
        pos++;
        if (pos < tokens.size())
            current = tokens[pos];
        else
            current = Token{ TokenType::END, 0, "" };
    }

    //--PLUS_MINUS--

    Node* Parser::expr() {
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

    Node* Parser::term() {
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

    Node* Parser::factor() {
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
                throw std::runtime_error("Missing ')'");

            advance();
            return node;
        }

        throw std::runtime_error("Invalid factor");
    }

    Node* Parser::statement() {
        if (current.type == TokenType::IDENT &&
            tokens[pos + 1].type == TokenType::ASSIGN) {

            std::string varName = current.name;
            advance(); // IDENT
            advance(); // =

            Node* right = expr();
            Node* left = new Node(varName);

            return new Node(OpType::NONE, left, right); // assignment node
        }

        return expr();
    }

    Parser::~Parser() {
        deleteTree(root);
    }
    Node* Parser::parse(std::vector<Token> t) {
        tokens = t;
        pos = 0;
        current = tokens[0];

        if (root)
            deleteTree(root);

        root = statement();

        if (current.type != TokenType::END)
            throw std::runtime_error("Unexpected input after expression");

        return root;
    }
