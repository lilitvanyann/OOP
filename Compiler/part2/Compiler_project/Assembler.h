#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "IR.h"

struct AssemblyInstruction
{
    std::string op;
    std::string rd;
    std::string rs1;
    std::string rs2;
    std::string extra;
};

class Assembler
{
public:
    Assembler();

    std::vector<AssemblyInstruction>
        generate(const std::vector<IRInstruction>& ir);

private:
    std::string getReg(const std::string& temp);

    std::unordered_map<std::string, std::string> regMap;
    int regIndex;
    int labelCounter;  // Add this
    std::string currentFunction;  // Add this to track current function

};
