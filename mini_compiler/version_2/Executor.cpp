#include "Executor.h"

int executeIR(std::vector<Instruction>& ir, SymbolTable& sym) {
    std::vector<int> stack;

    for (auto& ins : ir) {

        switch (ins.type) {

        case InstrType::PUSH:
            stack.push_back(ins.value);
            break;

        case InstrType::LOAD:
            stack.push_back(sym.get(ins.name));
            break;

        case InstrType::ADD: {
            int b = stack.back(); stack.pop_back();
            int a = stack.back(); stack.pop_back();
            stack.push_back(a + b);
            break;
        }

        case InstrType::SUB: {
            int b = stack.back(); stack.pop_back();
            int a = stack.back(); stack.pop_back();
            stack.push_back(a - b);
            break;
        }

        case InstrType::MUL: {
            int b = stack.back(); stack.pop_back();
            int a = stack.back(); stack.pop_back();
            stack.push_back(a * b);
            break;
        }

        case InstrType::DIV: {
            int b = stack.back(); stack.pop_back();
            int a = stack.back(); stack.pop_back();
            stack.push_back(a / b);
            break;
        }

        case InstrType::STORE: {
            int val = stack.back();
            stack.pop_back();
            sym.set( ins.name, val);

            stack.push_back(val); 
            break;
        }
        }
    }

    return stack.empty() ? 0 : stack.back();
}
