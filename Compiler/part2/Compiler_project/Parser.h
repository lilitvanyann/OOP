#pragma once
#include "Lexer.h"
#include "AST.h"
#include "Symbol_Table.h"
#include <vector>
#include <string>

//================ PARSER =================

class Parser
{
private:
    std::vector<Token> tokens;
    int pos;
    Token current;
    SymbolTable symbolTable;

    void advance();

    std::unique_ptr<ExpressionNode> expr();
    std::unique_ptr<ExpressionNode> assignment();
    std::unique_ptr<ExpressionNode> comparison();
    std::unique_ptr<ExpressionNode> logical();

    std::unique_ptr<ExpressionNode> additive();
    std::unique_ptr<ExpressionNode> term();
    std::unique_ptr<ExpressionNode> unary();
    std::unique_ptr<ExpressionNode> factor();

    std::unique_ptr<StatementNode> statement();

public:
    SymbolTable& getSymbolTable() { return symbolTable; }

    std::unique_ptr<BlockNode> parse(std::vector<Token> t);
};

