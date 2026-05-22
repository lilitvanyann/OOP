#pragma once
#include "Parser.h" //for symb_table
#include "Generator.h"

int executeIR(std::vector<Instruction>& ir, SymbolTable& sym);
