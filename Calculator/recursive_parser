#include <iostream>
using namespace std;

class Calculator_AST {
private:
    string expr;
    int pos;

    int parse_sub_add() {

        int value = parse_mul_div();
        while (pos < expr.size() && (expr[pos] == '+' || expr[pos] == '-'))
        {
            char op = expr[pos];
            pos++;

            if (op == '+')
                value = value + parse_mul_div();
            else
                value = value - parse_mul_div();
        }
        return value;

    }

    int parse_mul_div() {

        int value = parse_val();
        while (pos < expr.size() && (expr[pos] == '*' || expr[pos] == '/'))
        {
            char op = expr[pos];
            pos++;

            if (op == '*')
                value = value * parse_val();
            else
                value = value / parse_val();
        }
        return value;

    }

    int parse_val() {
        if (expr[pos] == '(') {
            pos++;
            int value = parse_sub_add();    //ռեկուրսիվ կանչ => ()-ի մեջի արտահայտության հաշվում
            if (pos >= expr.size() || expr[pos] != ')') {
                throw runtime_error("Mismatched parentheses");
            }
            pos++;  //բաց ենք թողնում ')' փակագիծը
            return value;
        }
        else
            return parse_number();
    }

    //ենթադրենք թիվը միանիշ չէ, անհրաժեշտ է ֆունկցիա մի քանի նիշ թիվը գտնելու համար
    int parse_number() {
        int num = 0;
        while (pos < expr.size() && isdigit(expr[pos]))
        {
            num = num * 10 + (expr[pos] - '0');
            pos++;
        }
        return num;
    }
public:
    int evaluate(string input) {
        expr = input;
        pos = 0;
        return parse_sub_add();
    }
};


int main()
{
    string input;
    cout << "your expression : "; 
    getline(cin, input);
    Calculator_AST calc;
    cout<<calc.evaluate(input);
}

