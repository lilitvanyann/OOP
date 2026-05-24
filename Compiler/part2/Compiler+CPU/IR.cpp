#include "IR.h"

std::string IR::newTemp()
{
    return "t" + std::to_string(tempCount++);
}
std::string IR::newLabel()
{
    return "L" + std::to_string(labelCount++);
}
IR::IR(SymbolTable* symTable) : symbolTable(symTable) {
    // constructor
}
std::vector<IRInstruction> IR::generate(BlockNode* root)
{
    instructions.clear();

    tempCount = 0;
    labelCount = 0;

    for (auto& stmt : root->statements)
    {
        visitStatement(stmt.get());
    }

    return instructions;
}

void IR::visitIf(IfNode* node, const std::string& endLabel)
{
    std::string elseLabel = newLabel();

    emitCondition(node->condition.get(), elseLabel);

    for (auto& s : node->body->statements)
        visitStatement(s.get());

    instructions.push_back({ "JMP", "", "", endLabel, "" });
    instructions.push_back({ "LABEL", "", "", elseLabel, "" });

    if (node->elseIf)
    {
        visitIf(node->elseIf.get(), endLabel);
    }
    else if (node->elseBody)
    {
        for (auto& s : node->elseBody->statements)
            visitStatement(s.get());
    }

    if (!node->elseIf) {
        instructions.push_back({ "LABEL", "", "", endLabel, "" });
    }
}

void IR::emitCondition(ExpressionNode* expr, const std::string& falseLabel)
{
    // CASE 1: Binary conditions

            // single variable condition: if (a)
    if (expr->type == ExprType::Variable)
    {
        std::string v = visitExpr(expr);

        instructions.push_back({ "CMP", v, "0", "", "" });
        instructions.push_back({ "JEQ", "", "", falseLabel, "" });
        return;
    }
    if (expr->type == ExprType::Binary)
    {
        auto* node = static_cast<BinaryNode*>(expr);

        // LEFT and RIGHT values
        std::string l = visitExpr(node->left.get());
        std::string r = visitExpr(node->right.get());

        switch (node->op)
        {
        case OpType::GT:
            instructions.push_back({ "CMP", l, r, "", "" });
            instructions.push_back({ "JLE", "", "", falseLabel, "" });
            break;

        case OpType::LT:
            instructions.push_back({ "CMP", l, r, "", "" });
            instructions.push_back({ "JGE", "", "", falseLabel, "" });
            break;

        case OpType::GE:
            instructions.push_back({ "CMP", l, r, "", "" });
            instructions.push_back({ "JLT", "", "", falseLabel, "" });
            break;

        case OpType::LE:
            instructions.push_back({ "CMP", l, r, "", "" });
            instructions.push_back({ "JGT", "", "", falseLabel, "" });
            break;

        case OpType::EQ:
            instructions.push_back({ "CMP", l, r, "", "" });
            instructions.push_back({ "JNE", "", "", falseLabel, "" });
            break;

        case OpType::NEQ:
            instructions.push_back({ "CMP", l, r, "", "" });
            instructions.push_back({ "JEQ", "", "", falseLabel, "" });
            break;

            // AND (a && b)

        case OpType::AND:
        {
            emitCondition(node->left.get(), falseLabel);
            emitCondition(node->right.get(), falseLabel);
            return;
        }

        // OR (a || b): if left is TRUE skip the right check; if right is FALSE → fail
        case OpType::OR:
        {
            std::string checkRight = newLabel();
            std::string endOr = newLabel();

            emitCondition(node->left.get(), checkRight);
            instructions.push_back({ "JMP", "", "", endOr, "" });

            instructions.push_back({ "LABEL", "", "", checkRight, "" });
            emitCondition(node->right.get(), falseLabel);

            instructions.push_back({ "LABEL", "", "", endOr, "" });
            break;
        }

        default:
            throw std::runtime_error("Unsupported binary condition");
        }

        return;
    }

    // CASE 2: NOT (!a)

    if (expr->type == ExprType::Unary)
    {
        auto* node = static_cast<UnaryNode*>(expr);

        if (node->op == OpType::NOT)
        {
            std::string v = visitExpr(node->expr.get());

            instructions.push_back({ "CMP", v, "0", "", "" });
            instructions.push_back({ "JNE", "", "", falseLabel, "" });
            return;
        }
    }

    throw std::runtime_error("Unsupported condition expression");
}

