#include "config.h"
#include <paganini/row/Row.h>
#include <paganini/util/format.h>
#include <stdexcept>


namespace paganini
{

// Wektor inicjalizujemy nullptr-ami, alokujemy od razu na calosc
Row::Row(FormatPtr format, std::initializer_list<types::Data*> fields,
    row_flags flags):
    
    format_(format), 
    fields_(format->columnCount(), nullptr),
    flags_(flags)
{
    int i = 0;
    for (types::Data* data: fields)
        setField(i++, data);
}


Row::Row(FormatPtr format, row_flags flags): 
    format_(format), 
    fields_(format->columnCount(), nullptr),
    flags_(flags)
{
}


Row::Row(Row&& other): 
    format_(other.format_),
    fields_(std::move(other.fields_))
{
}


void Row::setField(size16 index, types::Data* data)
{
    // Najpierw sprawdzamy rozmiar
    if (index < format_->columnCount())
    {
        // Potem typ
        const Column& col = format_->columns()[index];
        if (data != nullptr && data->type() != col.type)
        {
            throw std::logic_error(util::format("Types don't match; "
                "got {}, {} expected", data->type(), col.type));
        }
        // Zapisujemy w wektorze
        fields_[index] = DataPtr(data);
    }
    else
        throw std::logic_error(util::format("Too many fields; row has "
            "{} columns", format_->columnCount()));    
}


const RowFormat& Row::format() const
{
    return *format_;
}


row_flags Row::flags() const
{
    return flags_;
}


// Forwardujemy po prostu do wektora

Row::iterator Row::begin()
{
    return fields_.begin();
}


Row::iterator Row::end()
{
    return fields_.end();
}


Row::const_iterator Row::begin() const
{
    return fields_.begin();
}


Row::const_iterator Row::end() const
{
    return fields_.end();
}


// A tu forwardujemy do RowFormat...

size16 Row::columnCount() const
{
    return format_->columnCount();
}


size16 Row::fixedColumnCount() const
{
    return format_->fixedColumnCount();
}


size16 Row::variableColumnCount() const
{
    return format_->variableColumnCount();
}


const std::vector<Row::DataPtr>& Row::columns() const
{
    return fields_;
}


Row::FieldPtrVector Row::fixed() const
{
    return {
        format_->fixedIndices().begin(), 
        format_->fixedIndices().end(), 
        fields_.begin()
    };
}


Row::FieldPtrVector Row::variable() const
{
    return {
        format_->variableIndices().begin(),
        format_->variableIndices().end(), 
        fields_.begin()
    };
}


column_number Row::getColumnNumber(const string& name) const
{
    return format_->getColumnNumber(name);
}


bool Row::isNull(const string& name) const
{
    return (*this)[name] == nullptr;
}


bool Row::isNull(column_number col) const
{
    return (*this)[col] == nullptr;
}


// Implementacja identyczna jak w RowFormatterze

Row::FieldProxy Row::operator [] (const string& name)
{
    column_number col = getColumnNumber(name);
    if (col != NULL_COLUMN)
        return (*this)[col];
    else
        throw std::logic_error(util::format("Column '{}' does not exist", 
            name));
}


Row::ConstDataPtr Row::operator [] (const string& name) const
{
    column_number col = getColumnNumber(name);
    if (col != NULL_COLUMN)
        return (*this)[col];
    else
        throw std::logic_error(util::format("Column '{}' does not exist", 
            name));
}


Row::FieldProxy Row::operator [] (column_number col)
{
    return { *this, col };
}


Row::ConstDataPtr Row::operator [] (column_number col) const
{
    return fields_[col];
}


}
