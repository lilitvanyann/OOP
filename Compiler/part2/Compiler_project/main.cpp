#include <iostream>
#include <fstream>
#include "Lexer.h"
#include "Parser.h"
#include "IR.h"
#include "Assembler.h"
#include "MachineCode.h"
#include "print.h"
#include "cpu.h"

int main()
{
    try
    {
        // 1. READ SOURCE FILE
        std::ifstream file("program1.txt");
        if (!file.is_open()) {
            std::cout << "Cannot open program1.txt\n";
            return 1;
        }

        std::string source((std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>());

        // 2. LEXER
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.tokenize();

        // 3. PARSER
        Parser parser;
        auto ast = parser.parse(tokens);

        // 4. IR GENERATION
        IR irGen(&parser.getSymbolTable());  
        auto ir = irGen.generate(ast.get());
        printIR(ir);

        
        // 5. ASSEMBLER
        Assembler asmGen;
        auto asmCode = asmGen.generate(ir);
        printASM(asmCode);

        // 6. MACHINE CODE
        MachineCode mcGen;
        auto machineCode = mcGen.generate(asmCode);
        printMC(machineCode);

        // 7. SAVE TO BINARY FILE
        saveMachineCode(machineCode, "binaryfile.bin");

        // 8. RUN CPU SIMULATOR
        CPUSimulator cpu;
        cpu.loadProgram("program.bin");
        cpu.run();

        std::cout << "\n===== COMPILATION & EXECUTION SUCCESS =====\n";
    }
    catch (const std::exception& e)
    {
        std::cout << "ERROR: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
