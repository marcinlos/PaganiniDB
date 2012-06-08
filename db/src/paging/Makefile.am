AM_CPPFLAGS = -I ../include

noinst_LTLIBRARIES = libpaging.la
libpaging_la_SOURCES = PageHeader.cpp \
                       Page.cpp \
                       DatabaseHeader.cpp \
                       PageManager.cpp \
                       FilePersistenceManager.cpp \
                       DataPage.cpp

libpaging_la_includedir = ${includedir}/paganini/paging
libpaging_la_include_HEADERS = ../include/paganini/paging/Page.h \
                               ../include/paganini/paging/PageHeader.h \
                               ../include/paganini/paging/PageManager.h \
                               ../include/paganini/paging/FilePersistenceManager.h \
                               ../include/paganini/paging/DummyLocker.h \
                               ../include/paganini/paging/types.h \
                               ../include/paganini/paging/DatabaseHeader.h \
                               ../include/paganini/paging/DataPage.h
