#ifndef __PAGANINI_TOOLS_DBSHELL_INTERPRETER_H__
#define __PAGANINI_TOOLS_DBSHELL_INTERPRETER_H__

#include <paganini/util/lexer/Tokenizer.h>
#include <paganini/util/lexer/Lexer.h>
#include <paganini/util/lexer/TokenSequence.h>
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
#include <paganini/concurrency/FilePageLocker.h>
#include <paganini/concurrency/ThreadPageLocker.h>
#include <paganini/indexing/RowIndexer.h>
#include <paganini/indexing/IndexReader.h>
#include <paganini/indexing/IndexWriter.h>
#include <paganini/indexing/BTree.h>
#include <paganini/database/DatabaseEngine.h>

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
    
    engine::DatabaseEngine& engine_;
    
public:
    Interpreter(engine::DatabaseEngine& engine): engine_(engine)
    {
        lexer.registerOperators(",.+-=*^&%");
    }
    
    
    void createTable(const string& name, FormatInfo format)
    {
        engine_.createTable(name, format);
    }
    
    
    ~Interpreter()
    {
    }
    
    typedef TokenSequence<TokenIterator> Sequence;
    
    void process(const string& line)
    {        
        Tokenizer tokenizer(line, lexer.function());
        tokenizer.tokenize();
        Sequence s(tokenizer.begin(), tokenizer.end());
        parse(s);
    }
    
    void parse(Sequence& s)
    {
        if (! s.isEnd())
        {
            if (*s == "print")
                printText(++ s);
            else if (*s == "show")
                parseShow(++ s);
            else if (*s == "raw_page")
                showRawPage(++ s);
            else if (*s == "uv")
                showUvPage(++ s);
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
    
    void printText(Sequence& s)
    {
        s.mustBe(T_STRING);
        std::cout << *s << std::endl;
    }
    
    void parseShow(Sequence& s)
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
        std::vector<std::pair<string, engine::DatabaseEngine::TablePtr>> tables;
        engine_.getTables(std::back_inserter(tables));
        for (auto pair: tables)
        {
            std::cout << "Name: " << pair.first << std::endl;
            std::cout << "Columns: " << std::endl;
            std::cout << *(pair.second->indexer().rowFormat()) << std::endl;
        }
    }
    
    void showRawPage(Sequence& s)
    {
        s.mustBe(T_INTEGER);
        page_number n = lexical_cast<page_number>(*s);
        PageBuffer page;
        engine_.pageManager().readPage(n, &page);
        std::cout << std::endl << inspect::format_bytes(page.buffer, 
            PAGE_SIZE) << std::endl;
    }   
    
    void showUvPage(Sequence& s)
    {
        s.mustBe(T_INTEGER);
        page_number n = lexical_cast<page_number>(*s);
        if (! isUV(n))
        {
            throw std::logic_error(util::format("Page number '{}' is not UV",
                n));
        }
        PageBuffer page;
        engine_.pageManager().readPage(n, &page);
        inspect::InfoFormatter fmt;
        std::cout << std::endl << fmt.uv(page) << std::endl;
    }  
    
    void showPageHeader(Sequence& s)
    {
        s.mustBe(T_INTEGER);
        page_number n = lexical_cast<page_number>(*s);
        PageBuffer page;
        engine_.pageManager().readPage(n, &page);
        inspect::InfoFormatter fmt;
        std::cout << std::endl << fmt(page.header) << std::endl;
    }
    
    void showDatabaseHeader()
    {
        PageBuffer page;
        engine_.pageManager().readPage(0, &page);
        inspect::InfoFormatter fmt;
        std::cout << std::endl << fmt(*page.get<DatabaseHeader>()) << std::endl;
    }
    
    void insertNewRow(Sequence& s)
    {
        s.mustBe("into");
        s.nextMustBe(T_NAME);
        string table_name = *s;
        FormatInfo format = engine_.tableFormat(table_name);
        s.nextMustBe("values");
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
        engine_.insert(table_name, row);
        std::cout << "Row: " << std::endl << row << std::endl;
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
    
    
    template <typename Iter>
    void printRows(Iter begin, Iter end)
    {
        for (int i = 0; begin != end; ++ begin, ++ i)
        {
            std::cout << "Row " << i << std::endl;
            std::cout << *begin << std::endl;   
        }
    }
    
    
    void selectRows(Sequence& s)
    {
        s.notEnd();
        std::vector<string> cols = parseColumns(s);
        s.mustBe("from");
        s.nextMustBe(T_NAME);
        string table_name = *s;
        FormatInfo format = engine_.tableFormat(table_name);
        s.next();
        std::vector<Row> rows;
        if (! s.isEnd())
        {
            types::FieldType type = (*format)[0].type;
            std::shared_ptr<types::Data> data = readValue(*s, type);
            engine_.select(table_name, Index(type, data), 
                std::back_inserter(rows));
        }
        else 
        {
            engine_.select(table_name, [](const Row& row) { return true; },
                std::back_inserter(rows)); 
        }
        printRows(rows.begin(), rows.end());
    }
    
    std::vector<string> parseColumns(Sequence& s)
    {
        std::vector<string> cols;
        s.notEnd();
        while (*s != "from")
        {
            s.mustBe(T_NAME);
            cols.push_back(*s);
            s.nextMustBe(",");
            s.nextNotEnd();
        }
        return cols;
    }
    
    void createTable(Sequence& s)
    {
        s.mustBe("table");
        s.nextMustBe(T_NAME);
        string table = *s;
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
        engine_.createTable(table, format);
    }
    
    types::FieldType parseFieldType(Sequence& s)
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
