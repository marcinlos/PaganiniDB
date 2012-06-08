#include "config.h"
#include <paganini/indexing/IndexWriter.h>
#include <paganini/io/OutputBinaryStream.h>


namespace paganini
{


size16 IndexWriter::write(raw_data buffer, const Index& index)
{
    OutputBinaryStream stream(buffer);
    stream.write(index.child());
    
    // Dla typow o zmiennej dlugosci zapisujemy ja przed zawartoscia
    if (index.value()->isVariableSize())
    {
        size16 size = index.value()->size();
        stream.write(size);
    }

    index.value()->writeTo(stream);
    
    return stream.getOffset();
}


}
