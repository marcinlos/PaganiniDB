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
#include <paganini/paging/DataPage.h>
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

string format_bytes(raw_data data, size16 len, size16 in_line = 16)
{
    std::ostringstream ss;
    int lines = (len + in_line - 1) / in_line;
    for (int i = 0; i < len / in_line; ++ i)
    {
        ss << std::setw(4) << std::hex << i * in_line << " |";
        unsigned char* d = reinterpret_cast<unsigned char*>(data + i * in_line);
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
    void rowFormatTest();
    void rowTest();
    void dataPageTest();
    
private:
    RowFormat fmt;
    Row row;
    
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
}

void Test::databaseCreationTest()
{
    PageManager& manager = PageManager::getInstance();
    manager.createFile("db");
    manager.openFile("db");
    page_number pn = manager.allocPage();
    manager.closeFile();
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

void Test::rowFormatTest()
{
    std::cout << "Nasz wiersz:" << std::endl;
    std::cout << fmt << std::endl;

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
}

void Test::rowTest()
{        
    std::cout << "Wartosci:" << std::endl;
    for (auto p: row.variable())
    {
        std::cout << p->toString() << std::endl;
    }
    
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
    std::cout << "Przewidywany rozmiar: " << rw.size(row) << std::endl;
    rw.write(buffer, row);
    std::cout << "Zrzut pamieci zapisanej przez RowWritera:" << std::endl;
    std::cout << format_bytes(buffer, 300) << std::endl;
    
    RowReader rr;
    std::cout << "Wiersz odczytany z powrotem przez RowReadera:" << std::endl;
    std::cout << *rr.read(buffer, fmt);
}


void Test::dataPageTest()
{
    DataPage page;
    page.insertRow(row, 0);
    setRow(321, 6.66f, "David", "Hilbert", "Matematyk niemiecki");
    page.insertRow(row, 0);
    setRow(7788, 1.432f, "Sergiusz", "Prokofiew", 
        "Radziecki kompozytor klasyczny");
    page.insertRow(row, 2);
    
    std::cout << "Calosc strony:" << std::endl;
    std::cout << format_bytes(page.page().buffer(), PAGE_SIZE) << std::endl;
    
    std::cout << "Offset pierwszego: " << page.offset(0) << std::endl;
    std::cout << "Offset drugiego: " << page.offset(1) << std::endl;
    std::cout << "Offset trzeciego: " << page.offset(2) << std::endl;
    
    std::cout << "Test odczytu: " << std::endl;
    RowReader reader;
    for (int i = 0; i < page.rowCount(); ++ i)
    {
        std::cout << "Wiersz nr " << i << std::endl;
        std::cout << *reader.read(page.rowData(i), fmt) << std::endl;
    }
}


int main()
{
    try
    {     
        Test test;
        test.databaseCreationTest();
        test.rowFormatTest();
        test.rowTest();
        test.dataPageTest();
        /*        
        Row row(fmt, { new types::Int(432), new types::Float(1.23),
            new types::VarChar("Spadaj"), new types::VarChar("Cieciu") });
            
        printf("%d\n", static_cast<int>(row["Costam"]->type()));
        
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
