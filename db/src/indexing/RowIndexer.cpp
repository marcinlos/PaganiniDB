#include "config.h"
#include <paganini/indexing/RowIndexer.h>
#include <paganini/indexing/ComparatorFactory.h>
#include <paganini/util/format.h>
#include <stdexcept>


namespace paganini
{


RowIndexer::RowIndexer(const RowFormat& fmt, column_number column):
    type_(types::ContentType::None)
{
    fromNumber_(fmt, column);
}


RowIndexer::RowIndexer(const RowFormat& fmt, const string& name):
    type_(types::ContentType::None)
{
    column_ = fmt.getColumnNumber(name);
    if (column_ >= 0)
        fromNumber_(fmt, column_);
    else
        throw std::logic_error(util::format("Trying to create index on column "
            "'{}', which does not exist", name));
}


void RowIndexer::fromNumber_(const RowFormat& fmt, column_number column)
{
    if (column < fmt.columnCount())
    {
        column_ = column;
        const Column& col = fmt[column];
        type_ = col.type;
        ComparatorFactory& factory = ComparatorFactory::getInstance();
        comparator_ = factory.get(type_.content);
        
    }
    else 
        throw std::logic_error(util::format("Trying to create index on column "
            "nr {}, which does not exist; {} columns are available", 
            column, fmt.columnCount()));
}


RowIndexer::ReturnType RowIndexer::operator ()(const Row& a, 
    page_number dest) const
{
    return ReturnType(new Index(type_, a[column_], dest));
}


int RowIndexer::operator ()(const Row& a, const Row& b) const
{
    return (*comparator_)(*a[column_], *b[column_]);
}


int RowIndexer::operator ()(const Index& a, const Index& b) const
{
    return (*comparator_)(*a.value(), *b.value());
}


}
