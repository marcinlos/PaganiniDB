#include "config.h"
#include <paganini/util/lexer/StringUtils.h>
#include <paganini/util/lexer/Lexer.h>
#include <stdexcept>
using namespace std;


namespace paganini
{

namespace util
{

namespace lexer
{


struct ParseSingleToken
{
    typedef string::const_iterator iterator;
    iterator pos;
    string& content;
    bool success;
    int backupPos;
    ParseSingleToken(iterator next, iterator /*end*/, string& content):
        pos(next), content(content), success(true)
    {
        backupPos = content.length();
    }
    void clean() { content.erase(backupPos); }
};


struct ParseDigitSeq: public ParseSingleToken
{
    bool empty;
    ParseDigitSeq(iterator next, iterator end, string& content):
      ParseSingleToken(next, end, content)
    {
        if (pos == end || ! isDigit(*pos))
            empty = true;
        else empty = false;
        while (pos != end && isDigit(*pos))
            content += *pos ++;
    }
};


struct ParseDecimalLiteral: public ParseSingleToken
{
    ParseDecimalLiteral(iterator next, iterator end, string& content):
        ParseSingleToken(next, end, content)
    {
        if (! isNonzeroDigit(*pos))
        {
            success = false;
            return;
        }
        content += *pos ++;
        while (pos != end && isDigit(*pos))
            content += *pos ++;
    }
};


struct ParseOctalLiteral: public ParseSingleToken
{
    ParseOctalLiteral(iterator next, iterator end, string& content):
        ParseSingleToken(next, end, content)
    {
        if (! isOctDigit(*pos))
        {
            success = false;
            return;
        }
        content += *pos ++;
        while (pos != end && isOctDigit(*pos))
            content += *pos ++;
    }
};


struct ParseHexLiteral: public ParseSingleToken
{
    ParseHexLiteral(iterator next, iterator end, string& content):
        ParseSingleToken(next, end, content)
    {
        if (*pos != '0' || ++ pos == end || (*pos != 'x' && *pos != 'X'))
        {
            success = false;
            return;
        }
        ++ pos;
        if (pos == end || ! isHexDigit(*pos))
        {
            success = false;
            return;
        }
        content += "0x";
        while (pos != end && isHexDigit(*pos))
            content += *pos ++;
    }
};


struct ParseIntegerSuffix: public ParseSingleToken
{
    ParseIntegerSuffix(iterator next, iterator end, string& content):
        ParseSingleToken(next, end, content)
    {
        bool u = false, l = false;
        if (! isOneOf(*pos, "uUlL"))
        {
            success = false;
            return;
        }
        if (*pos == 'u' || *pos == 'U')
        {
            content += 'u';
            u = true;
        }
        else if (*pos == 'l' || *pos == 'L')
        {
            content += 'L';
            l = true;
        }
        if (++ pos == end || ! isOneOf(*pos, "uUlL"))
            return;
        if (! u && (*pos == 'u' || *pos == 'U'))
        {
            content += 'u';
            ++ pos;
        }
        else if (! l && (*pos == 'l' || *pos == 'L'))
        {
            content += 'L';
            ++ pos;
        }
    }
};


struct ParseIntegerLiteral: public ParseSingleToken
{
    ParseIntegerLiteral(iterator next, iterator end, string& content):
      ParseSingleToken(next, end, content)
    {
        if (*pos != '0')
        {
            ParseDecimalLiteral dec(pos, end, content);
            if (dec.success)
                pos = dec.pos;
            else
                success = false;
        }
        else
        {
            ParseHexLiteral hex(pos, end, content);
            if (hex.success)
             pos = hex.pos;
            else
            {
                ParseOctalLiteral oct(pos, end, content);
                if (oct.success)
                    pos = oct.pos;
                else
                    success = false;
            }
        }
        ParseIntegerSuffix suffix(pos, end, content);
        if (suffix.success)
            pos = suffix.pos;
    }
};


struct ParseName: public ParseSingleToken
{
    ParseName(iterator next, iterator end, string& content):
        ParseSingleToken(next, end, content)
    {
        if (! canBeginId(*pos))
        {
            success = false;
            return;
        }
        content += *pos ++;
        while (pos != end && canBeInId(*pos))
            content += *pos ++;
    }
};


struct ParseSimpleEscSeq: public ParseSingleToken
{
    ParseSimpleEscSeq(iterator next, iterator end, string& content):
        ParseSingleToken(next, end, content)
    {
        if (*pos != '\\' || ++ pos == end)
        {
            success = false;
            return;
        }
        switch (*pos)
        {
        case '\'':    content += '\'';   break;
        case '"':     content += '"';    break;
        case '?':     content += '\?';   break;
        case '\\':    content += '\\';   break;
        case 'a':     content += '\a';   break;
        case 'b':     content += '\b';   break;
        case 'f':     content += '\f';   break;
        case 'n':     content += '\n';   break;
        case 'r':     content += '\r';   break;
        case 't':     content += '\t';   break;
        case 'v':     content += '\v';   break;
        default:
            success = false;
            return;
        }
        ++ pos;
        return;
    }
};


struct ParseOctEscSeq: public ParseSingleToken
{
    ParseOctEscSeq(iterator next, iterator end, string& content):
      ParseSingleToken(next, end, content)
    {
        if (*pos != '\\' || ++ pos == end || ! isOctDigit(*pos))
        {
            success = false;
            return;
        }
        int val = 0;
        for (int i = 0; i < 3; ++ i)
        {
            val <<= 3;
            val += octToVal(*pos);
            if (++ pos == end || ! isOctDigit(*pos))
                break;
        }
        content += static_cast<char>(val);
    }
};

struct ParseHexEscSeq: public ParseSingleToken
{
    ParseHexEscSeq(iterator next, iterator end, string& content):
        ParseSingleToken(next, end, content)
    {
        if (*pos != '\\' || ++ pos == end || (*pos != 'x' && *pos != 'X') 
            || ++ pos == end)
        {
            success = false;
            return;
        }
        int val = 0;
        for (int i = 0; i < 2; ++ i)
        {
            val <<= 4;
            val += hexToVal(*pos);
            if (++ pos == end || ! isHexDigit(*pos))
                break;
        }
        content += static_cast<char>(val);
    }
};


struct ParseEscSeq: public ParseSingleToken
{
    ParseEscSeq(iterator next, iterator end, string& content):
        ParseSingleToken(next, end, content)
    {
        if (*pos != '\\')
        {
            success = false;
            return;
        }
        ParseSimpleEscSeq simple(pos, end, content);
        if (simple.success)
        {
            pos = simple.pos;
            return;
        }
        ParseOctEscSeq oct(pos, end, content);
        if (oct.success)
        {
            pos = oct.pos;
            return;
        }
        ParseHexEscSeq hex(pos, end, content);
        if (hex.success)
        {
            pos = hex.pos;
            return;
        }
    }
};


struct ParseCharLiteral: public ParseSingleToken
{
    bool isAcceptable(char c) { return c != '\n' && c != '\''; }
    
