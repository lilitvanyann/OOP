#pragma once
#include <map>
#include <vector>
#include <string>
#include <set>

//--------------------------------------Symb_table------------------------------------------------------------


class SymbolTable {
private:

    std::map <std::string, int> table; // key->string, value-> int

public:
    void set(std::string name, int value);

    int get(std::string name) const;

    bool exists(std::string name) const;
};

//--------------------------------------Lexer------------------------------------------------------------
enum class TokenType {
    NUMBER, IDENT,
    PLUS, MINUS, MUL, DIV,
    LPAREN, RPAREN,
    ASSIGN, END
};

struct Token {
    TokenType type;
    int value;
    std::string name;
};

class Lexer {
private:
    std::string text;
    int pos;
    char current;

    void advance();
    void skip_whitespace();
    Token number();

public:
    Lexer(std::string input_text);
    std::vector<Token> tokenize();
};

//--------------------------------------Parser------------------------------------------------------------



enum class NodeType { NUMBER, VARIABLE, OPERATOR };
enum class OpType { NONE, ADD, SUB, MUL, DIV };

struct Node {
    NodeType type;
    OpType op;
    std::string var;
    int value;
    Node* left;
    Node* right;

    Node(int v);
    Node(OpType o, Node* l, Node* r);
    Node(std::string c);
};
void collectVariables(Node* root, std::set<std::string>& variables);


class Parser {
private:
    Node* root;

    void deleteTree(Node* node);
    Node* expr();
    Node* term();
    Node* factor();
    Node* statement();
    void advance();

    std::vector<Token> tokens;
    int pos;
    Token current;

public:
    ~Parser();
    Node* parse(std::vector<Token> t);
};
