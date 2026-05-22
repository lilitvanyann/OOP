#pragma once
#include <vector>
#include <string>
#include "Assembler.h"

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

