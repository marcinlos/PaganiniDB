AM_CPPFLAGS = -iquote ../include

noinst_LTLIBRARIES = libpaging.la
libpaging_la_SOURCES = page.c \
                       page_manager.c

libpaging_la_includedir = ${includedir}/paganini/paging
libpaging_la_include_HEADERS = ../include/paging/page.h \
    ../include/paging/page_header.h \
    ../include/paging/page_manager.h \
    ../include/paging/types.h \
    ../include/paging/dbfile_header.h
