/*
*/
#ifndef __PAGANINI_DATABASE_DATABASE_ENGINE_H__
#define __PAGANINI_DATABASE_DATABASE__ENGINE_H__

#include <paganini/paging/PageManager.h>
#include <paganini/paging/FilePersistenceManager.h>
#include <paganini/paging/DummyLocker.h>
#include <paganini/indexing/RowIndexer.h>
#include <paganini/indexing/IndexReader.h>
#include <paganini/indexing/IndexWriter.h>
#include <paganini/database/Table.h>
#include <string>
#include <unordered_map>
using std::string;

 
namespace paganini
{
namespace engine
{


class DatabaseEngine
{
private:
    typedef PageManager<FilePersistenceManager<DummyLocker>> Pager;
    
    typedef DataPage<Index, types::FieldType, IndexReader, IndexWriter> 
        IndexDataPage;

    typedef DataPage<Row, std::shared_ptr<const RowFormat>, RowReader, RowWriter> 
        RowDataPage;

    typedef BTree<Pager, RowIndexer, IndexDataPage, RowDataPage> 
        TableTree;
    typedef std::shared_ptr<Table<TableTree>> TablePtr; 
    
public:
    typedef std::shared_ptr<const RowFormat> FormatPtr;
    
    DatabaseEngine(const string& path);
    
    ~DatabaseEngine();
    
    void createTable(const string& name, FormatPtr format);

private:
    
    Pager page_manager_;
    std::unordered_map<string, TablePtr> tables_;
};


void DatabaseEngine::createTable(const string& name, FormatPtr format)
{
    //tables_.insert(std::make_pair(name, new TableTree(page_manager_,
    //    TableTree::ALLOC_NEW, RowIndexer(format, 0))));
}


DatabaseEngine::DatabaseEngine(const string& path)
{
    page_manager_.createFile(path);
    page_manager_.openFile(path);
}


DatabaseEngine::~DatabaseEngine()
{   
    page_manager_.closeFile();
}



} // engine
} // paganini

#endif // __PAGANINI_DATABASE_DATABASE_ENGINE_H__
