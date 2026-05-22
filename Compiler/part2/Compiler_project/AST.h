#pragma once
#include "Lexer.h"

//========================================================== NODES (Expressions AST) =======================


//========== EXPRESSIONS (INHERITANCE) =================

enum class ExprType
{
    Number,
    Variable,
    Binary,
    Call,
    Unary,
    Assignment,
    Logical,
};

//---------------- BLOCK ----------------
struct StatementNode;
struct BlockNode
{
    std::vector<std::unique_ptr<StatementNode>> statements;

};
//---------------- BASE ----------------

struct ExpressionNode
{
    ExprType type;
    virtual ~ExpressionNode() = default;
};

//---------------- NUMBER ----------------

struct NumberNode : ExpressionNode
{
    int value;

    NumberNode()
    {
        type = ExprType::Number;
    }
};

//---------------- VARIABLE ----------------

struct VariableNode : ExpressionNode
{
    std::string name;

    VariableNode()
    {
        type = ExprType::Variable;
    }
};

//---------------- BINARY ----------------

struct BinaryNode : ExpressionNode
{
    std::unique_ptr<ExpressionNode> left;
    std::unique_ptr<ExpressionNode> right;

    OpType op;

    BinaryNode()
    {
        type = ExprType::Binary;
        op = OpType::NONE;
    }
};

//---------------- FunctionCall ----------------

struct FunctionCallNode : ExpressionNode
{
    std::string name;
    std::vector < std::unique_ptr<ExpressionNode> > args;

    FunctionCallNode()
    {
        type = ExprType::Call;
    }
};
struct UnaryNode : ExpressionNode
{
    OpType op;
    std::unique_ptr<ExpressionNode> expr;

    UnaryNode()
    {
        type = ExprType::Unary;
    }
};
//---------------- ASSIGNMENT ----------------

struct AssignmentNode : ExpressionNode
{
    std::string variableName;
    std::unique_ptr<ExpressionNode> value;

    AssignmentNode()
    {
        type = ExprType::Assignment;
    }
};

//============================================== EXTRA NODES (Statements AST)=================

struct IfNode
{
    std::unique_ptr<ExpressionNode> condition ;
    std::unique_ptr<BlockNode> body ;

    std::unique_ptr<IfNode> elseIf;
    std::unique_ptr<BlockNode> elseBody;
};

struct WhileNode
{
    std::unique_ptr<ExpressionNode> condition;
    std::unique_ptr<BlockNode> body ;
};

struct ReturnNode
{
    std::unique_ptr<ExpressionNode> value;
};

struct VariableDeclarationNode
{
    std::string type;
    std::string name;
    std::unique_ptr<ExpressionNode> value ;
};
struct Parameter
{
    std::string type;
    std::string name;
};
struct FunctionNode
{
    std::string returnType;
    std::string name;

    std::vector<Parameter> parameters;

    std::unique_ptr<BlockNode> body;
};

struct PrintNode
{
    std::unique_ptr<ExpressionNode> value;

};

//================ STATEMENTS =================


enum class StmtType
{
    If,
    While,
    Ret,
    Assignment,
    Block,
    VariableDeclaration,
    Function,
    Print,
    Expression
};

struct StatementNode
{
    StmtType type;

    std::unique_ptr<IfNode> ifNode;
    std::unique_ptr<WhileNode> whileNode;
    std::unique_ptr<ReturnNode> returnNode;
    std::unique_ptr<AssignmentNode> assignNode;
    std::unique_ptr<VariableDeclarationNode> varDeclNode;
    std::unique_ptr<BlockNode> blockNode;
    std::unique_ptr<FunctionNode> functionNode;
    std::unique_ptr<PrintNode> printNode;

    std::unique_ptr<ExpressionNode> exprNode;


};


