#include "Lexer.h"

//-------------------------------Lexer---------------------
//--String-to-described-vector--

void Lexer::advance()
{
    pos++;
    if (pos < text.size())
        current = text[pos];
    else
        current = '\0';
}

void Lexer::skip_whitespace()
{
    while (isspace(current))
        advance();
}

Token Lexer::number()
{
    std::string result = "";

    while (current != '\0' && isdigit(current))
    {
        result += current;
        advance();
    }

    return Token{ TokenType::NUMBER, (stoi(result)), "" };
}

Lexer::Lexer(std::string input_text)
{
    text = input_text;
    pos = 0;

    if (!text.empty())
        current = text[0];
    else
        current = '\0';
}

std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> tokens;

    while (current != '\0')
    {
        if (isspace(current))
        {
            skip_whitespace();
            continue;
        }

        if (isdigit(current))
        {
            tokens.push_back(number());
            continue;
        }

        if (isalpha(current))
        {
            std::string result;

            while (current != '\0' &&
                (isalpha(current) || isdigit(current) || current == '_'))
            {
                result += current;
                advance();
            }

            //  KEYWORDS
            if (result == "int")
                tokens.push_back(Token{ TokenType::INT, 0, "" });
            else if (result == "if")
                tokens.push_back(Token{ TokenType::IF, 0, "" });
            else if (result == "else")  
                tokens.push_back(Token{ TokenType::ELSE, 0, "" });
            else if (result == "while")
                tokens.push_back(Token{ TokenType::WHILE, 0, "" });
            else if (result == "return")
                tokens.push_back(Token{ TokenType::RET, 0, "" });
            else if (result == "print")
                tokens.push_back(Token{ TokenType::PRINT, 0, "" });
            else
                tokens.push_back(Token{ TokenType::IDENT, 0, result });

            continue;
        }

        if (current == '*')
        {
            tokens.push_back(Token{ TokenType::MUL, 0, "" });
            advance();
            continue;
        }

        if (current == '/')
        {
            tokens.push_back(Token{ TokenType::DIV, 0, "" });
            advance();
            continue;
        }

        if (current == '(')
        {
            tokens.push_back(Token{ TokenType::LPAREN, 0, "" });
            advance();
            continue;
        }

        if (current == ')')
        {
            tokens.push_back(Token{ TokenType::RPAREN, 0, "" });
            advance();
            continue;
        }

        // ==
        if (current == '=' && pos + 1 < text.size() && text[pos + 1] == '=')
        {
            advance();
            advance();
            tokens.push_back(Token{ TokenType::EQ, 0, "" });
            continue;
        }

        if (current == '=')
        {
            tokens.push_back(Token{ TokenType::ASSIGN, 0, "" });
            advance();
            continue;
        }

        // !=
        if (current == '!' && pos + 1 < text.size() && text[pos + 1] == '=')
        {
            advance();
            advance();
            tokens.push_back(Token{ TokenType::NEQ, 0, "" });
            continue;
        }

        // <=
        if (current == '<' && pos + 1 < text.size() && text[pos + 1] == '=')
        {
            advance();
            advance();
            tokens.push_back(Token{ TokenType::LE, 0, "" });
            continue;
        }

        // >=
        if (current == '>' && pos + 1 < text.size() && text[pos + 1] == '=')
        {
            advance();
            advance();
            tokens.push_back(Token{ TokenType::GE, 0, "" });
            continue;
        }
        // INC
        if (current == '+' && pos + 1 < text.size() && text[pos + 1] == '+')
        {
            advance();
            advance();
            tokens.push_back(Token{ TokenType::INC, 0, "" });
            continue;
        }
        // PLUS_ASSIGN
        if (current == '+' && pos + 1 < text.size() && text[pos + 1] == '=')
        {
            advance();
            advance();
            tokens.push_back(Token{ TokenType::PLUS_ASSIGN, 0, "" });
            continue;
        }
        //+
        if (current == '+')
        {
            tokens.push_back(Token{ TokenType::PLUS, 0, "" });
            advance();
            continue;
        }

        // DEC
        if (current == '-' && pos + 1 < text.size() && text[pos + 1] == '-')
        {
            advance();
            advance();
            tokens.push_back(Token{ TokenType::DEC, 0, "" });
            continue;
        }
        // MINUS_ASSIGN
        if (current == '-' && pos + 1 < text.size() && text[pos + 1] == '=')
        {
            advance();
            advance();
            tokens.push_back(Token{ TokenType::MINUS_ASSIGN, 0, "" });
            continue;
        }
        //-
        if (current == '-')
        {
            tokens.push_back(Token{ TokenType::MINUS, 0, "" });
            advance();
            continue;
        }



        if (current == '<')
        {
            tokens.push_back(Token{ TokenType::LT, 0, "" });
            advance();
            continue;
        }


        if (current == '>')
        {
            tokens.push_back(Token{ TokenType::GT, 0, "" });
            advance();
            continue;
        }

        if (current == '{')
        {
            tokens.push_back(Token{ TokenType::LBRACE, 0, "" });
            advance();
            continue;
        }

        if (current == '}')
        {
            tokens.push_back(Token{ TokenType::RBRACE, 0, "" });
            advance();
            continue;
        }

        if (current == ';')
        {
            tokens.push_back(Token{ TokenType::SEMI, 0, "" });
            advance();
            continue;
        }

        if (current == ',')
        {
            tokens.push_back(Token{ TokenType::COMMA, 0, "" });
            advance();
            continue;
        }
        if (current == '!')
        {
            tokens.push_back(Token{ TokenType::NOT, 0, "" });
            advance();
            continue;
        }
        if (current == '&' && pos + 1 < text.size() && text[pos + 1] == '&')
        {
            advance();
            advance();
            tokens.push_back(Token{ TokenType::AND,0,"&&" });
            continue;


        }
        if (current == '|' && pos + 1 < text.size() && text[pos + 1] == '|')
        {
            advance();
            advance();
            tokens.push_back(Token{ TokenType::OR,0,"||" });
            continue;


        }
        // NOT operator !
        if (current == '!')
        {
            // Check for != (not equal)
            if (pos + 1 < text.size() && text[pos + 1] == '=')
            {
                advance();
                advance();
                tokens.push_back(Token{ TokenType::NEQ, 0, "!=" });
                continue;
            }
            // Single ! (logical NOT)
            tokens.push_back(Token{ TokenType::NOT, 0, "!" });
            advance();
            continue;
        }

        throw std::runtime_error(
            std::string("Invalid character: '") + current + "'"
        );    
    }

    tokens.push_back(Token{ TokenType::END, 0, "" });
    return tokens;
}
