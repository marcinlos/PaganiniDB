#include "config.h"
#include <paganini/paging/Page.h>
#include <paganini/paging/PageManager.h>
#include <paganini/Error.h>
#include <paganini/row/datatypes.h>
#include <paganini/row/RowFormat.h>
#include <paganini/row/Row.h>
#include <paganini/row/FieldMetadata.h>
#include <cstdio>
#include <iostream>
using namespace paganini;


void database_creation()
{
    PageManager& manager = PageManager::getInstance();
    manager.createFile("db");
    manager.openFile("db");
    page_number pn = manager.allocPage();
    manager.closeFile();
}

void row_format_test()
{
    RowFormat fmt = 
    { 
        {types::FieldType::Int, "count"}, 
        {types::FieldType::Float, "variance"},
        {types::FieldType::Char, "Name", 0, 14},
        {types::FieldType::VarChar, "Surname"},
        {types::FieldType::VarChar, "Description"}
    };
    std::cout << "Po nazwie: Name ma rozmiar " << fmt["Name"].size << std::endl;
    std::cout << "Wszystkie kolumny:" << std::endl;
    for (const Column& c: fmt)
    {
        std::cout << "- " << c.name << "(" << c.size << ")" << std::endl;
    }
    std::cout << "Pola o zmiennym rozmiarze:" << std::endl;
    auto view = fmt.variable();
    for (auto it = view.begin(); it != view.end(); ++ it)
    {
        std::cout << "- " << (*it).name << std::endl;
    }
    std::cout << "Pola o stalym rozmiarze (random access):" << std::endl;
    auto view2 = fmt.fixed();
    for (int i = 0; i < fmt.fixedColumnCount(); ++ i)
    {
        std::cout << "- " << view2[i].name << std::endl;
    }
    std::cout << "Laczna dlugosc tych o stalym rozmiarze: "
        << fmt.totalFixedSize() << std::endl;
}

void row_test()
{
    RowFormat fmt = 
    { 
        {types::FieldType::Int, "count"}, 
        {types::FieldType::Float, "variance"},
        {types::FieldType::Char, "Name", 0, 14},
        {types::FieldType::VarChar, "Surname"},
        {types::FieldType::VarChar, "Description"}
    };
    
    Row row(fmt, { 
        new types::Int(432), 
        new types::Float(1.23),
        new types::Char(14, "Aram"), 
        new types::VarChar("Khachaturian"),
        new types::VarChar("Kompozytor radziecki")
    });
    
    std::cout << "Wartosci:" << std::endl;
    for (std::shared_ptr<types::Data> p: row)
    {
        std::cout << p->toString() << std::endl;
    }
    /*
    std::unique_ptr<types::Data> ddata = 
        FieldMetadata::getInstance().create(types::FieldType::Float);
    printf("float = %s\n", ddata->toString().c_str());
    */
    std::cout << "Pole 'Name': " << row["Name"]->toString() << std::endl;
    row["Surname"] = new types::VarChar("Blabla");
    std::cout << "Wartosci:" << std::endl;
    for (std::shared_ptr<types::Data> p: row)
    {
        std::cout << p->toString() << std::endl;
    }
}


int main()
{
    try
    {     
        database_creation();
        row_format_test();
        row_test();
        /*        
        Row row(fmt, { new types::Int(432), new types::Float(1.23),
            new types::VarChar("Spadaj"), new types::VarChar("Cieciu") });
            
        printf("%d\n", static_cast<int>(row["Jebaka"]->type()));
        
        for (std::shared_ptr<types::Data> p: row)
        {
            printf("val: %s\n", p->toString().c_str());
        }
        
        std::unique_ptr<types::Data> ddata = 
            FieldFactory::getInstance().create(types::FieldType::Float);
        printf("float = %s\n", ddata->toString().c_str());
        
        types::Int i(34567);
        i.writeTo(data);
        //data += i.size();
        //types::VarChar text("Dupa jasia");
        //text.writeTo(data);
        types::Int jj;
        jj.readFrom(data, data + jj.size());
        printf("%d\n", jj.val);
        manager.writePage(pn, &page);
        
        manager.closeFile();*/
        
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
