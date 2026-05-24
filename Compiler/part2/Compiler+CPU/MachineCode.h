#pragma once
#include <vector>
#include <string>
#include "Assembler.h"


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
#define OP_JNE   20  
#define OP_CMP   21
#define OP_EQ    22
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


class MachineCode
{
public:
    std::vector<uint32_t> generate(const std::vector<AssemblyInstruction>& asmCode);

private:
    uint32_t encode(const AssemblyInstruction& ins);

    uint32_t getOpcode(const std::string& op);
    uint32_t getReg(const std::string& r);
    void saveToFile(const std::vector<uint32_t>& binary,
        const std::string& filename);
};
