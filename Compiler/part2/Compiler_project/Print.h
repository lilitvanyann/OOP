#pragma once
#include <vector>
#include <string>
#include "IR.h"
#include "Assembler.h"

void printIR(const std::vector<IRInstruction>& ir);
void printASM(const std::vector<AssemblyInstruction>& asmCode);
void printMC(const std::vector<uint32_t>& mc);
void saveMachineCode(const std::vector<uint32_t>& mc, const std::string& filename);
