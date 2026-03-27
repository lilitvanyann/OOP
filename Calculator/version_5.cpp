```cpp
#include <iostream>
#include <string>
#include <map>
#include <stdexcept>
using namespace std;

enum class OpType { NONE, ADD, SUB, MUL, DIV };

class SymbolTable {
private:
    map <char,int> table; // key->char, value-> int

public:
    void set(const char& name, int value) {
        table[name] = value;
    }

    int get(const char& name) const {
        auto it = table.find(name);
        if (it != table.end()) return it->second; //val
        throw std::runtime_error("Unknown variable: " + string(1, name));
    }

    bool exists(const char& name) const {
        return table.find(name) != table.end();
    }
};

struct Node{
    OpType op;
    char var;
    int value; //if digit
    Node* right;
    Node* left;

    //for operand node
    Node(int v) : op(OpType::NONE), value(v), var('\0'), left(nullptr), right(nullptr) {}

    //for operator node
    Node(OpType o, Node* l, Node* r) : op(o), value(0), var('\0'), left(l), right(r) {}

    //for variable
    Node(char c) : op(OpType::NONE), value(0), var(c), left(nullptr), right(nullptr) {}

};

class Calculator_AST {
private:
    string expr;
    int pos;

    Node* parse_sub_add() {

        Node* left = parse_mul_div();
        while (pos < expr.size() && (expr[pos] == '+' || expr[pos] == '-'))
        {
            OpType op = (expr[pos] == '+') ? OpType::ADD : OpType::SUB;
            pos++;
            Node* right = parse_mul_div();
            left = new Node(op, left, right);

        }
        return left;

    }

    Node* parse_mul_div() {

        Node* left = parse_val();
        while (pos < expr.size() && (expr[pos] == '*' || expr[pos] == '/'))
        {
            OpType op = (expr[pos] == '*') ? OpType::MUL : OpType::DIV;            pos++;
            Node* right = parse_val();
            left = new Node(op, left, right);
        }
        return left;

    }

    Node* parse_val() {
        if (expr[pos] == '(') {
            pos++;
            Node* node = parse_sub_add();
            if (pos >= expr.size() || expr[pos] != ')') {
                throw runtime_error("Mismatched parentheses");
            }
            pos++;  
            return node;
        }
        else 
            if (isalpha(expr[pos])) {
            char var = expr[pos];
            pos++;
            return new Node(var);
        }
        else
            return parse_number();

    }

   //if the number has more than one digit

    Node* parse_number() {
        int num = 0;
        while (pos < expr.size() && isdigit(expr[pos]))
        {
            num = num * 10 + (expr[pos] - '0');
            pos++;
        }
        return new Node(num);
    }
public:
    Node* parse(const string& s) {
        expr = s;   // set expression
        pos = 0;    // reset position
        return parse_sub_add();  // build the AST and return root
    }

    int evaluate(Node* node, const SymbolTable& symb) {
        if (!node)return 0;

        if (node->op == OpType::NONE) {
            if (node->var != '\0') return symb.get(node->var);
            return node->value;
        }

        // recursive evaluation of left and right
        int leftVal = evaluate(node->left, symb);
        int rightVal = evaluate(node->right, symb);

        switch (node->op) {
        case OpType::ADD: return leftVal + rightVal;
        case OpType::SUB: return leftVal - rightVal;
        case OpType::MUL: return leftVal * rightVal;
        case OpType::DIV:
            if (rightVal == 0) throw runtime_error("Division by zero");
            return leftVal / rightVal;
        default: return 0; // should not happen
        }
    }
};

int main()
{
    string input;int num1,num2;
    cout << "your expression : ";
    getline(cin, input);
    cout<<"your var1:";cin>>num1;
    cout<<"your var2:";cin>>num2;

    SymbolTable symb;
    symb.set('x', num1);
    symb.set('y', num2);

    Calculator_AST calc;

    Node* root = calc.parse(input);

    try {
        // evaluate 
        int result = calc.evaluate(root, symb);
        cout << "Result: " << result << endl;
    }
    catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }

    return 0;
        
}

```