std::string IR::visitExpr(ExpressionNode* expr)
{
    // NUMBER
    if (expr->type == ExprType::Number)
    {
        std::string t = newTemp();

        instructions.push_back({
            "NUMBER",
            std::to_string(static_cast<NumberNode*>(expr)->value),
            "",
            t,
            "int"
            });

        return t;
    }


    // VARIABLE
    if (expr->type == ExprType::Variable)
    {
        std::string t = newTemp();
        instructions.push_back({
            "LOAD",
            static_cast<VariableNode*>(expr)->name,
            "",
            t,
            "int"
            });
        return t;
    }


    // BINARY
    if (expr->type == ExprType::Binary)
    {
        auto* node = static_cast<BinaryNode*>(expr);

        if (node->op == OpType::AND || node->op == OpType::OR)
        {
            throw std::runtime_error("Logical operators can only be used in conditions, not expressions");
        }


        std::string l = visitExpr(node->left.get());
        std::string r = visitExpr(node->right.get());

        std::string t = newTemp();

        std::string op;
        switch (node->op)
        {
        case OpType::ADD: op = "ADD"; break;
        case OpType::SUB: op = "SUB"; break;
        case OpType::MUL: op = "MUL"; break;
        case OpType::DIV: op = "DIV"; break;

        case OpType::EQ:  op = "EQ";  break;
        case OpType::NEQ: op = "NEQ"; break;
        case OpType::LT:  op = "LT";  break;
        case OpType::GT:  op = "GT";  break;
        case OpType::LE:  op = "LE";  break;
        case OpType::GE:  op = "GE";  break;

            // a += b
        case OpType::PLUS_ASSIGN:
        {
            std::string l = visitExpr(node->left.get());
            std::string r = visitExpr(node->right.get());

            std::string t = newTemp();

            instructions.push_back({ "ADD", l, r, t, "int" });

            instructions.push_back({
                "ASSIGN",
                t,
                "",
                static_cast<VariableNode*>(node->left.get())->name,
                "int"
                });

            return t;
        }

        // a -= b
        case OpType::MINUS_ASSIGN:
        {
            std::string l = visitExpr(node->left.get());
            std::string r = visitExpr(node->right.get());

            std::string t = newTemp();

            instructions.push_back({ "SUB", l, r, t, "int" });

            instructions.push_back({
                "ASSIGN",
                t,
                "",
                static_cast<VariableNode*>(node->left.get())->name,
                "int"
                });

            return t;
        }

        default:
            throw std::runtime_error("unknown op");
        }

        instructions.push_back({
            op,
            l,
            r,
            t,
            "int"
            });

        return t;
    }

    // INC (++var) and (var++)
    if (expr->type == ExprType::Unary && static_cast<UnaryNode*>(expr)->op == OpType::INC)
    {
        auto* node = static_cast<UnaryNode*>(expr);

        // Get the variable name from the inner expression
        std::string varName;
        if (node->expr->type == ExprType::Variable) {
            varName = static_cast<VariableNode*>(node->expr.get())->name;
        }
        else {
            throw std::runtime_error("INC can only be applied to variables");
        }

        // Load current value
        std::string loadTemp = newTemp();
        instructions.push_back({
            "LOAD",
            varName,
            "",
            loadTemp,
            "int"
            });

        // Create constant 1
        std::string one = newTemp();
        instructions.push_back({ "NUMBER", "1", "", one, "int" });

        // Add 1
        std::string result = newTemp();
        instructions.push_back({ "ADD", loadTemp, one, result, "int" });

        // Store back to variable
        instructions.push_back({
            "ASSIGN",
            result,
            "",
            varName,
            "int"
            });

        return result;
    }
    // NOT (!var)
    if (expr->type == ExprType::Unary && static_cast<UnaryNode*>(expr)->op == OpType::NOT)
    {
        auto* node = static_cast<UnaryNode*>(expr);

        // Get the value
        std::string val = visitExpr(node->expr.get());

        // Create temporaries
        std::string result = newTemp();
        std::string zero = newTemp();
        std::string one = newTemp();

        // Load constants
        instructions.push_back({ "NUMBER", "0", "", zero, "int" });
        instructions.push_back({ "NUMBER", "1", "", one, "int" });

        // Compare val with 0
        instructions.push_back({ "CMP", val, zero, "", "" });

        // If equal, result = 1, else result = 0
        std::string labelTrue = newLabel();
        std::string labelEnd = newLabel();

        instructions.push_back({ "JEQ", "", "", labelTrue, "" });
        instructions.push_back({ "MOV", result, zero, "", "" });
        instructions.push_back({ "JMP", "", "", labelEnd, "" });
        instructions.push_back({ "LABEL", "", "", labelTrue, "" });
        instructions.push_back({ "MOV", result, one, "", "" });
        instructions.push_back({ "LABEL", "", "", labelEnd, "" });

        return result;
    }
    // DEC (--var) and (var--)
    if (expr->type == ExprType::Unary && static_cast<UnaryNode*>(expr)->op == OpType::DEC)
    {
        auto* node = static_cast<UnaryNode*>(expr);

        std::string varName;
        if (node->expr->type == ExprType::Variable) {
            varName = static_cast<VariableNode*>(node->expr.get())->name;
        }
        else {
            throw std::runtime_error("DEC can only be applied to variables");
        }

        // Load current value
        std::string loadTemp = newTemp();
        instructions.push_back({
            "LOAD",
            varName,
            "",
            loadTemp,
            "int"
            });

        // Create constant 1
        std::string one = newTemp();
        instructions.push_back({ "NUMBER", "1", "", one, "int" });

        // Subtract 1
        std::string result = newTemp();
        instructions.push_back({ "SUB", loadTemp, one, result, "int" });

        instructions.push_back({
            "ASSIGN",
            result,
            "",
            varName,
            "int"
            });

        return result;
    }

    // FUNCTION CALL
    if (expr->type == ExprType::Call)
    {
        auto* call = static_cast<FunctionCallNode*>(expr);

        int paramIndex = 0;

        for (auto& arg : call->args)
        {
            std::string v = visitExpr(arg.get());

            instructions.push_back(
                { "PARAM", v, std::to_string(paramIndex++), "", "int" });
        }

        // Make the call
        instructions.push_back({
            "CALL",
            call->name,
            "",
            "",
            ""
            });

        std::string retTemp = newTemp();
        instructions.push_back({
            "MOV",
            retTemp,
            "R0",
            "",
            "int"
            });

        return retTemp;
    }

    throw std::runtime_error("bad expr");
}

