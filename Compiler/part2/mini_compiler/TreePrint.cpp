#include <iostream>
#include "TreePrint.h"
using namespace std;
//--Tree-print--

void printTree(Node* node, int depth) {
    if (!node) return;

    // right first
    printTree(node->right, depth + 1);

    // indent
    for (int i = 0; i < depth; i++)
        cout << "    ";

    // node value
    if (node->type == NodeType::NUMBER)
        cout << node->value;
    else if (node->type == NodeType::VARIABLE)
        cout << node->var;
    else {
        if (node->op == OpType::ADD) cout << "+";
        if (node->op == OpType::SUB) cout << "-";
        if (node->op == OpType::MUL) cout << "*";
        if (node->op == OpType::DIV) cout << "/";
    }

    cout << endl;

    // left
    printTree(node->left, depth + 1);
}


void PrettyPrint(Node* node, string prefix, bool isLast)
{
    if (node == nullptr) return;

    cout << prefix;

    if (isLast) {
        cout << "└──";
        prefix += "   ";
    }
    else {
        cout << "├──";
        prefix += "│  ";
    }

    // print node
    if (node->type == NodeType::NUMBER)
        cout << node->value;
    else if (node->type == NodeType::VARIABLE)
        cout << node->var;
    else {
        switch (node->op) {
        case OpType::ADD: cout << "+"; break;
        case OpType::SUB: cout << "-"; break;
        case OpType::MUL: cout << "*"; break;
        case OpType::DIV: cout << "/"; break;
        default: cout << "?";
        }
    }

    cout << endl;

    // children count check
    if (node->left || node->right) {

        // IMPORTANT: right first (for nice tree view)
        PrettyPrint(node->right, prefix, node->left == nullptr);
        PrettyPrint(node->left, prefix, true);
    }

}
