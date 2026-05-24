#include "Parser.h"
#include <stdexcept>
#include <iostream>

//-------------------Parser-----------------------------

//================ ADVANCE =================

void Parser::advance()
{
    pos++;
    if (pos < tokens.size())
        current = tokens[pos];
    else
        current = { TokenType::END, 0, "" };
}

//================ ENTRY =================

std::unique_ptr<BlockNode> Parser::parse(std::vector<Token> t)
{
    tokens = t;
    pos = 0;
    current = tokens[0];

    auto block = std::make_unique<BlockNode>();

    while (current.type != TokenType::END)
    {
        auto stmt = statement();

        if (!stmt)
            throw std::runtime_error("Invalid statement (parser stuck)");

        block->statements.push_back(std::move(stmt));
    }
    return block;
}

//factor → term → additive → comparison → expr
//================ EXPRESSIONS =================

std::unique_ptr<ExpressionNode> Parser::expr()
{
    return  logical();
}

//================= (=/+=/-=) =================

std::unique_ptr<ExpressionNode> Parser::assignment()
{
    std::unique_ptr<ExpressionNode> node = logical();

    // only left side can be variable
    if (current.type == TokenType::ASSIGN)
    {
        if (node->type != ExprType::Variable)
            throw std::runtime_error("Left side must be variable");

        advance(); // =

        auto a = std::make_unique<AssignmentNode>();
        a->variableName
            = static_cast<VariableNode*>(node.get())->name;

        a->value = assignment(); 

        return a;
    }

    // +=
    if (current.type == TokenType::PLUS_ASSIGN)
    {
        if (node->type != ExprType::Variable)
            throw std::runtime_error("Left side must be variable");

        auto* var = static_cast<VariableNode*>(node.get());
        std::string varName = var->name;

        advance();

        auto b = std::make_unique<BinaryNode>();

        auto leftVar = std::make_unique<VariableNode>();
        leftVar->name = varName;
        b->left = std::move(leftVar);

        b->right = assignment();

        auto a = std::make_unique<AssignmentNode>();
        a->variableName = varName;
        a->value = std::move(b);

        return a;
    }

    // -=
    if (current.type == TokenType::MINUS_ASSIGN)
    {
        if (node->type != ExprType::Variable)
            throw std::runtime_error("Left side must be variable");

        auto* var = static_cast<VariableNode*>(node.get());
        std::string varName = var->name;

        advance();

        auto b = std::make_unique<BinaryNode>();
        b->op = OpType::SUB;

        auto leftVar = std::make_unique<VariableNode>();
        leftVar->name = varName;
        b->left = std::move(leftVar);

        b->right = assignment();

        auto a = std::make_unique<AssignmentNode>();
        a->variableName = varName;
        a->value = std::move(b);

        return a;
    }

    return node;
}
//================= COMPARISON =================

std::unique_ptr<ExpressionNode> Parser::comparison()
{
    std::unique_ptr<ExpressionNode> node = additive();

    while (
        current.type == TokenType::EQ ||
        current.type == TokenType::NEQ ||
        current.type == TokenType::LT ||
        current.type == TokenType::GT ||
        current.type == TokenType::LE ||
        current.type == TokenType::GE
        )
    {
        auto b = std::make_unique<BinaryNode>();

        b->left = std::move(node);

        TokenType opType = current.type;

        advance();

        b->right = additive();

        switch (opType)
        {
        case TokenType::EQ:  b->op = OpType::EQ; break;
        case TokenType::NEQ: b->op = OpType::NEQ; break;
        case TokenType::LT:  b->op = OpType::LT; break;
        case TokenType::GT:  b->op = OpType::GT; break;
        case TokenType::LE:  b->op = OpType::LE; break;
        case TokenType::GE:  b->op = OpType::GE; break;
        }

        node = std::move(b);
    }
    // AND (&&)
    if (current.type == TokenType::AND)
    {
        auto b = std::make_unique<BinaryNode>();
        b->left = std::move(node);
        b->op = OpType::AND;
        advance();
        b->right = comparison();
        return b;
    }

    // OR (||)
    if (current.type == TokenType::OR)
    {
        auto b = std::make_unique<BinaryNode>();
        b->left = std::move(node);
        b->op = OpType::OR;
        advance();
        b->right = comparison();
        return b;
    }

    return node;
}
std::unique_ptr<ExpressionNode> Parser::logical()
{
    std::unique_ptr<ExpressionNode> node = comparison();

    while (current.type == TokenType::AND || current.type == TokenType::OR)
    {
        auto b = std::make_unique<BinaryNode>();
        b->left = std::move(node);

        if (current.type == TokenType::AND) {
            b->op = OpType::AND;
        }
        else if (current.type == TokenType::OR) {
            b->op = OpType::OR;
        }

        advance();
        b->right = comparison();  // Right associative
        node = std::move(b);
    }

    return node;
}
//================= ADDITIVE (+ -) =================

