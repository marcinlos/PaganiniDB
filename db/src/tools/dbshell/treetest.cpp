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

typedef 
BTree<
    PageManager<FilePersistenceManager<DummyLocker>>,
    RowIndexer,
    DataPage<Index, types::FieldType, IndexReader, IndexWriter>,
    DataPage<Row, std::shared_ptr<const RowFormat>, RowReader, RowWriter>
>
Table;



class Test
{    
public:
    Test();
    ~Test();
    void databaseCreationTest();
    
    void userLoop();
    
private:
    std::shared_ptr<RowFormat> fmt;
    std::shared_ptr<Row> row;
    PageManager<FilePersistenceManager<DummyLocker>> manager;
    
    void setRow(int count, float variance, const string& name,
        const string& surname, const string& description);
};

Test::Test(): 
    fmt(new RowFormat { 
        {types::ContentType::Int, "count"}, 
        {types::ContentType::Float, "variance"},
        {{types::ContentType::Char, 14}, "Name"},
        {types::ContentType::VarChar, "Surname"},
        {types::ContentType::VarChar, "Description"}
    }),
    row(new Row(fmt, { 
        new types::Int(432), 
        new types::Float(1.23),
        new types::Char(14, "Aram"),
        new types::VarChar("Khachaturian"),
        new types::VarChar("Kompozytor radziecki")
    }))
{
    using namespace types;
    manager.createFile("db");
    manager.openFile("db");

    RowIndexer indexer(fmt, 3);
    Index idx( { ContentType::VarChar }, 
        Index::DataPtr(new VarChar("Dupa")), 99);

    BTree<
        PageManager<FilePersistenceManager<DummyLocker>>,
        RowIndexer,
        DataPage<Index, types::FieldType, IndexReader, IndexWriter>,
        DataPage<Row, std::shared_ptr<const RowFormat>, RowReader, RowWriter>
    > tree(manager, -1, indexer);
    
    for (int i = 0; i < 100050; ++ i)
    {
        (*row)["count"] = new Int(i);
        tree.insert(*row);
    }
        
    (*row)["Surname"] = new VarChar("Dupa");
    (*row)["count"] = new Int(3999);
    tree.insert(*row);
    (*row)["Surname"] = new VarChar("Khachaturian");
    
    for (int i = 0; i < 1050; ++ i)
        tree.insert(*row);
        
    std::cout << "Szukamy: " << std::endl; 
    std::cout << *tree.find(idx) << std::endl;

    PageBuffer page;
    manager.readPage(0, &page);
    inspect::InfoFormatter fmt;
    std::cout << fmt(*page.get<DatabaseHeader>()) << std::endl;
}


Test::~Test()
{
    manager.closeFile();
}


void Test::databaseCreationTest()
{

    //manager.closeFile();
}

void Test::setRow(int count, float variance, const string& name,
    const string& surname, const string& description)
{
    (*row)["count"] = new types::Int(count);
    (*row)["variance"] = new types::Float(variance);
    (*row)["Name"] = new types::Char(14, name);
    (*row)["Surname"] = new types::VarChar(surname);
    (*row)["Description"] = new types::VarChar(description);
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
