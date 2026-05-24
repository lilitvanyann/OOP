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
    std::string getVarAddress(const std::string& varName);

    std::unordered_map<std::string, std::string> regMap;
    std::unordered_map<std::string, int> varMem;
    int regIndex;
    int labelCounter;
    int nextVarAddr;
    std::string currentFunction;
    bool inFuncPrologue;

};
