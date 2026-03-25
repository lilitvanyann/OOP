#include <iostream>
#include <vector>
#include <stack>
#include <cctype>
using namespace std;

class Calculator{
private:

int precedence(char op)
{
    switch(op)
    {
        case '*':
        case '/':
            return 2;
        case '+':
        case '-':
            return 1;
        default:
            return 0;
    }
}

bool isOperator(char c)
{
    return (c == '+' || c == '-' || c == '*' || c == '/');
}

// INFIX -> POSTFIX
vector<string> infixToPostfix(string expr){
    stack<char> st;
    vector<string> output;

    for (int i = 0; i < expr.size(); i++)
    {
        char c = expr[i];

        // 🔹 multi-digit number
        if (isdigit(c))
        {
            string number = "";
            while (i < expr.size() && isdigit(expr[i]))
            {
                number += expr[i];
                i++;
            }
            i--; // because for loop will increment
            output.push_back(number);
        }
        else if (c == '(')
        {
            st.push(c);
        }
        else if (c == ')')
        {
            while (!st.empty() && st.top() != '(')
            {
                output.push_back(string(1, st.top()));
                st.pop();
            }
            st.pop(); // remove '('
        }
        else if (isOperator(c))
        {
            while (!st.empty() &&
                   precedence(st.top()) >= precedence(c))
            {
                output.push_back(string(1, st.top()));
                st.pop();
            }
            st.push(c);
        }
    }

    while (!st.empty())
    {
        output.push_back(string(1, st.top()));
        st.pop();
    }

    return output;
}


// POSTFIX evaluation
int EvaluatePostfix(vector<string> postfix){
    stack<int> res;

    for(string token : postfix)
    {
        if(isdigit(token[0]))
        {
            res.push(stoi(token));
        }
        else
        {
            int b = res.top(); res.pop();
            int a = res.top(); res.pop();

            int result;

            if (token == "+") result = a + b;
            else if (token == "-") result = a - b;
            else if (token == "*") result = a * b;
            else if (token == "/") result = a / b;

            res.push(result);
        }
    }

    return res.top();
}

public:
    int Evaluate(string expr)
    {
        vector<string> postfix = infixToPostfix(expr);
        return EvaluatePostfix(postfix);
    }
};


int main(){
    Calculator calculator;

    string expression;
    cout<<"expression :";cin >> expression;

    cout << "Result: " << calculator.Evaluate(expression);
}