void IR::visitStatement(StatementNode* stmt)
{
    switch (stmt->type)
    {
        //================ ASSIGNMENT =================
    case StmtType::Assignment:
    {
        auto* asg = stmt->assignNode.get();

        std::string rhs = visitExpr(asg->value.get());

        instructions.push_back({
            "ASSIGN",
            rhs,
            "",
            asg->variableName,
            "int"
            });

        break;
    }
    //================ EXPRESSION =================
    case StmtType::Expression:
    {
        visitExpr(stmt->exprNode.get());
        break;
    }
    //================ PRINT =================
    case StmtType::Print:
    {
        auto* pr = stmt->printNode.get();

        std::string v = visitExpr(pr->value.get());

        instructions.push_back({
            "PRINT",
            v,
            "",
            "",
            "int"
            });

        break;
    }

    //================ VARIABLE DECL =================
    case StmtType::VariableDeclaration:
    {
        auto* v = stmt->varDeclNode.get();

        if (v->value)
        {
            std::string r = visitExpr(v->value.get());

            instructions.push_back({
                "ASSIGN",
                r,
                "",
                v->name,
                "int"
                });
        }

        break;
    }

    //================ RETURN =================
    case StmtType::Ret:
    {
        auto* r = stmt->returnNode.get();
        std::string v = visitExpr(r->value.get());

        instructions.push_back({
            "RET",
            v,
            "",
            "",
            "int"
            });
        break;
    }

    //================ IF =================
    case StmtType::If:
    {
        auto* ifNode = stmt->ifNode.get();
        std::string endLabel = newLabel();
        visitIf(ifNode, endLabel);
        break;
    }

    //================ WHILE =================
    case StmtType::While:
    {
        auto* w = stmt->whileNode.get();

        std::string start = newLabel();
        std::string end = newLabel();

        instructions.push_back({ "LABEL", "", "", start, "" });
        emitCondition(w->condition.get(), end);

        for (auto& s : w->body->statements)
            visitStatement(s.get());

        instructions.push_back({ "JMP", "", "", start, "" });
        instructions.push_back({ "LABEL", "", "", end, "" });

        break;
    }

    //================ BLOCK =================
    case StmtType::Block:
    {
        auto* b = stmt->blockNode.get();

        for (auto& s : b->statements)
            visitStatement(s.get());

        break;
    }

    //================ FUNCTION =================
    case StmtType::Function:
    {
        auto* f = stmt->functionNode.get();
        int localSize = 0;  

        IRInstruction funcBegin;
        funcBegin.op = "FUNC_BEGIN";
        funcBegin.result = f->name;
        funcBegin.localSize = localSize;
        instructions.push_back(funcBegin);


        // Parameters
        int i = 0;
        for (auto& p : f->parameters)
        {
            instructions.push_back({
                "PARAM",
                p.name,
                std::to_string(i),
                "",
                "int"
                });
            i++;
        }

        // Function body
        for (auto& s : f->body->statements)
            visitStatement(s.get());

        bool hasReturn = false;
        for (auto& s : f->body->statements) {
            if (s->type == StmtType::Ret) {
                hasReturn = true;
                break;
            }
        }

        if (!hasReturn && f->name != "main") {
            instructions.push_back({
                "RET",
                "0",
                "",
                "",
                "int"
                });
        }

        instructions.push_back({
            "FUNC_END",
            "",
            "",
            f->name,
            ""
            });

        break;
    }

    default:
        throw std::runtime_error("Unsupported statement type in IR");
    }
}
