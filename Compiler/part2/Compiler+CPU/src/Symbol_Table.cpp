#include "Symbol_Table.h"

SymbolTable::SymbolTable() {
    enterScope();  // Global scope
    currentStackOffset = 0;
}

void SymbolTable::enterScope() {
    scopes.push_back({});
}

void SymbolTable::exitScope() {
    if (scopes.empty())
        throw std::runtime_error("No scope to exit");
    scopes.pop_back();
}

// ========== THIS WAS MISSING ==========
bool SymbolTable::existsInCurrentScope(const std::string& name) {
    if (scopes.empty())
        return false;
    return scopes.back().count(name) > 0;  // or find() != end()
}
// ======================================

bool SymbolTable::exists(const std::string& name) {
    return lookup(name) != nullptr;
}

Symbol* SymbolTable::lookup(const std::string& name) {
    // Search from innermost to outermost scope
    for (int i = scopes.size() - 1; i >= 0; i--) {
        auto it = scopes[i].find(name);
        if (it != scopes[i].end())
            return &it->second;
    }
    return nullptr;
}

int SymbolTable::allocateStackSlot() {
    currentStackOffset -= 4;  // Stack grows downward
    return currentStackOffset;
}

void SymbolTable::declare(const std::string& name, const std::string& type) {
    if (existsInCurrentScope(name))
        throw std::runtime_error("Variable already declared in this scope: " + name);

    Symbol sym;
    sym.name = name;
    sym.type = type;
    sym.stackOffset = allocateStackSlot();
    sym.isParameter = false;

    scopes.back()[name] = sym;
}

void SymbolTable::declareParameter(const std::string& name, int index) {
    Symbol sym;
    sym.name = name;
    sym.type = "int";
    sym.stackOffset = 8 + (index * 4);  // Parameters start at BP+8
    sym.isParameter = true;
    sym.paramIndex = index;

    scopes.back()[name] = sym;
}

void SymbolTable::setRegister(const std::string& name, const std::string& reg) {
    Symbol* sym = lookup(name);
    if (sym) {
        sym->reg = reg;
        sym->inRegister = true;
    }
}

void SymbolTable::clearRegister(const std::string& name) {
    Symbol* sym = lookup(name);
    if (sym) {
        sym->reg = "";
        sym->inRegister = false;
    }
}

std::string SymbolTable::getRegister(const std::string& name) {
    Symbol* sym = lookup(name);
    if (sym && sym->inRegister)
        return sym->reg;
    return "";
}
