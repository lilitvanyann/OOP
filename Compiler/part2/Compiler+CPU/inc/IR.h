#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "Parser.h"
#include "AST.h"

struct IRInstruction {
    std::string op;   // ADD, SUB, ASSIGN ..

    std::string arg1;
    std::string arg2;

    std::string result;

    std::string type = "int";
    int localSize = 0;


};

class SymbolTable;

class IR
{
public:
    IR(SymbolTable* symTable);
    std::vector<IRInstruction> generate(BlockNode* root);

private:
    SymbolTable* symbolTable;
    std::vector<IRInstruction> instructions;

    int tempCount = 0;
    int labelCount = 0;

    std::string newTemp();
    std::string newLabel();


    void visitStatement(StatementNode* stmt);

    std::string visitExpr(ExpressionNode* expr);

    void visitIf(IfNode* ifNode, const std::string& endLabel);

    void emitCondition(ExpressionNode* expr, const std::string& falseLabel);
};

