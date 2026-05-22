#include "print.h"
#include <iostream>
#include <fstream>
#include <iomanip>

void printIR(const std::vector<IRInstruction>& ir)
{
    std::cout << "\n===== IR =====\n";
    for (auto& i : ir)
    {
        std::cout << i.op << " "
            << i.arg1 << " "
            << i.arg2 << " "
            << i.result << "\n";
    }
}

void printASM(const std::vector<AssemblyInstruction>& asmCode)
{
    std::cout << "\n===== ASSEMBLY =====\n";
    for (auto& i : asmCode)
    {
        std::cout << i.op << " "
            << i.rd << " "
            << i.rs1 << " "
            << i.rs2 << " "
            << i.extra << "\n";
    }
}

void printMC(const std::vector<uint32_t>& mc)
{
    std::cout << "\n===== MACHINE CODE =====\n";
    for (auto& i : mc)
    {
        std::cout << "0x" << std::hex << std::setw(8)
            << std::setfill('0') << i << std::dec << "\n";
    }
}

void saveMachineCode(const std::vector<uint32_t>& mc, const std::string& filename)
{
    std::ofstream out(filename, std::ios::binary);
    if (!out) {
        throw std::runtime_error("Cannot create binary file: " + filename);
    }

    for (uint32_t instr : mc) {
        out.write(reinterpret_cast<const char*>(&instr), sizeof(uint32_t));
    }

    out.close();
    std::cout << "\nMachine code saved to: " << filename << std::endl;
}
