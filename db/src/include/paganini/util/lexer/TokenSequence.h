/*
*/
#ifndef __PAGANINI_UTIL_LEXER_TOKEN_SEQUENCE_H__
#define __PAGANINI_UTIL_LEXER_TOKEN_SEQUENCE_H__

#include <paganini/util/lexer/Lexer.h>
#include <paganini/util/format.h>
#include <stdexcept>


namespace paganini
{
namespace util
{
namespace lexer
{

    template <typename TokenIterator>
    struct TokenSequence
    {
        TokenIterator begin, end;
        
        TokenSequence(TokenIterator begin, TokenIterator end): 
            begin(begin), end(end)
        {
        }
        
        bool isEnd() const { return begin == end; }
        
        bool next() { return (++ begin == end); }
        
        void nextNotEnd()
        {
            ++ begin;   
            notEnd();
        }
        
        void notEnd(const string& error = "unexpected end of input") const
        {
            if (begin == end)
                throw std::runtime_error(util::format("after '{}'\n{}", 
                    (begin - 1)->content, error));
        }
        
        void mustBe(const string& content) const 
        {
            notEnd();
            if (begin->content != content)
                throw std::runtime_error(util::format("'{}' got, {} expected",
                    begin->content, content));
        }
        
        void nextMustBe(const string& content)
        {
            nextNotEnd();
            mustBe(content);
        }
        
        bool is(const string& content) const
        {
            return begin->content == content;
        }
        
        void mustBe(TokenType type) const
        {
            notEnd();
            if (begin->type != type)
                throw std::runtime_error(util::format("'{}' got, {} expected",
                    begin->content, type));
        }

        bool is(TokenType type) const
        {
            return begin->type == type;
        }
        
        void nextMustBe(TokenType type)
        {
            nextNotEnd();
            mustBe(type);
        }
        
        const string& operator * () const
        {
            return begin->content;
        }
        
        TokenSequence& operator ++ ()
        {
            next();
            return *this;
        }
        
        TokenType type() const
        {
            return begin->type;
        }
    };


} // lexer
} // util
} // paganini



#endif // __PAGANINI_UTIL_LEXER_TOKEN_SEQUENCE_H__
