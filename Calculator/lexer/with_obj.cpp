#include <iostream>
#include <string>
#include <cctype>
#include <stdexcept>
using namespace std;

enum TokenType
{
    NUMBER,
    PLUS,
    MINUS,
    MUL,
    DIV,
    LPAREN,
    RPAREN,
    END
};

struct Token {
    TokenType type;
    int value;

    Token(TokenType t, int v = 0)
    {
        type = t;
        value = v;
    };
    Token() {}; //default
};

 

class Lexer {
private:
    string text;
    int pos;
    char current;

    void advance() {
        pos++;
        if (pos < text.size())
            current = text[pos];
        else
            current = NULL;
    }
    void skip_whitespace() {
        while (pos < text.size() && text[pos] == ' ')
        {
            advance();
            continue;
        }
    }
    Token number() {
        string result ="";
        while (isdigit(current))
        {
            result += current;
            advance();
        }
        return Token(NUMBER,(stoi(result)));
    }


public:
    Lexer(string input_text) {
        text = input_text;
        pos = 0;
        current= text[0];
    }

    Token get_next_token() {
        while (current != NULL) {
            if (current == ' ')
                skip_whitespace();
            if (isdigit(current))
                return number();
            if (current == '+')
            {
                advance();
                return Token(PLUS);
            }
            if (current == '-')
            {
                advance();
                return Token(MINUS);
            }
            if (current == '*')
            {
                advance();
                return Token(MUL);
            }
            if (current == '/')
            {
                advance();
                return Token(DIV);
            }
            if (current == '(')
            {
                advance();
                return Token(LPAREN);
            }
            if (current == ')')
            {
                advance();
                return Token(RPAREN);
            }
            else cout << "Unknown character";

        }
        return Token(END);
    }
};

class Parser {
private:
    Lexer& lexer;
    Token current_token;

    void eat(TokenType type) {
        if (current_token.type == type)
            current_token = lexer.get_next_token();
        else
            throw runtime_error("Unexpected token");
    }
    //---PLUS/MINUS---
    int expr() {
        int result = term();
        while (current_token.type == PLUS || current_token.type == MINUS) {
            if (current_token.type == PLUS) {
                eat(PLUS);
                result = result + term();
            }
            else {
                eat(MINUS);
                result = result - term();
            }
        }
    return result;
    }
    //---MUL/DIV---
    int term() {
        int result = factor();
        while (current_token.type == MUL || current_token.type == DIV) {
            if (current_token.type == MUL) {
                eat(MUL);
                result = result * factor();
            }
            else {
                eat(DIV);
                result = result / factor();
            }
        }
    return result;
    }
    //---VAL/EXPRESSION---
    int factor() {
        if (current_token.type == NUMBER) {
            int value = current_token.value;
            eat(NUMBER);
            return value;
        }
        if (current_token.type == LPAREN) {
            eat(LPAREN);
            int value = expr();
            eat(RPAREN);
            return value;
        }
        else
            throw runtime_error("Unexpected token in factor");

    }

public:
    Parser(Lexer& input_lexer) : lexer(input_lexer) {
        current_token = lexer.get_next_token();
    }
    int evaluate() {
        return expr();  
    }
};

class Calc_lexer {
private:
    Lexer lexer;
    Parser parser;
public:
    Calc_lexer(string input) 
        : lexer(input), parser(lexer) {}   

    int calc() {
        return parser.evaluate();   // parse and calculate
    }
};

int main() {
    string input;
    cout << "Enter expression: ";
    getline(cin, input);

    Calc_lexer Calculator(input);
    cout << Calculator.calc() << endl;
}

