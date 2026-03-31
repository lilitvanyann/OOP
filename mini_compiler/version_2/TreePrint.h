#pragma once
#include <string>
#include "Parser.h"

void printTree(Node* node, int depth);
void PrettyPrint(Node* node, std::string indent="", bool isLast=true);
