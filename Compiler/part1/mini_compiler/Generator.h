#pragma once
#include <vector>
#include "Parser.h"   // Node, NodeType, OpType-ի համար

enum class InstrType {
    PUSH, ADD, SUB, MUL, DIV,
    LOAD, STORE
};

struct Instruction {
    InstrType type;
    int value = 0;
    std::string name = "";
};

// only declaration
void generateIR(Node* node, std::vector<Instruction>& ir);
