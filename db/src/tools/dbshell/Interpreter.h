#ifndef __PAGANINI_TOOLS_DBSHELL_INTERPRETER_H__
#define __PAGANINI_TOOLS_DBSHELL_INTERPRETER_H__

#include <paganini/util/lexer/Tokenizer.h>
#include <paganini/util/lexer/Lexer.h>
#include <paganini/util/format.h>
#include <paganini/row/RowFormat.h>
#include <paganini/row/Row.h>
#include <paganini/row/TypeMapping.h>
#include <paganini/row/FieldFactory.h>
#include <paganini/paging/PageManager.h>
#include <paganini/paging/DataPage.h>
#include <paganini/row/RowReader.h>
#include <paganini/row/RowWriter.h>
#include <paganini/paging/FilePersistenceManager.h>
#include <paganini/paging/DummyLocker.h>
#include <paganini/indexing/RowIndexer.h>
#include <paganini/indexing/IndexReader.h>
#include <paganini/indexing/IndexWriter.h>
#include <paganini/indexing/BTree.h>

#include <paganini/inspect/InfoFormatter.h>
#include <paganini/inspect/format_bytes.h>

#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <memory>
#include <stdexcept>
using std::string;


namespace paganini
{

namespace shell
{

typedef PageManager<FilePersistenceManager<DummyLocker>> FilePageManager;

typedef DataPage<Index, types::FieldType, IndexReader, IndexWriter> 
    IndexDataPage;

typedef DataPage<Row, std::shared_ptr<const RowFormat>, RowReader, RowWriter> 
    RowDataPage;

typedef BTree<FilePageManager, RowIndexer, IndexDataPage, RowDataPage> 
    TableTree;


using namespace util::lexer;

template <typename Dest, typename Src>
inline Dest lexical_cast(const Src& src)
{
    std::stringstream ss;
    ss << src;
    Dest dest;
    ss >> dest;
    if (! ss)
        throw std::runtime_error("Bad lexical cast");
    return dest;
}


class Interpreter
{
private:
    util::lexer::Lexer lexer;
    typedef std::vector<util::lexer::Token>::const_iterator TokenIterator;
    
    typedef util::lexer::Tokenizer<
        util::lexer::Lexer::TokenizingFunction,
        util::lexer::Token
    >
    Tokenizer;
    
    FilePageManager pageManager_;
    std::unordered_map<string, std::unique_ptr<TableTree>> tables_;
    
public:
    Interpreter()
    {
        pageManager_.createFile("db");
        pageManager_.openFile("db");
        lexer.registerOperators(",.+-=*^&%");
        
        std::shared_ptr<RowFormat> format(new RowFormat { 
            {types::ContentType::Int, "Count"}, 
            {types::ContentType::Float, "Variance"},
            {{types::ContentType::Char, 14}, "Name"},
            {types::ContentType::VarChar, "Surname"},
            {types::ContentType::VarChar, "Description"}
        });
        
        tables_.insert(std::make_pair("Example", new TableTree(pageManager_, 
            TableTree::ALLOC_NEW, RowIndexer(format, 2))));
    }
    
    
    ~Interpreter()
    {
        pageManager_.closeFile();
    }
    
    
    void process(const string& line)
    {
        using namespace util::lexer;
        
        Tokenizer tokenizer(line, lexer.function());
        tokenizer.tokenize();
        parse(tokenizer.begin(), tokenizer.end());
        /*
        for (const auto& i: tables_)
        {
            std::cout << i.first << std::endl;
            std::cout << "Format: " << std::endl;
            std::cout << *(i.second->indexer().rowFormat()) << std::endl;
        }*/
    }
    
    void parse(TokenIterator begin, TokenIterator end)
    {
        if (begin->type == T_NAME)
        {
            if (begin->content == "show")
                parseShow(++ begin, end);
            else if (begin->content == "raw_page")
                showRawPage(++ begin, end);
            else if (begin->content == "header")
                showPageHeader(++ begin, end);
            else if (begin->content == "dbheader")
                showDatabaseHeader(++ begin, end);
            else if (begin->content == "insert")
                insertNewRow(++ begin, end);
        }
    }
    
