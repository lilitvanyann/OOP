#include "Generator.h"


void generateIR(Node* node, std::vector<Instruction>& ir) {
    if (!node) return;

    //  assignment (=)
    if (node->type == NodeType::OPERATOR &&
        node->op == OpType::NONE &&
        node->left && node->left->type == NodeType::VARIABLE) {

        generateIR(node->right, ir);

        ir.push_back({ InstrType::STORE, 0, node->left->var });
        ir.push_back({ InstrType::LOAD, 0, node->left->var });
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
