#include "MachineCode.h"
#include <stdexcept>
#include <fstream>
#include <iomanip>

#define OP_ADD   1
#define OP_SUB   2
#define OP_MUL   3
#define OP_DIV   4
#define OP_LI    5
#define OP_LOAD  6
#define OP_STORE 7
#define OP_JMP   8
#define OP_BEQZ  9
#define OP_BNEZ  10
#define OP_CALL  11
#define OP_RET   12
#define OP_OUT   13
#define OP_MOV   14
#define OP_JLT   15
#define OP_JGT   16
#define OP_JLE   17
#define OP_JGE   18
#define OP_JEQ   19
#define OP_JNEQ   20
#define OP_CMP   21
#define OP_EQ   22
#define OP_JNE   23
#define OP_NEQ   24 
#define OP_LT    25
#define OP_GT    26
#define OP_LE    27
#define OP_GE    28
#define OP_AND   29
#define OP_OR    30
#define OP_NOT   31
#define OP_PARAM 32
#define OP_PUSH  33
#define OP_POP   34
#define OP_HALT 255



// MAIN GENERATOR
std::vector<uint32_t>
MachineCode::generate(const std::vector<AssemblyInstruction>& asmCode)
{

    std::vector<uint32_t> binary;

    for (const auto& ins : asmCode)
    {
        binary.push_back(encode(ins));
    }

    return binary;
}

// ENCODE ONE INSTRUCTION
// [ opcode:8 ][ rd:8 ][ rs1:8 ][ rs2:8 ]

uint32_t MachineCode::encode(const AssemblyInstruction& ins)
{
    if (ins.op == "LABEL") {
        return 0;
    }

    uint32_t opcode = getOpcode(ins.op);
    uint32_t rd = getReg(ins.rd);

    uint32_t rs1 = 0;
    uint32_t rs2 = getReg(ins.rs2);

    if (ins.op == "LI") {
        rs1 = std::stoi(ins.rs1);
    }
    else {
        rs1 = getReg(ins.rs1);
    }

    return (opcode << 24) | (rd << 16) | (rs1 << 8) | (rs2);
}

// OPCODE MAP

uint32_t MachineCode::getOpcode(const std::string& op)
{
    if (op == "ADD")   return OP_ADD;
    if (op == "SUB")   return OP_SUB;
    if (op == "MUL")   return OP_MUL;
    if (op == "DIV")   return OP_DIV;

    if (op == "LI")    return OP_LI;
    if (op == "LOAD")  return OP_LOAD;
    if (op == "STORE") return OP_STORE;

    if (op == "J")     return OP_JMP;
    if (op == "BEQZ")  return OP_BEQZ;
    if (op == "BNEZ")  return OP_BNEZ;

    if (op == "CALL")  return OP_CALL;
    if (op == "RET")   return OP_RET;

    if (op == "OUT")   return OP_OUT;
    if (op == "MOV")   return OP_MOV;

    if (op == "JLT") return OP_JLT;
    if (op == "JGT") return OP_JGT;
    if (op == "JLE") return OP_JLE;
    if (op == "JGE") return OP_JGE;
    if (op == "JEQ") return OP_JEQ;
    if (op == "JNEQ") return OP_JNEQ;
    if (op == "CMP") return OP_CMP;
    if (op == "LABEL") return 0;  // No-op

    if (op == "EQ") return OP_EQ;  
    if (op == "JNE") return OP_JNE;
    if (op == "NEQ") return OP_NEQ;
    if (op == "LT") return OP_LT;
    if (op == "GT") return OP_GT;
    if (op == "LE") return OP_LE;
    if (op == "GE") return OP_GE;

    if (op == "AND") return OP_AND;
    if (op == "OR")  return OP_OR;
    if (op == "NOT") return OP_NOT;
    if (op == "PARAM") return OP_PARAM;

    if (op == "PUSH")  return OP_PUSH;
    if (op == "POP")   return OP_POP;

    if (op == "HALT") return OP_HALT;
    throw std::runtime_error("Unknown opcode: " + op);
}


// REGISTER MAP
// R0–R7 supported

uint32_t MachineCode::getReg(const std::string& r)
{
    if (r.empty()) return 0;

    if (r[0] >= '0' && r[0] <= '9') {
        return std::stoi(r); 
    }

    if (r == "BP") return 29;
    if (r == "SP") return 30;
    if (r[0] == 'R' && r.length() == 2) {
        return r[1] - '0';
    }

    return 0;
}
//----------------machine_code_vector----TO---FILE

void MachineCode::saveToFile(const std::vector<uint32_t>& binary,
    const std::string& binaryfile)
{
    std::ofstream out(binaryfile, std::ios::binary);

    if (!out)
        throw std::runtime_error("Cannot open file");

    int instructionCount = 0;
    for (uint32_t instr : binary)
    {
        if (instr == 0 || instr == 0xFFFFFFFF) continue;
        out.write(reinterpret_cast<const char*>(&instr), sizeof(uint32_t));
        instructionCount++;  
    }

    out.close();
    std::cout << "Saved " << instructionCount << " instructions to " << binaryfile << std::endl;
}
