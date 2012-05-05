#include "config.h"
#include <paganini/row/RowFormat.h>
#include <paganini/util/format.h>
#include <stdexcept>

namespace paganini
{


Column::Column(types::FieldType type, string name, column_flags flags):
    type(type), name(name), flags(flags), col(NULL_COLUMN)
{
}


Column::Column(Column&& other)
{
    name = std::move(other.name);
    col = other.col;
    flags = other.flags;
    type = other.type;
}
    
    
Column::Column(const Column& other): name(other.name), col(other.col),
    type(other.type), flags(other.flags)
{
}


RowFormat::RowFormat(const std::vector<Column>& cols)
{
    for (const Column& c: cols)
        addColumn(c);
}


RowFormat::RowFormat(std::initializer_list<Column> cols)
{
    for (const Column& c: cols)
        addColumn(c);
}


void RowFormat::addColumn(const Column& col)
{
    // Kopiujemy dane do wektora
    _columns.push_back(col);
    Column& c = _columns.back();
    c.col = _columns.size() - 1;
    
    // Uaktualniamy wektory pomocnicze
    if (types::is_variable_size(c.type))
        _variable.push_back(c.col);
    else
        _fixed.push_back(c.col);
        
    names[c.name] = c.col;
}


RowFormat::const_iterator RowFormat::begin() const
{
    return _columns.begin();
}


RowFormat::const_iterator RowFormat::end() const
{
    return _columns.end();
}


size16 RowFormat::columnCount() const 
{
    return _columns.size();
}


size16 RowFormat::fixedColumnCount() const
{
    return _fixed.size();
}
    

size16 RowFormat::variableColumnCount() const
{
    return _variable.size();
}


const std::vector<Column>& RowFormat::columns() const
{
    return _columns;
}


RowFormat::ColumnVector RowFormat::fixed() const
{
    return {_fixed.begin(), _fixed.end(), _columns.begin()};
}


const std::vector<int>& RowFormat::fixedIndices() const
{
    return _fixed;
}


RowFormat::ColumnVector RowFormat::variable() const
{
    return {_variable.begin(), _variable.end(), _columns.begin()};
}


const std::vector<int>& RowFormat::variableIndices() const
{
    return _variable;
}


column_number RowFormat::getColumnNumber(const string& name) const
{
    auto i = names.find(name);
    return i != names.end() ? i->second : NULL_COLUMN;
}


Column& RowFormat::operator [] (const string& name)
{
    column_number col = getColumnNumber(name);
    if (col != NULL_COLUMN)
        return (*this)[col];
    else
        throw std::logic_error(util::format("Column '{}' does not exist", 
            name));
}


const Column& RowFormat::operator [] (const string& name) const
{
    column_number col = getColumnNumber(name);
    if (col != NULL_COLUMN)
        return (*this)[col];
    else
        throw std::logic_error(util::format("Column '{}' does not exist", 
            name));
}


Column& RowFormat::operator [] (column_number col)
{
    return _columns[col];
}


const Column& RowFormat::operator [] (column_number col) const
{
    return _columns[col];
}


}
