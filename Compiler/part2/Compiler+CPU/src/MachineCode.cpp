#include "MachineCode.h"
#include <stdexcept>
#include <fstream>
#include <iomanip>

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

    // beyond 255 bytes are representable.
    if (ins.op == "J" || ins.op == "JLE" || ins.op == "JGE" ||
        ins.op == "JLT" || ins.op == "JGT" || ins.op == "JEQ" ||
        ins.op == "JNE" || ins.op == "BEQZ" || ins.op == "BNEZ" ||
        ins.op == "CALL")
    {
        uint32_t addr = 0;
        if (!ins.rd.empty() && (isdigit((unsigned char)ins.rd[0]))) {
            addr = static_cast<uint32_t>(std::stoul(ins.rd));
        }
        return (opcode << 24) | (addr & 0xFFFFFF);
    }

    // LI uses a 16-bit immediate in the lower 16 bits so addresses up to 65535 work.
    if (ins.op == "LI") {
        uint32_t rd = getReg(ins.rd);
        uint32_t imm = static_cast<uint32_t>(std::stoi(ins.rs1)) & 0xFFFF;
        return (opcode << 24) | (rd << 16) | imm;
    }

    uint32_t rd = getReg(ins.rd);
    uint32_t rs1 = getReg(ins.rs1);
    uint32_t rs2 = getReg(ins.rs2);

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
    if (op == "JNEQ") return OP_JNE;
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
