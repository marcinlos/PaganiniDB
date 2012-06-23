/*
*/
#ifndef __PAGANINI_DATABASE_DATABASE_ENGINE_H__
#define __PAGANINI_DATABASE_DATABASE__ENGINE_H__

#include <paganini/paging/types.h>
#include <paganini/paging/PageManager.h>
#include <paganini/paging/FilePersistenceManager.h>
#include <paganini/paging/DummyLocker.h>
#include <paganini/concurrency/ThreadPageLocker.h>
#include <paganini/row/RowReader.h>
#include <paganini/row/RowWriter.h>
#include <paganini/row/RowFormat.h>
#include <paganini/indexing/RowIndexer.h>
#include <paganini/indexing/IndexReader.h>
#include <paganini/indexing/IndexWriter.h>
#include <paganini/row/FieldFactory.h>
#include <paganini/paging/DataPage.h>
#include <paganini/indexing/BTree.h>
#include <paganini/database/Table.h>
#include <paganini/util/format.h>
#include <paganini/row/TypeMapping.h>
// TODO: Brzydkie, popraw
#include <paganini/concurrency/pthread/Mutex.h>
#include <paganini/concurrency/pthread/Thread.h>
#include <paganini/concurrency/ScopeLock.h>
#include <paganini/database/DatabaseError.h>
#include <string>
#include <unordered_map>
using std::string;

 
namespace paganini
{
namespace engine
{


class DatabaseEngine
{
public:
    typedef std::shared_ptr<const RowFormat> FormatPtr;
    
    DatabaseEngine(const string& path, bool create = false);
    
    //~DatabaseEngine();
    
    void createTable(const string& name, FormatPtr format);

private:
    // Konfiguracja typow
    typedef concurrency::ThreadPageLocker<concurrency::pthread::Thread> ThreadPageLocker;
    
    typedef PageManager<FilePersistenceManager<ThreadPageLocker>> Pager;
    
    typedef DataPage<Index, types::FieldType, IndexReader, IndexWriter> 
        IndexDataPage;

    typedef DataPage<Row, std::shared_ptr<const RowFormat>, RowReader, RowWriter> 
        RowDataPage;

    typedef BTree<Pager, RowIndexer, IndexDataPage, RowDataPage> 
        TableTree;
    typedef std::shared_ptr<TableTree> TablePtr; 
    typedef concurrency::pthread::Mutex Mutex;


    // Numer strony stanowiacej korzen tabeli sysobjects
    static const int SYSOBJECTS_ROOT = 2;
    
    /*
    // Zwraca numer strony korzenia danej tabeli. Jesli jest w mapie tabel
    // nie jest wyszukiwana w bazie danych. W przeciwnym wypadku wymaga to
    // przegladniecia tabeli sysobjects.
    page_number find_table_root_(const string& table_name);
    */
    
    // Zwraca tabele o podanej nazwie. Jesli jest w cache, to stamtad jest 
    // brana. W przeciwnym wypadku wczytywana jest z dysku. Jesli nie istnieje,
    // zwracane jest nullptr.
    TablePtr get_table_(const string& table_name);
    
    // Tworzy sysobjects_ i syscolumns_
    void create_system_formats_();
    
    // Tworzy reprezentacje kolumny dla tabeli columns
    Row create_column_description_(const Column& column, object_id owner, 
        object_id id);

    // Wstawia dane tabeli jedynie do mapy tabel i tworzy jej strukture
    // na dysku. Osobno trzeba umiescic informacje w sysobjects.
    TablePtr insert_to_table_map_(const string& name, FormatPtr format, 
        page_number page = TableTree::ALLOC_NEW);
        
    // Dodaje dane tabeli do tabel sysobjects i columns
    void add_table_to_sysobjects_(const string& name, FormatPtr format, 
        page_number page, object_id id, object_id owner = NULL_OBJ);
    
    // Tworzy obiekt kolumny z wiersza ja opisujacego
    Column parse_column_(const Row& row);
    
    // Wczytuje format tabeli z podanego wiersza - wyszukuje informacje
    // o kolumnach w tabeli columns
    FormatPtr parse_table_(const Row& row);
    
    // Znajduje wartosc najwiekszego indeksu w podanej tabeli.
    // Jest on poszukiwany wsrod wartosci pierwszej kolumny (klucza glownego).
    // Synchronizacje nalezy zapewnic we wlasnym zakresie.
    int max_index_(const string& name);
    
    // Wczytuje od nowa wszystkie definicje tabel (poza systemowymi)
    void update_table_defs_();
    
    
    Pager page_manager_;
    
    std::unordered_map<string, TablePtr> tables_;
    
    std::shared_ptr<const RowFormat> 
        sysobjects_fmt_,    // tabela glowna zawierajaca tabele i indeksy 
        syscolumns_fmt_;       // tabela z definicjami kolumn
    
    TablePtr sysobjects_, syscolumns_;
    
