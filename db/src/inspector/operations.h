#ifndef __OPERATIONS_H__
#define __OPERATIONS_H__

#include <string>
#include <vector>
using std::string;
using std::vector;

void print_page_header(const vector<string>& args);

void print_db_header(const vector<string>& args);

void print_uv_content(const vector<string>& args);

#endif /* ! __OPERATIONS_H__ */
