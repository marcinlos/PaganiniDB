AM_CPPFLAGS = -iquote ../include

noinst_LTLIBRARIES = libpaging.la
libpaging_la_SOURCES = PageHeader.cpp \
                       Page.cpp \
                       DatabaseHeader.cpp \
                       PageManager.cpp

libpaging_la_includedir = ${includedir}/paganini/paging
libpaging_la_include_HEADERS = ../include/paging/Page.h \
    ../include/paging/PageHeader.h \
    ../include/paging/PageManager.h \
    ../include/paging/types.h \
    ../include/paging/DatabaseHeader.h
