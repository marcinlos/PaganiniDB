#include "config.h"
#include <paganini/indexing/BTree.h>
#include <paganini/inspect/InfoFormatter.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <array>
#include <sstream>
#include <string>
using namespace paganini;
using std::string;

namespace paganini
{
namespace mock
{

// Mockowate obiekty
struct MockRow
{
    int key;
    string value;
};

template <typename T>
struct MockDataPage
{
    static int MAX_IN_ROW = 7;
    std::vector<T> rows;
    
    int prev, next;
    
    T row(int i) { return rows[i]; }
    int rowFormat() const { return 0; }
    
    MockDataPage(): prev(NULL_PAGE), next(NULL_PAGE) { }
    int rowFormat() { return 0; }
    
    //bool canFit(const T& t) { 
};


struct MockIndexer
{
    typedef RowType T;
    typedef int IndexType;
    
    int operator () (const T& row, int n = 0)
    {
        return row.key;
    }
};


struct PagingSystemMock
{
    static const size16 SIZE = 64;
    std::vector<std::array<char, SIZE>> pages;
    std::vector<bool> usage;
    
    void createFile(const string& path){}

    void openFile(const string& path){}

    void closeFile() {}
    
    void readPage(page_number number, char* buffer)
    {
        std::copy(pages[number].begin(), pages[number].end(), buffer);
    }

    void writePage(page_number number, const char* buffer)
    {
        std::copy(buffer, buffer + SIZE, pages[number].begin());
    }
    
    page_nubmer allocPage()
    {
        for (unsigned i = 0; i < usage.size(); ++ i)
        {
            if (! usage[i])
            {
                usage[i] = true;
                return i;
            }
        }
        pages.push_back((std::array<char, SIZE>));
        usage.push_back(true);
        return pages.size() - 1;
    }
};

struct test
{
    template <typename T>
    T operator (int a) const { return T() + a; }
};


int main()
{
    test t;
    std::cout << text<int>(7);
    return 0;
}
