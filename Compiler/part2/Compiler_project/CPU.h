#pragma once
#include <vector>
#include <cstdint>
#include <string>

// Opcode definitions
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
#define OP_AND   29
#define OP_OR    30
#define OP_NOT   31
#define OP_PUSH  33
#define OP_POP   34
#define OP_HALT 255

struct CPU {
    uint32_t R[8];   // General registers R0–R7
    uint32_t PC;     // Program Counter
    uint32_t SP;     // Stack Pointer
    uint32_t RA;     // Return Address

    std::vector<uint8_t> memory;
    bool zero_flag;
    bool negative_flag;
    bool running;

    CPU();
    void reset();
    void dumpRegisters() const;
};

class CPUSimulator {
public:
    CPUSimulator();
    ~CPUSimulator();

    void loadProgram(const std::string& filename);
    void run(int maxInstructions = 10000);
    void step();
    bool isRunning() const;
    uint32_t getRegister(int idx) const;

private:
    CPU cpu;
    std::vector<uint32_t> program;

    uint32_t loadWord(uint32_t address);
    void storeWord(uint32_t address, uint32_t value);
    void push(uint32_t value);
    uint32_t pop();
    uint32_t fetch();
    void execute(uint32_t instr);
};
