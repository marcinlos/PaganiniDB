#include "config.h"
#include <paganini/row/Row.h>
#include <paganini/util/format.h>
#include <stdexcept>


namespace paganini
{

// Wektor inicjalizujemy nullptr-ami, alokujemy od razu na calosc
Row::Row(const RowFormat& format, 
    std::initializer_list<types::Data*> fields,
    row_flags flags):
    
    _format(format), 
    _fields(format.columnCount(), nullptr),
    _flags(flags)
{
    int i = 0;
    for (types::Data* data: fields)
        setField(i++, data);
}


Row::Row(const RowFormat& format, row_flags flags): 
    _format(format), 
    _fields(format.columnCount(), nullptr),
    _flags(flags)
{
}


Row::Row(Row&& other): 
    _format(other._format),
    _fields(std::move(other._fields))
{
}


void Row::setField(size16 index, types::Data* data)
{
    // Najpierw sprawdzamy rozmiar
    if (index < _format.columnCount())
    {
        // Potem typ
        const Column& col = _format.columns()[index];
        if (data != nullptr && data->type() != col.type)
        {
            throw std::logic_error(util::format("Types don't match; "
                "got {}, {} expected", data->type(), col.type));
        }
        // Zapisujemy w wektorze
        _fields[index] = DataPtr(data);
    }
    else
        throw std::logic_error(util::format("Too many fields; row has "
            "{} columns", _format.columnCount()));    
}


const RowFormat& Row::format() const
{
    return _format;
}


row_flags Row::flags() const
{
    return _flags;
}


// Forwardujemy po prostu do wektora

Row::iterator Row::begin()
{
    return _fields.begin();
}


Row::iterator Row::end()
{
    return _fields.end();
}


Row::const_iterator Row::begin() const
{
    return _fields.begin();
}


Row::const_iterator Row::end() const
{
    return _fields.end();
}


// A tu forwardujemy do RowFormat...

size16 Row::columnCount() const
{
    return _format.columnCount();
}


size16 Row::fixedColumnCount() const
{
    return _format.fixedColumnCount();
}


size16 Row::variableColumnCount() const
{
    return _format.variableColumnCount();
}


const std::vector<Row::DataPtr>& Row::columns() const
{
    return _fields;
}


Row::FieldPtrVector Row::fixed() const
{
    return {
        _format.fixedIndices().begin(), 
        _format.fixedIndices().end(), 
        _fields.begin()
    };
}


Row::FieldPtrVector Row::variable() const
{
    return {
        _format.variableIndices().begin(),
        _format.variableIndices().end(), 
        _fields.begin()
    };
}


column_number Row::getColumnNumber(const string& name) const
{
    return _format.getColumnNumber(name);
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
    return _fields[col];
}


}
