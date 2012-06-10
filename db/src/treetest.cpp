#include "config.h"
#include <paganini/paging/Page.h>
#include <paganini/paging/PageManager.h>
#include <paganini/paging/FilePersistenceManager.h>
#include <paganini/paging/DummyLocker.h>
#include <paganini/Error.h>
#include <paganini/row/datatypes.h>
#include <paganini/row/RowFormat.h>
#include <paganini/row/Row.h>
#include <paganini/row/RowWriter.h>
#include <paganini/row/RowReader.h>
#include <paganini/row/FieldFactory.h>
#include <paganini/paging/DataPage.h>
#include <paganini/row/Comparator.h>
#include <paganini/indexing/ComparatorFactory.h>
#include <paganini/indexing/Index.h>
#include <paganini/indexing/IndexReader.h>
#include <paganini/indexing/IndexWriter.h>
#include <paganini/indexing/RowIndexer.h>
#include <paganini/indexing/BTree.h>
#include <paganini/inspect/InfoFormatter.h>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <sstream>
using namespace paganini;

inline char hex_digit(int i)
{
    return i < 10 ? i + '0' : i - 10 + 'A';
}

inline unsigned char make_ascii(unsigned char c)
{
    return c == '\0' ? '.' : (c < ' ' ? ' ' : c);
}

string format_bytes(const_raw_data data, size16 len, size16 in_line = 16)
{
    std::ostringstream ss;
    // int lines = (len + in_line - 1) / in_line;
    for (unsigned int i = 0; i < len / in_line; ++ i)
    {
        typedef const unsigned char* bytes;
        ss << std::setw(4) << std::hex << i * in_line << " |";
        bytes d = reinterpret_cast<bytes>(data + i * in_line);
        for (int j = 0; j < in_line; ++ j)
        {
            ss << hex_digit((d[j] & 0xf0) >> 4);
            ss << hex_digit(d[j] & 0x0f);
            if (j != in_line - 1)
            {
                ss << ' '; 
                if ((j + 1) % 2 == 0)
                    ss << ' ';
            }
        }
        ss << " | ";
        for (int j = 0; j < in_line; ++ j)
        {
            ss << make_ascii(d[j]);
        }
        ss << std::endl;
    }
    return ss.str();
}

class Test
{    
public:
    Test();
    void databaseCreationTest();
    
    void userLoop();
    
private:
    RowFormat fmt;
    Row row;
    PageManager<FilePersistenceManager<DummyLocker>> manager;
    
    void setRow(int count, float variance, const string& name,
        const string& surname, const string& description);
};

Test::Test(): 
    fmt({ 
        {types::ContentType::Int, "count"}, 
        {types::ContentType::Float, "variance"},
        {{types::ContentType::Char, 14}, "Name"},
        {types::ContentType::VarChar, "Surname"},
        {types::ContentType::VarChar, "Description"}
    }),
    row(fmt, { 
        new types::Int(432), 
        new types::Float(1.23),
        new types::Char(14, "Aram"),
        new types::VarChar("Khachaturian"),
        new types::VarChar("Kompozytor radziecki")
    })
{
    manager.createFile("db");
    manager.openFile("db");

    BTree<
        PageManager<FilePersistenceManager<DummyLocker>>,
        RowIndexer,
        DataPage<Index, types::FieldType, IndexReader, IndexWriter>,
        DataPage<Row, RowFormat, RowReader, RowWriter>
    > tree(manager, -1, RowIndexer(fmt, 4));
    
    for (int i = 0; i < 1050; ++ i)
        tree.insert(row);
}


void Test::databaseCreationTest()
{

        


    //manager.closeFile();
}

void Test::setRow(int count, float variance, const string& name,
    const string& surname, const string& description)
{
    row["count"] = new types::Int(count);
    row["variance"] = new types::Float(variance);
    row["Name"] = new types::Char(14, name);
    row["Surname"] = new types::VarChar(surname);
    row["Description"] = new types::VarChar(description);
}


void Test::userLoop()
{
    string line;
    while (std::getline(std::cin, line))
    {
        std::stringstream ss;
        ss << line;
        int n;
        ss >> n;
        if (ss)
        {
            PageBuffer page;
            manager.readPage(n, &page);
            inspect::InfoFormatter fmt;
            std::cout << std::endl << fmt(page) << std::endl;
        }
    }
}


int main()
{
    try
    {     
        Test test;  
        
        test.userLoop();
    }
    catch (paganini::Exception& e)
    {
        printf("%s\n%s\n", e.what(), e.getCodeMessage());
    }
    catch (std::exception& e)
    {
        printf("%s\n", e.what());
    }
    return 0;
}
