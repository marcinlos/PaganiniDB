#include "config.h"
#include <paganini/paging/Page.h>
#include <paganini/paging/PageManager.h>
#include <paganini/Error.h>
#include <paganini/row/datatypes.h>
#include <paganini/row/RowFormat.h>
#include <paganini/row/Row.h>
#include <cstdio>
using namespace paganini;


int main()
{
    printf("Size: %d\n", sizeof(Page));
    try
    {
        PageManager manager;
        manager.createFile("dupa");
        manager.openFile("dupa");
        printf("%u, %u\n", DATA_SIZE, DATA_OFFSET);
        /*
        for (int i = 0; i < 10; ++ i)
        {
            manager.allocPage();
        }
        */
        page_number pn = manager.allocPage();
        Page page;
        manager.readPage(pn, &page);
        page_data data = page.data;
     
        RowFormat fmt = 
        { 
            {types::FieldType::Int, "Dupa"}, 
            {types::FieldType::Float, "Sranie"},
            {types::FieldType::VarChar, "Jebaka"},
            {types::FieldType::VarChar, "madafaka"}
        };
        //fmt.print();
        fmt["Dupa"];
        for (const Column& c: fmt.fixed())
        {
            printf("%s\n", c.name.c_str());
        }
        auto it = fmt.variable().begin();
        printf("%s\n%s\n", it->name.c_str(), (*it).name.c_str());
        printf("%s\n", fmt.fixed()[1].name.c_str());
        
        Row row(fmt, { new types::Int(432), new types::Float(1.23),
            new types::VarChar("Spadaj"), new types::VarChar("Cieciu") });
            
        printf("%d\n", static_cast<int>(row["Jebaka"]->type()));
        
        for (std::shared_ptr<types::Data> p: row)
        {
            printf("%d\n", static_cast<int>(p->type()));
        }
        
        types::Int i(34567);
        i.writeTo(data);
        //data += i.size();
        //types::VarChar text("Dupa jasia");
        //text.writeTo(data);
        types::Int jj;
        jj.readFrom(data, data + jj.size());
        printf("%d\n", jj.val);
        manager.writePage(pn, &page);
        
        manager.closeFile();
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
