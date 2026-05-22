#pragma once
#include <string>
#include <vector>
#include <iostream>
//---------------------Lexer--------------------------------------

//================ TOKEN =================

enum class TokenType
{

    NUMBER,
    IDENT,

    INT,
    IF,
    ELSE,
    WHILE,
    RET,    

    PLUS,
    MINUS,
    MUL,
    DIV,
    ASSIGN,

    EQ,
    NEQ,
    LT,
    GT,
    LE,
    GE,
    INC,
    DEC,
    PLUS_ASSIGN,
    MINUS_ASSIGN,

    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    SEMI,
    COMMA,

    NOT,
    AND,
    OR,

    PRINT,
    END

};

struct Token
{
    TokenType type;
    int value;
    std::string name;
};

class Lexer
{
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

//================ OP =================

enum class OpType
{
    NONE,
    ADD, SUB, MUL, DIV,
    EQ, NEQ, LT, GT, LE, GE, INC, DEC,
    PLUS_ASSIGN, MINUS_ASSIGN, 
    AND,OR,NOT,
    MOV, CMP
};

