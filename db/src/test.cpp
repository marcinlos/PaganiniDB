#include "config.h"
#include <paganini/paging/Page.h>
#include <paganini/paging/PageManager.h>
#include <paganini/Error.h>
#include <paganini/row/datatypes.h>
#include <paganini/row/RowFormat.h>
#include <paganini/row/Row.h>
#include <paganini/row/RowWriter.h>
#include <paganini/row/RowReader.h>
#include <paganini/row/FieldFactory.h>
#include <cstdio>
#include <iostream>
#include <sstream>
using namespace paganini;

inline char hex_digit(int i)
{
    return i < 10 ? i + '0' : i - 10 + 'A';
}

string format_bytes(raw_data data, size16 len, size16 in_line = 16)
{
    std::ostringstream ss;
    for (int i = 0; i < len; ++ i)
    {
        unsigned char c = static_cast<unsigned char>(data[i]);
        ss << hex_digit(c / 16);
        ss << hex_digit(c % 16);
        ss << ' ';
        if ((i + 1) % in_line == 0)
            ss << std::endl; 
        else if ((i + 1) % 2 == 0)
            ss << ' ';
    }
    return ss.str();
}

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
        {types::ContentType::Int, "count"}, 
        {types::ContentType::Float, "variance"},
        {{types::ContentType::Char, 14}, "Name"},
        {types::ContentType::VarChar, "Surname"},
        {types::ContentType::VarChar, "Description"}
    };
    //std::cout << "Po nazwie: Name ma rozmiar " << fmt["Name"].size << std::endl;
    std::cout << "Wszystkie kolumny:" << std::endl;
    /*for (const Column& c: fmt)
    {
        std::cout << "- " << c.name << "(" << c.size << ")" << std::endl;
    }*/
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
    //std::cout << "Laczna dlugosc tych o stalym rozmiarze: "
    //    << fmt.totalFixedSize() << std::endl;
}

void row_test()
{    
    RowFormat fmt = 
    { 
        {types::ContentType::Int, "count"}, 
        {types::ContentType::Float, "variance"},
        {{types::ContentType::Char, 14}, "Name"},
        {types::ContentType::VarChar, "Surname"},
        {types::ContentType::VarChar, "Description"}
    };

    Row row(fmt, { 
        new types::Int(432), 
        new types::Float(1.23),
        new types::Char(14, "Aram"),
        new types::VarChar("Khachaturian"),
        new types::VarChar("Kompozytor radziecki")
    });
    
    std::cout << "Wartosci:" << std::endl;
    for (auto p: row.variable())
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
    
    std::cout << "Test zapisywania w pamieci" << std::endl;
    row["Name"] = nullptr;
    char buffer[PAGE_SIZE] = { 0 };
    RowWriter rw;
    rw.write(buffer, row);
    std::cout << format_bytes(buffer, 300) << std::endl;
    
    RowReader rr;
    std::cout << *rr.read(buffer, fmt);
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