    Mutex mutex_; // broni dostepu do struktur
};


DatabaseEngine::DatabaseEngine(const string& path, bool create)
{
    if (create)
        page_manager_.createFile(path);
    page_manager_.openFile(path);
    
    // Tworzymy format wiersza glownej tabeli: tej zawierajacej informacje
    // o tabelach i indeksach, oraz format tabeli kolumn
    create_system_formats_();
    
    page_number sysobjects_page = create ? TableTree::ALLOC_NEW : 
        SYSOBJECTS_ROOT;
        
    sysobjects_ = insert_to_table_map_("sysobjects", sysobjects_fmt_, 
        sysobjects_page);
    
    page_number syscolumns_page = create ? TableTree::ALLOC_NEW : 
        (SYSOBJECTS_ROOT + 1);
        
    syscolumns_ = insert_to_table_map_("syscolumns", syscolumns_fmt_, 
        syscolumns_page);
    
    if (create)
    {   
        add_table_to_sysobjects_("sysobjects", sysobjects_fmt_, 
            SYSOBJECTS_ROOT, 0);
        add_table_to_sysobjects_("syscolumns", syscolumns_fmt_, 
            SYSOBJECTS_ROOT + 1, 1);
    }
}


DatabaseEngine::TablePtr DatabaseEngine::get_table_(const string& table_name)
{
    using namespace types;
    auto lock = concurrency::make_lock(mutex_);
    
    auto i = tables_.find(table_name);
    if (i == tables_.end())
    {
        Row def = sysobjects_->find([&table_name](const Row& row)
        {
            return table_name == data_cast<ContentType::VarChar>(*row["name"]);
        });
        if (def)
        {
            FormatPtr format = parse_table_(def);
            page_number root = data_cast<ContentType::Int>(*def["root_page"]);
            return insert_to_table_map_(table_name, format, root);
        }
        else
            return nullptr;
    }
    else
        return i->second;
}


void DatabaseEngine::create_system_formats_()
{
    using namespace types;
    
    sysobjects_fmt_.reset(new RowFormat { 
        { ContentType::Int,        "id"}, 
        { ContentType::VarChar,    "name"},
        { ContentType::Int,        "owner"},
        {{ContentType::Char, 2},   "type"},
        { ContentType::Int,        "root_page"}
    });
    
    syscolumns_fmt_.reset(new RowFormat {
        { ContentType::Int,       "id"},
        { ContentType::VarChar,   "name"},
        { ContentType::Int,       "owner"},
        { ContentType::FieldType, "field_type"},
        { ContentType::Int,       "column_nr"}
    });
}


DatabaseEngine::TablePtr 
DatabaseEngine::insert_to_table_map_(const string& name, FormatPtr format,
    page_number page)
{
    auto lock = concurrency::make_lock(mutex_);
    TablePtr table(new TableTree(page_manager_, page, RowIndexer(format, 0)));
    tables_.insert(std::make_pair(name, table));
    return table;
}


void DatabaseEngine::add_table_to_sysobjects_(const string& name, 
    FormatPtr format, page_number page, object_id id, object_id owner)
{
    using types::make_field;
    
    Row table_def(sysobjects_fmt_, { 
        make_field(id),
        make_field(name),
        make_field(owner),
        make_field(2, "T"),
        make_field(page)
    });
    sysobjects_->insert(table_def);
    int i = max_index_("syscolumns") + 1;
    for (const Column& column: *format)
    {
        Row row = create_column_description_(column, id, i ++);
        syscolumns_->insert(row);    
    }
}


Column DatabaseEngine::parse_column_(const Row& row)
{
    using namespace types;

    FieldType type = data_cast<ContentType::FieldType>(*row["field_type"]);
    string name = data_cast<ContentType::VarChar>(*row["name"]);
    column_number column = data_cast<ContentType::Int>(*row["column_nr"]);
    
    return {type, name, 0, column};
}


void DatabaseEngine::createTable(const string& name, FormatPtr format)
{
    TablePtr table = insert_to_table_map_(name, format);
    object_id id = max_index_("sysobjects") + 1;
    add_table_to_sysobjects_(name, format, table->root(), id);
}


Row DatabaseEngine::create_column_description_(const Column& column, 
    object_id owner, object_id id)
{
    using types::make_field;
    return Row(syscolumns_fmt_, {
        make_field(id),
        make_field(column.name),
        make_field(owner),
        make_field(column.type),
        make_field(static_cast<int>(column.col))
    });
}


DatabaseEngine::FormatPtr DatabaseEngine::parse_table_(const Row& row)
{
    using namespace types;

    object_id id = data_cast<ContentType::Int>(*row["id"]);

    // Musimy sie dobrac do tabeli columns
    std::vector<Row> rows;
    syscolumns_->findAll([id](const Row& row)
    {
        return data_cast<ContentType::Int>(*row["owner"]) == id;
    },
    std::back_inserter(rows));
    // Sortujemy po numerze kolumny
    std::sort(rows.begin(), rows.end(), [](const Row& r1, const Row& r2)
    {
        return data_cast<ContentType::Int>(*r1["column_nr"])
            < data_cast<ContentType::Int>(*r2["column_nr"]);
    });
    std::unique_ptr<RowFormat> format(new RowFormat);
    for (const Row& row: rows)
    {
        format->addColumn(parse_column_(row));
    }
    return FormatPtr(format.release());
}


int DatabaseEngine::max_index_(const string& name)
{
    using namespace types;
    TablePtr table = get_table_(name);
    if (table == nullptr)
    {
        throw DatabaseError(util::format("Table '{}' does not exist "
            "(trying to find max index)", name));
    }
    Row max = table->max();
    return max ? data_cast<ContentType::Int>(*max[0]) : 0;
}


void DatabaseEngine::update_table_defs_()
{
    
}


} // engine
} // paganini

#endif // __PAGANINI_DATABASE_DATABASE_ENGINE_H__