std::unique_ptr<ExpressionNode> Parser::additive()
{
    std::unique_ptr<ExpressionNode> node = term();

    while (
        current.type == TokenType::PLUS ||
        current.type == TokenType::MINUS
        )
    {
        auto b = std::make_unique<BinaryNode>();
        b->left = std::move(node);

        b->op = (current.type == TokenType::PLUS)
            ? OpType::ADD
            : OpType::SUB;

        advance();

        b->right = term();
        node = std::move(b);
    }

    return node;
}

//================= TERM (* /) =================

std::unique_ptr<ExpressionNode> Parser::term()
{
    std::unique_ptr<ExpressionNode> node = unary();

    while (
        current.type == TokenType::MUL ||
        current.type == TokenType::DIV)
    {
        auto b = std::make_unique<BinaryNode>();
        b->left = std::move(node);

        b->op = (current.type == TokenType::MUL)
            ? OpType::MUL
            : OpType::DIV;

        advance();

        b->right = unary();
        node = std::move(b);
    }

    return node;
}
//--------------------unar_gorcoxutyun-----------
std::unique_ptr<ExpressionNode> Parser::unary()
{
    // Handle NOT (!)
    if (current.type == TokenType::NOT)
    {
        advance();
        auto node = std::make_unique<UnaryNode>();
        node->op = OpType::NOT;
        node->expr = unary();
        return node;
    }
    // Handle prefix ++var

    if (current.type == TokenType::INC)
    {
        advance();

        auto node = std::make_unique<UnaryNode>();
        node->op = OpType::INC;
        node->expr = unary();

        return node;
    }
    // Handle prefix --var

    if (current.type == TokenType::DEC)
    {
        advance();

        auto node = std::make_unique<UnaryNode>();
        node->op = OpType::DEC;
        node->expr = unary();

        return node;
    }

    return factor();
}
//================= FACTOR =================
std::unique_ptr<ExpressionNode> Parser::factor()
{
    // NUMBER
    if (current.type == TokenType::NUMBER)
    {
        auto n = std::make_unique<NumberNode>();
        n->value = current.value;
        advance();
        return n;
    }

    // VARIABLE or FUNCTION CALL
    if (current.type == TokenType::IDENT)
    {
        Token nameToken = current;
        advance();

        // FUNCTION CALL
        if (current.type == TokenType::LPAREN)
        {
            auto call = std::make_unique<FunctionCallNode>();
            call->name = nameToken.name;
            advance(); // (

            if (current.type != TokenType::RPAREN)
            {
                while (true)
                {
                    call->args.push_back(expr());
                    if (current.type == TokenType::COMMA)
                    {
                        advance();
                        continue;
                    }
                    break;
                }
            }

            if (current.type != TokenType::RPAREN)
                throw std::runtime_error("Expected )");
            advance(); // )
            return call;
        }

        // VARIABLE
        auto variable = std::make_unique<VariableNode>();
        variable->name = nameToken.name;

        // Handle postfix increment/decrement
        if (current.type == TokenType::INC || current.type == TokenType::DEC)
        {
            auto postNode = std::make_unique<UnaryNode>();
            postNode->op = (current.type == TokenType::INC) ? OpType::INC : OpType::DEC;
            postNode->expr = std::move(variable);
            advance();
            return postNode;
        }

        return variable;
    }

    // PARENTHESIZED EXPRESSION
    if (current.type == TokenType::LPAREN)
    {
        advance();
        auto node = expr();
        if (current.type != TokenType::RPAREN)
            throw std::runtime_error("Expected )");
        advance();
        return node;
    }

    std::string errorMsg = "Invalid factor. Unexpected token: ";
    switch (current.type) {
    case TokenType::PLUS: errorMsg += "'+'"; break;
    case TokenType::MINUS: errorMsg += "'-'"; break;
    case TokenType::MUL: errorMsg += "'*'"; break;
    case TokenType::DIV: errorMsg += "'/'"; break;
    case TokenType::ASSIGN: errorMsg += "'='"; break;
    case TokenType::SEMI: errorMsg += "';'"; break;
    case TokenType::RBRACE: errorMsg += "'}'"; break;
    case TokenType::END: errorMsg += "end of file"; break;
    default: errorMsg += "unknown token type " + std::to_string((int)current.type);
    }
    throw std::runtime_error(errorMsg);
}

