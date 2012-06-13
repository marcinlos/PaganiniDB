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
    
    typedef std::shared_ptr<const RowFormat> FormatInfo;
    
    FilePageManager pageManager_;
    std::unordered_map<string, std::shared_ptr<TableTree>> tables_;
    
public:
    Interpreter()
    {
        using types::ContentType;
        
        pageManager_.createFile("db");
        pageManager_.openFile("db");
        lexer.registerOperators(",.+-=*^&%");
        
        std::shared_ptr<RowFormat> format(new RowFormat { 
            {types::ContentType::Int, "Count"}, 
            {types::ContentType::Float, "Variance"},
            {{types::ContentType::Char, 14}, "Name"},
            {types::ContentType::VarChar, "Surname"},
            {types::ContentType::VarChar, "Description"}
            /*{ContentType::VarChar, "name"},
            {ContentType::PageNumber, "*/
        });
        
        tables_.insert(std::make_pair(/*"Tables"*/"Example", new TableTree(pageManager_, 
            2/*TableTree::ALLOC_NEW*/, RowIndexer(format, 2))));
    }
    
    
    void createTable(const string& name, FormatInfo format)
    {
        tables_.insert(std::make_pair(name, new TableTree(pageManager_,
            TableTree::ALLOC_NEW, RowIndexer(format, 0))));
    }
    
    
    ~Interpreter()
    {
        pageManager_.closeFile();
    }
    
    
    struct TokenSequence
    {
        TokenIterator begin, end;
        
        TokenSequence(TokenIterator begin, TokenIterator end): 
            begin(begin), end(end)
        {
        }
        
        bool eof() const { return begin == end; }
        
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
                throw std::runtime_error(util::format("'{} got, {} expected",
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
    
    
    void process(const string& line)
    {        
        Tokenizer tokenizer(line, lexer.function());
        tokenizer.tokenize();
        TokenSequence s(tokenizer.begin(), tokenizer.end());
        parse(s);
    }
    
    void parse(TokenSequence& s)
    {
        if (! s.eof())
        {
            if (*s == "show")
                parseShow(++ s);
            else if (*s == "raw_page")
                showRawPage(++ s);
            else if (*s == "header")
                showPageHeader(++ s);
            else if (*s == "dbheader")
                showDatabaseHeader();
            else if (*s == "insert")
                insertNewRow(++ s);
            else if (*s == "select")
                selectRows(++ s);
            else if (*s == "create")
                createTable(++ s);
            else 
                throw std::runtime_error(util::format(
                    "unknown command: '{}'", *s));
        }
    }
    
    void parseShow(TokenSequence& s)
    {
        s.mustBe(T_NAME);
        if (*s == "tables")
            showTables();
        else 
            std::runtime_error(util::format("unknown object '{}' to show", *s));
    }
    
    void showTables()
    {
        std::cout << std::endl;
        for (const auto& table: tables_)
        {
            std::cout << "Name: " << table.first << std::endl;
            std::cout << "Columns: " << std::endl;
            std::cout << *(table.second->indexer().rowFormat()) << std::endl;
        }
    }
    
    void showRawPage(TokenSequence& s)
    {
        s.mustBe(T_INTEGER);
        page_number n = lexical_cast<page_number>(*s);
        PageBuffer page;
        pageManager_.readPage(n, &page);
        std::cout << std::endl << inspect::format_bytes(page.buffer, 
            PAGE_SIZE) << std::endl;
    }    
    
    void showPageHeader(TokenSequence& s)
    {
        s.mustBe(T_INTEGER);
        page_number n = lexical_cast<page_number>(*s);
        PageBuffer page;
        pageManager_.readPage(n, &page);
        inspect::InfoFormatter fmt;
        std::cout << std::endl << fmt(page.header) << std::endl;
    }
    
    void showDatabaseHeader()
    {
        PageBuffer page;
        pageManager_.readPage(0, &page);
        inspect::InfoFormatter fmt;
        std::cout << std::endl << fmt(*page.get<DatabaseHeader>()) << std::endl;
    }
    
    void insertNewRow(TokenSequence& s)
    {
        s.mustBe("into");
        s.nextMustBe(T_NAME);

        auto iter = tables_.find(*s);
        if (iter == tables_.end())
        {
            throw std::runtime_error(util::format("Table '{}' does not "
                "exist", *s));
        }
        TableTree& table = *iter->second;
        std::shared_ptr<const RowFormat> format = table.indexer().rowFormat();
        
        s.nextMustBe(T_LPAR);
        s.nextNotEnd();
        Row row(format);
        
        int i = 0;
        while (! s.is(T_RPAR))
        {
            if (i != 0) 
            {
                s.mustBe(",");
                s.next();
            }
            s.notEnd();
            if (i < format->columnCount())
            {
                row[i] = readValue(*s, (*format)[i].type).release();
            }
            else 
                throw std::runtime_error("Too many values");
            s.nextNotEnd();
            ++ i;
        }
        std::cout << "Row: " << std::endl << row << std::endl;
        table.insert(row);
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
    
    void selectRows(TokenSequence& s)
    {
        s.mustBe("from");
        s.nextMustBe(T_NAME);
        auto iter = tables_.find(*s);
        if (iter == tables_.end())
        {
            throw std::runtime_error(util::format("Table '{}' does not "
                "exist", *s));
        }
        TableTree& table = *iter->second;
        std::shared_ptr<const RowFormat> format = table.indexer().rowFormat();
        int i = 0;
        for (const auto& row: table.all())
        {
            std::cout << "Row " << ++ i << std::endl;
            std::cout << row << std::endl;
        }
    }
    
    void createTable(TokenSequence& s)
    {
        s.mustBe("table");
        s.nextMustBe(T_NAME);
        string table = *s;
        
        if (tables_.find(*s) != tables_.end())
        {
            throw std::runtime_error(util::format("Table '{}' already "
                "exists", *s));
        }
        s.nextMustBe(T_LPAR);
        s.nextNotEnd();
        std::vector<Column> columns;
        std::shared_ptr<RowFormat> format(new RowFormat);
        int i = 0;
        string name;
        while (! s.is(T_RPAR))
        {
            if (i != 0)
            {
                s.mustBe(",");
                s.nextNotEnd();
            }
            s.mustBe(T_NAME);
            name = *s;
            s.nextNotEnd();
            types::FieldType type = parseFieldType(s);
            if (type == types::ContentType::None)
                throw std::runtime_error("Invalid column type");
            format->addColumn(Column(type, name));
            s.nextNotEnd();
            ++ i;
        }
        createTable(table, format);
    }
    
    types::FieldType parseFieldType(TokenSequence& s)
    {
        using types::ContentType;
        s.mustBe(T_NAME);
        if (*s == "int")
            return { ContentType::Int };
        else if (*s == "float")
            return { ContentType::Float };
        else if (*s == "varchar")
            return { ContentType::VarChar };
        else if (*s == "char")
        {
            s.nextMustBe(T_LPAR);
            s.nextMustBe(T_INTEGER);
            size16 size = lexical_cast<size16>(*s);            
            s.nextMustBe(T_RPAR);
            return { ContentType::Char, size };
        }
        return { ContentType::None };
    }
};


} // shell
} // paganini


#endif // __PAGANINI_TOOLS_DBSHELL_INTERPRETER_H__
