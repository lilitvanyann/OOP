#include "CPU.h"
#include <iostream>
#include <fstream>
#include <stdexcept>

CPU::CPU() { reset(); }

void CPU::reset() {
    for (int i = 0; i < 8; i++) R[i] = 0;
    PC = 0;
    SP = 65536;   // stack at top of memory
    RA = 0;
    zero_flag = false;
    negative_flag = false;
    running = true;

    memory.assign(65536, 0); // fresh memory
}

void CPU::dumpRegisters() const {
    std::cout << "\n=== CPU Registers ===\n";
    for (int i = 0; i < 8; i++) {
        std::cout << "R" << i << ": " << R[i] << "\n";
    }
    std::cout << "PC=" << PC << " SP=" << SP << " RA=" << RA
        << " Flags: Z=" << zero_flag << " N=" << negative_flag << "\n";
}

CPUSimulator::CPUSimulator() {}
CPUSimulator::~CPUSimulator() { program.clear(); cpu.memory.clear(); }

void CPUSimulator::loadProgram(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in) throw std::runtime_error("Cannot open file: " + filename);

    program.clear();
    uint32_t instr;
    while (in.read(reinterpret_cast<char*>(&instr), sizeof(uint32_t))) {
        program.push_back(instr);
    }
    in.close();

    for (size_t i = 0; i < program.size(); i++) {
        storeWord(i * 4, program[i]);
    }

    cpu.PC = 0;
    cpu.running = true;
}

uint32_t CPUSimulator::loadWord(uint32_t address) {
    if (address + 3 >= cpu.memory.size())
        throw std::runtime_error("Memory access violation");
    // little-endian
    return cpu.memory[address] |
        (cpu.memory[address + 1] << 8) |
        (cpu.memory[address + 2] << 16) |
        (cpu.memory[address + 3] << 24);
}

void CPUSimulator::storeWord(uint32_t address, uint32_t value) {
    if (address + 3 >= cpu.memory.size())
        throw std::runtime_error("Memory access violation");
    cpu.memory[address] = value & 0xFF;
    cpu.memory[address + 1] = (value >> 8) & 0xFF;
    cpu.memory[address + 2] = (value >> 16) & 0xFF;
    cpu.memory[address + 3] = (value >> 24) & 0xFF;
}

void CPUSimulator::push(uint32_t value) {
    cpu.SP -= 4;
    storeWord(cpu.SP, value);
}

uint32_t CPUSimulator::pop() {
    uint32_t value = loadWord(cpu.SP);
    cpu.SP += 4;
    return value;
}

uint32_t CPUSimulator::fetch() {
    if (cpu.PC + 3 >= cpu.memory.size()) {
        cpu.running = false;
        return 0;
    }
    return loadWord(cpu.PC);
}

void CPUSimulator::execute(uint32_t instr) {
    uint8_t opcode = (instr >> 24) & 0xFF;
    uint8_t rd = (instr >> 16) & 0xFF;
    uint8_t rs1 = (instr >> 8) & 0xFF;
    uint8_t rs2 = instr & 0xFF;

    bool jump = false;

    switch (opcode) {
    case OP_ADD: cpu.R[rd] = cpu.R[rs1] + cpu.R[rs2]; break;
    case OP_SUB: cpu.R[rd] = cpu.R[rs1] - cpu.R[rs2]; break;
    case OP_MUL: cpu.R[rd] = cpu.R[rs1] * cpu.R[rs2]; break;
    case OP_DIV: if (cpu.R[rs2] == 0) throw std::runtime_error("Div by 0");
        cpu.R[rd] = cpu.R[rs1] / cpu.R[rs2]; break;

    case OP_LI:  cpu.R[rd] = rs1; break;

    case OP_LOAD: cpu.R[rd] = loadWord(cpu.R[rs1]); break;
    case OP_STORE: storeWord(cpu.R[rs1], cpu.R[rd]); break; // fixed

    case OP_JMP: cpu.PC = cpu.R[rd]; jump = true; break;
    case OP_CALL: cpu.RA = cpu.PC + 4; cpu.PC = cpu.R[rd]; jump = true; break;
    case OP_RET: cpu.PC = cpu.RA; jump = true; break;

    case OP_CMP: {
        int32_t result = cpu.R[rs1] - cpu.R[rs2];
        cpu.zero_flag = (result == 0);
        cpu.negative_flag = (result < 0);
    } break;

    case OP_JLT: if (cpu.negative_flag) { cpu.PC = cpu.R[rd]; jump = true; } break;
    case OP_JGT: if (!cpu.negative_flag && !cpu.zero_flag) { cpu.PC = cpu.R[rd]; jump = true; } break;
    case OP_JEQ: if (cpu.zero_flag) { cpu.PC = cpu.R[rd]; jump = true; } break;
    case OP_JNE: if (!cpu.zero_flag) { cpu.PC = cpu.R[rd]; jump = true; } break;

    case OP_AND: cpu.R[rd] = cpu.R[rs1] & cpu.R[rs2]; break;
    case OP_OR:  cpu.R[rd] = cpu.R[rs1] | cpu.R[rs2]; break;
    case OP_NOT: cpu.R[rd] = (cpu.R[rs1] == 0) ? 1 : 0; break;

    case OP_MOV: cpu.R[rd] = cpu.R[rs1]; break;

    case OP_PUSH: push(cpu.R[rs1]); break;
    case OP_POP:  cpu.R[rd] = pop(); break;

    case OP_OUT: std::cout << cpu.R[rd] << std::endl; break;

    case OP_HALT: cpu.running = false; break;

    default: throw std::runtime_error("Unknown opcode");
    }

    if (!jump) cpu.PC += 4;
}

void CPUSimulator::run(int maxInstructions) {
    int count = 0;
    while (cpu.running && count < maxInstructions) {
        uint32_t instr = fetch();
        if (instr == 0) { cpu.PC += 4; continue; }
        execute(instr);
        count++;
    }
    cpu.dumpRegisters();
}

void CPUSimulator::step() {
    if (!cpu.running) return;
    uint32_t instr = fetch();
    if (instr == 0) {
        cpu.PC += 4;
    }
}