    ParseCharLiteral(iterator next, iterator end, string& content):
        ParseSingleToken(next, end, content)
    {
        if (*pos != '\'' || ++ pos == end || ! isAcceptable(*pos))
        {
            success = false;
            return;
        }
        if (*pos != '\\')
            content += *pos ++;
        else
        {
            ParseEscSeq esc(pos, end, content);
            if (! esc.success)
            {
                success = false;
                return;
            }
            else pos = esc.pos;
        }
        if (pos == end || *pos ++ != '\'')
        {
            success = false;
            clean();
        }
    }
};


struct ParseStringLiteral: public ParseSingleToken
{
    bool isAcceptable(char c) { return c != '\n' && c != '\"'; }
    
    ParseStringLiteral(iterator next, iterator end, string& content):
        ParseSingleToken(next, end, content)
    {
        if (*pos != '"' || ++ pos == end || *pos == '\n')
        {
            success = false;
            return;
        }
        while (pos != end && isAcceptable(*pos))
        {
            if (*pos != '\\')
            content += *pos ++;
            else
            {
                ParseEscSeq esc(pos, end, content);
                if (! esc.success)
                {
                    success = false;
                    clean();
                    return;
                }
                else pos = esc.pos;
            }
        }
        if (pos == end || *pos ++ != '"')
        {
            success = false;
            clean();
        }
    }
};


struct ParseFracConstant: public ParseSingleToken
{
    ParseFracConstant(iterator next, iterator end, string& content):
        ParseSingleToken(next, end, content)
    {
        bool emptyBegin = true;
        if (isDigit(*pos))
        {
            ParseDigitSeq digits(pos, end, content);
            emptyBegin = digits.empty;
            pos = digits.pos;
        }
        if (pos == end || *pos ++ != '.')
        {
            success = false;
            clean();
            return;
        }
        content += '.';
        ParseDigitSeq digits(pos, end, content);
        if (emptyBegin && digits.empty)
        {
            success = false;
            clean();
            return;
        }
        pos = digits.pos;
    }
};

struct ParseExponentPart: public ParseSingleToken
{
    ParseExponentPart(iterator next, iterator end, string& content):
        ParseSingleToken(next, end, content)
    {
        if ((*pos != 'E' && *pos != 'e') || ++ pos == end || 
          (*pos != '-' && *pos != '+'))
        {
            success = false;
            return;
        }
        (content += 'e') += *pos ++;
        if (pos == end || ! isDigit(*pos))
        {
            success = false;
            clean();
            return;
        }
        ParseDigitSeq digits(pos, end, content);
        if (! digits.success || digits.empty)
        {
            success = false;
            clean();
        }
        else pos = digits.pos;
    }
};

struct ParseFloatingLiteral: public ParseSingleToken
{
    ParseFloatingLiteral(iterator next, iterator end, string& content):
        ParseSingleToken(next, end, content)
    {
        ParseFracConstant frac(pos, end, content);
        if (frac.success)
        {
            ParseExponentPart exponent(frac.pos, end, content);
            if (exponent.success)
                pos = exponent.pos;
            else pos = frac.pos;
        }
        else
        {
            ParseDigitSeq digits(pos, end, content);
            if (! digits.success || digits.empty)
                success = false;
            else
            {
                ParseExponentPart exponent(digits.pos, end, content);
                if (exponent.success)
                    pos = exponent.pos;
                else
                {
                    success = false;
                    digits.clean();
                }
            }
        }
        if (pos != end && isOneOf(*pos, "lLfF"))
        {
            if (*pos == 'f' || *pos == 'F')
            {
                content += 'f';
                ++ pos;
            }
            else if (*pos == 'l' || *pos == 'L')
            {
                content += 'L';
                ++ pos;
            }
        }
    }
};


struct SkipComment: public ParseSingleToken
{
    SkipComment(iterator next, iterator end, string& content):
        ParseSingleToken(next, end, content)
    {
        if (pos == end || *pos != '/' || ++ pos == end)
        {
            success = false;
            return;
        }
        if (*pos == '/')
            while (pos != end && *pos ++ != '\n');
        else if (*pos == '*')
        {
            while (pos != end)
            {
                if (*pos ++ == '*' && pos != end && *pos ++ == '/')
                   return;
            }
            throw std::runtime_error("Unfinished comment");
        }
        else success = false;
    }
};


bool Lexer::TokenizingFunction::operator () (iterator& next, 
   iterator end, Token& token)
{
    skipWhite(next, end);

    SkipComment sc(next, end, token.content);
    if (sc.success) next = sc.pos;
    if (next == end) return false;

    ParseName pname(next, end, token.content);
    if (pname.success)
    {
        next = pname.pos;
        token.type = T_NAME;
        return true;
    }
    ParseFloatingLiteral pfloat(next, end, token.content);
    if (pfloat.success)
    {
        next = pfloat.pos;
        token.type = T_FLOAT;
        return true;
    }
    ParseIntegerLiteral pint(next, end, token.content);
    if (pint.success)
    {
        next = pint.pos;
        token.type = T_INTEGER;
        return true;
    }
    ParseCharLiteral pchar(next, end, token.content);
    if (pchar.success)
    {
        next = pchar.pos;
        token.type = T_CHAR;
        return true;
    }
    ParseStringLiteral pstring(next, end, token.content);
    if (pstring.success)
    {
        next = pstring.pos;
        token.type = T_STRING;
        return true;
    }
    if (parseOperator(next, end, token))
        return true;
    if (parseSymbol(next, token)) 
        return true;

    ++ next;
    return false;
}

void Lexer::TokenizingFunction::skipWhite(iterator& next, iterator end)
{
    while (next != end && isWhite(*next))
        ++ next;
}

bool Lexer::TokenizingFunction::parseOperator(iterator& next, 
   iterator end, Token& token)
{
    iterator i = next;
    string tmp;
    while (i != end && canBeInOperator(*i))
        tmp += *i ++;
    while (! tmp.empty())
    {
        if (lexer.multicharOperators.find(tmp) != 
            lexer.multicharOperators.end())
            break;
        tmp.erase(tmp.length() - 1);
    }
    if (tmp.empty())
        return false;
    else 
    {
        token.content = tmp;
        token.type = T_OPERATOR;
        next += tmp.length();
        return true;
    }
}

bool Lexer::TokenizingFunction::parseSymbol(iterator& next, 
    Token& token)
{
    if (*next == '{')           token.type = T_RBRACE;
    else if (*next == '}')      token.type = T_LBRACE;
    else if (*next == '[')      token.type = T_LBRACKET;
    else if (*next == ']')      token.type = T_RBRACKET;
    else if (*next == '(')      token.type = T_LPAR;
    else if (*next == ')')      token.type = T_RPAR;
    else if (lexer.operators.find(*next) != string::npos)
        token.type = T_OPERATOR;
    if (token.type != T_NONE)
    {
        token.content = *next;
        ++ next;
        return true;
    }
    else return false;
}

bool Lexer::registerOperators(const std::string& ops)
{
    bool fullSuccess = true;
    for (string::const_iterator i = ops.begin(); i != ops.end(); ++ i)
    {
        if (! canBeInOperator(*i))
            fullSuccess = false;
        else operators.push_back(*i);
    }
    return fullSuccess;
}

bool Lexer::registerMulticharOperator(const string& op)
{
    for (string::const_iterator i = op.begin(); i != op.end(); ++ i)
    {
        if (! canBeInOperator(*i))
            return false;
    }
    multicharOperators.insert(op);
    return true;
}


} // lexer
} // util
} // paganini