    void parseShow(TokenIterator begin, TokenIterator end)
    {
        if (begin != end && begin->type == T_NAME)
        {
            if (begin->content == "tables")
                showTables();
        }
        else std::cout << "Niedobrze" << std::endl;
    }
    
    void showTables()
    {
        std::cout << std::endl;
        for (const auto& table: tables_)
        {
            std::cout << "Nazwa: " << table.first << std::endl;
            std::cout << "Kolumny: " << std::endl;
            std::cout << *(table.second->indexer().rowFormat()) << std::endl;
        }
    }
    
    void showRawPage(TokenIterator begin, TokenIterator end)
    {
        if (begin != end && begin->type == T_INTEGER)
        {
            page_number n = lexical_cast<page_number>(begin->content);
            PageBuffer page;
            pageManager_.readPage(n, &page);
            // inspect::InfoFormatter fmt;
            std::cout << std::endl << inspect::format_bytes(page.buffer, 
                PAGE_SIZE) << std::endl;
        }
    }    
    
    void showPageHeader(TokenIterator begin, TokenIterator end)
    {
        if (begin != end && begin->type == T_INTEGER)
        {
            page_number n = lexical_cast<page_number>(begin->content);
            PageBuffer page;
            pageManager_.readPage(n, &page);
            inspect::InfoFormatter fmt;
            std::cout << std::endl << fmt(page.header) << std::endl;
        }
    }
    
    void showDatabaseHeader(TokenIterator begin, TokenIterator end)
    {
        PageBuffer page;
        pageManager_.readPage(0, &page);
        inspect::InfoFormatter fmt;
        std::cout << std::endl << fmt(*page.get<DatabaseHeader>()) 
            << std::endl;
    }
    
    void insertNewRow(TokenIterator begin, TokenIterator end)
    {
        if (begin == end || (begin ++)->content != "into")
            throw std::runtime_error("'into' expected after 'insert'");
        if (begin != end && begin->type == T_NAME)
        {
            auto iter = tables_.find(begin->content);
            if (iter == tables_.end())
            {
                throw std::runtime_error(util::format("Table '{}' does not "
                    "exist", begin->content));
            }
            TableTree& table = *iter->second;
            std::shared_ptr<const RowFormat> format = 
                table.indexer().rowFormat();
            
            if (++ begin == end || begin->type != T_LPAR)
                throw std::runtime_error(util::format("'(' expected, got '{}'", begin->type));
            if (++ begin == end)
                throw std::runtime_error("unexpected end of input");
            std::unique_ptr<Row> row(new Row(format));
            
            int i = 0;
            while (begin->type != T_RPAR)
            {
                if (i != 0 && (begin ++)->content != ",")
                    throw std::runtime_error("comma expected");
                if (begin == end)
                    throw std::runtime_error("unexpected end of input");
                if (i < format->columnCount())
                {
                    (*row)[i] = readValue(begin->content, 
                        (*format)[i].type).release();
                }
                else
                    throw std::runtime_error("Too many values");
                ++ begin;
                if (begin == end)
                    throw std::runtime_error("unexpected end of input");
                ++ i;
            }
            std::cout << "Row: " << std::endl << *row << std::endl;
            table.insert(*row);
        }
    }
    
    std::unique_ptr<types::Data> readValue(const string& value, 
        types::FieldType type)
    {
        FieldFactory& factory = FieldFactory::getInstance();
        std::unique_ptr<types::Data> data = factory.create(type);
        if (! data->readFrom(value))
        {
            throw std::runtime_error(util::format("Cannot convert '{}' "
                "to {}", value, type));
        }
        return data;
    }
};


} // shell
} // paganini


#endif // __PAGANINI_TOOLS_DBSHELL_INTERPRETER_H__
