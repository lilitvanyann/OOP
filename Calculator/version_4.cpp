
#include <iostream>
using namespace std;

enum class OpType { NONE, ADD, SUB, MUL, DIV };

struct Node{
    OpType op;
    int value; //if digit
    Node* right;
    Node* left;

    //for operand node
    Node(int v) : op(OpType::NONE), value(v), left(nullptr), right(nullptr) {}

    //for operator node
    Node(OpType o, Node* l, Node* r) : op(o), value(0), left(l), right(r) {}

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

    int evaluate(Node* node) {
        if (node->op == OpType::NONE) {
            // leaf node ? number
            return node->value;
        }

        // recursive evaluation of left and right
        int leftVal = evaluate(node->left);
        int rightVal = evaluate(node->right);

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
    string input;
    cout << "your expression : ";
    getline(cin, input);
    Calculator_AST calc;
    Node* root = calc.parse(input);
    int result = calc.evaluate(root);
    cout <<"result :"<< result;
        
}