//------------------------------------Statement-----------

std::unique_ptr<StatementNode> Parser::statement()
{

    //================ FUNCTION / VAR DECL =================
    if (current.type == TokenType::INT)
    {
        advance();

        if (current.type != TokenType::IDENT)
            throw std::runtime_error("Expected identifier");

        Token nameToken = current;
        std::string varName = current.name;

        advance();

        //================ FUNCTION =================
        if (current.type == TokenType::LPAREN)
        {
            // Enter new scope for function parameters
            symbolTable.enterScope();

            advance(); // consume '('

            auto stmt = std::make_unique<StatementNode>();
            stmt->type = StmtType::Function;

            auto fn = std::make_unique<FunctionNode>();
            fn->returnType = "int";
            fn->name = nameToken.name;

            int paramIndex = 0;

            if (current.type != TokenType::RPAREN)
            {
                while (true)
                {
                    Parameter p;

                    // parameter type
                    if (current.type != TokenType::INT)
                        throw std::runtime_error("Expected parameter type");

                    p.type = "int";
                    advance();

                    // parameter name
                    if (current.type != TokenType::IDENT)
                        throw std::runtime_error("Expected parameter name");

                    p.name = current.name;

                    // Declare parameter in symbol table
                    symbolTable.declareParameter(p.name, paramIndex);
                    paramIndex++;

                    advance();

                    fn->parameters.push_back(p);

                    if (current.type == TokenType::COMMA)
                    {
                        advance();
                        continue;
                    }

                    break;
                }
            }

            advance(); // )

            if (current.type != TokenType::LBRACE)
                throw std::runtime_error("Expected {");

            advance();

            auto body = std::make_unique<BlockNode>();

            while (current.type != TokenType::RBRACE)
                body->statements.push_back(std::move(statement()));

            advance();

            fn->body = std::move(body);

            symbolTable.exitScope();

            if (fn->body->statements.empty() ||
                fn->body->statements.back()->type != StmtType::Ret)
            {
                // Create an implicit return 0
                auto returnStmt = std::make_unique<StatementNode>();
                returnStmt->type = StmtType::Ret;

                auto returnNode = std::make_unique<ReturnNode>();

                // Create a number node for 0
                auto zeroNode = std::make_unique<NumberNode>();
                zeroNode->value = 0;
                returnNode->value = std::move(zeroNode);

                returnStmt->returnNode = std::move(returnNode);
                fn->body->statements.push_back(std::move(returnStmt));
            }

            stmt->functionNode = std::move(fn);

            return stmt;
        }
        else
        {

            //================ VARIABLE DECL =================

            if (symbolTable.existsInCurrentScope(varName))
            {
                throw std::runtime_error(
                    "Variable already declared: " + varName);
            }

            symbolTable.declare(varName, "int");

            auto stmt = std::make_unique<StatementNode>();
            stmt->type = StmtType::VariableDeclaration;

            auto decl = std::make_unique<VariableDeclarationNode>();

            decl->type = "int";
            decl->name = nameToken.name;


            if (current.type == TokenType::ASSIGN)
            {
                advance();
                decl->value = expr();
            }

            stmt->varDeclNode = std::move(decl);

            if (current.type == TokenType::SEMI)
                advance();

            return stmt;
        }
    }
    //================ ASSIGNMENT & FUNCTION CALLS =================
    if (current.type == TokenType::IDENT)
    {
        Token name = current;
        std::string varName = name.name;

        advance();  // Consume IDENT

        // Check for function call: foo(...)
        if (current.type == TokenType::LPAREN)
        {
            // Parse function call (no variable check needed)
            auto call = std::make_unique<FunctionCallNode>();
            call->name = name.name;

            advance(); // (

            if (current.type != TokenType::RPAREN)
            {
                while (true)
                {
                    call->args.push_back(expr());

                    if (current.type == TokenType::COMMA)
                    {
                        advance();
                        continue;
                    }
                    break;
                }
            }

            if (current.type != TokenType::RPAREN)
                throw std::runtime_error("Expected )");

            advance(); // )

            if (current.type != TokenType::SEMI)
                throw std::runtime_error("Expected ; after function call");

            advance(); // ;

            auto stmt = std::make_unique<StatementNode>();
            stmt->type = StmtType::Expression;
            stmt->exprNode = std::move(call);

            return stmt;
        }

        // Check for increment/decrement (var++ or var--)
        if (current.type == TokenType::INC || current.type == TokenType::DEC)
        {
            // Variable must exist for increment/decrement
            if (!symbolTable.exists(varName)) {
                throw std::runtime_error("Undefined variable: " + varName);
            }

            TokenType op = current.type;
            advance(); // consume ++ or --

            if (current.type != TokenType::SEMI)
                throw std::runtime_error("Expected ; after increment/decrement");

            advance(); // consume ;

            auto unary = std::make_unique<UnaryNode>();
            unary->op = (op == TokenType::INC) ? OpType::INC : OpType::DEC;

            auto var = std::make_unique<VariableNode>();
            var->name = name.name;
            unary->expr = std::move(var);

            auto stmt = std::make_unique<StatementNode>();
            stmt->type = StmtType::Expression;
            stmt->exprNode = std::move(unary);

            return stmt;
        }

        // Handle assignments
        if (current.type == TokenType::ASSIGN ||
            current.type == TokenType::PLUS_ASSIGN ||
            current.type == TokenType::MINUS_ASSIGN)
        {
            // Variable must exist for assignment
            if (!symbolTable.exists(varName)) {
                throw std::runtime_error("Undefined variable: " + varName);
            }

            TokenType op = current.type;
            advance();

            auto stmt = std::make_unique<StatementNode>();
            stmt->type = StmtType::Assignment;

            auto a = std::make_unique<AssignmentNode>();
            a->variableName = name.name;

            if (op == TokenType::ASSIGN)
            {
                a->value = expr();
            }
            else
            {
                auto b = std::make_unique<BinaryNode>();
                b->left = std::make_unique<VariableNode>();
                static_cast<VariableNode*>(b->left.get())->name = name.name;
                b->right = expr();
                b->op = (op == TokenType::PLUS_ASSIGN) ? OpType::ADD : OpType::SUB;
                a->value = std::move(b);
            }

            stmt->assignNode = std::move(a);

            if (current.type != TokenType::SEMI)
                throw std::runtime_error("Expected ;");

            advance();

            return stmt;
        }

        // Check if variable exists
        if (!symbolTable.exists(varName)) {
            throw std::runtime_error("Undefined variable: " + varName);
        }

        // Expect semicolon
        if (current.type != TokenType::SEMI)
            throw std::runtime_error("Expected ; after variable expression");

        advance(); // consume ;

        auto var = std::make_unique<VariableNode>();
        var->name = name.name;

        auto stmt = std::make_unique<StatementNode>();
        stmt->type = StmtType::Expression;
        stmt->exprNode = std::move(var);

        return stmt;
    }

    //================ RETURN =================
    if (current.type == TokenType::RET)
    {
        auto stmt = std::make_unique<StatementNode>();
        stmt->type = StmtType::Ret;

        advance();

        auto r = std::make_unique<ReturnNode>();
        r->value = expr();

        stmt->returnNode = std::move(r);

        if (current.type != TokenType::SEMI)
            throw std::runtime_error("Expected ;");

        advance();
        return stmt;
    }

    //================ BLOCK =================
    if (current.type == TokenType::LBRACE)
    {
        symbolTable.enterScope();
        auto stmt = std::make_unique<StatementNode>();
        stmt->type = StmtType::Block;

        advance();

        auto block = std::make_unique<BlockNode>();

        while (current.type != TokenType::RBRACE)
        {
            auto stmt = statement();
            block->statements.push_back(std::move(stmt));
        }

        advance();

        symbolTable.exitScope();

        stmt->blockNode = std::move(block);
        return stmt;
    }

    //================ PRINT =================
    if (current.type == TokenType::PRINT)
    {
        auto stmt = std::make_unique<StatementNode>();
        stmt->type = StmtType::Print;

        advance(); // print

        if (current.type != TokenType::LPAREN)
            throw std::runtime_error("Expected (");

        advance();

        auto p = std::make_unique<PrintNode>();
        p->value = expr();

        if (current.type != TokenType::RPAREN)
            throw std::runtime_error("Expected )");

        advance();

        if (current.type == TokenType::SEMI)
            advance();

        stmt->printNode = std::move(p);

        return stmt;
    }
    //================ IF =================
    if (current.type == TokenType::IF)
    {
        auto stmt = std::make_unique<StatementNode>();
        stmt->type = StmtType::If;

        advance(); // if

        if (current.type != TokenType::LPAREN)
            throw std::runtime_error("Expected ( after if");

        advance();

        auto ifNode = std::make_unique<IfNode>();
        ifNode->condition = expr();

        if (current.type != TokenType::RPAREN)
            throw std::runtime_error("Expected ) after condition");

        advance();

        if (current.type != TokenType::LBRACE)
            throw std::runtime_error("Expected { after if");

        advance();

        auto body = std::make_unique<BlockNode>();

        while (current.type != TokenType::RBRACE)
        {
            auto stmt = statement();
            body->statements.push_back(std::move(stmt));
        }

        advance();

        ifNode->body = std::move(body);

        // Check for else or else if
        if (current.type == TokenType::ELSE)
        {
            advance(); // else

            // Check for else if
            if (current.type == TokenType::IF)
            {
                // Parse the else-if as a nested if statement
                auto elseIfStmt = statement();
                if (elseIfStmt->type == StmtType::If)
                {
                    ifNode->elseIf = std::move(elseIfStmt->ifNode);
                }
            }
            else
            {
                // Parse else body
                if (current.type != TokenType::LBRACE)
                    throw std::runtime_error("Expected { after else");

                advance();

                auto elseBody = std::make_unique<BlockNode>();
                while (current.type != TokenType::RBRACE)
                {
                    elseBody->statements.push_back(std::move(statement()));
                }
                advance();
                ifNode->elseBody = std::move(elseBody);
            }
        }

        stmt->ifNode = std::move(ifNode);

        return stmt;
    }
    //================ WHILE =================
    if (current.type == TokenType::WHILE)
    {
        auto stmt = std::make_unique<StatementNode>();
        stmt->type = StmtType::While;

        advance(); // while

        if (current.type != TokenType::LPAREN)
            throw std::runtime_error("Expected ( after while");

        advance();

        auto whileNode = std::make_unique<WhileNode>();
        whileNode->condition = expr();

        if (current.type != TokenType::RPAREN)
            throw std::runtime_error("Expected ) after condition");

        advance();

        if (current.type != TokenType::LBRACE)
            throw std::runtime_error("Expected { after while");

        advance();

        auto body = std::make_unique<BlockNode>();

        while (current.type != TokenType::RBRACE)
        {
            auto stmt = statement();
            body->statements.push_back(std::move(stmt));
        }

        advance();

        whileNode->body = std::move(body);

        stmt->whileNode = std::move(whileNode);

        return stmt;
    }

    throw std::runtime_error("Unknown statement");
}

