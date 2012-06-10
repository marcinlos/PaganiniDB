AM_CPPFLAGS = -I ../include

noinst_LTLIBRARIES = libindexing.la
libindexing_la_SOURCES = ComparatorFactory.cpp \
                         RowIndexer.cpp \
                         Index.cpp \
                         IndexReader.cpp \
                         IndexWriter.cpp

libindexing_la_includedir = ${includedir}/paganini/indexing

libindexing_la_include_HEADERS = ../include/paganini/indexing/RowIndexer.h \
                                 ../include/paganini/indexing/ComparatorFactory.h \
                                 ../include/paganini/indexing/Index.h \
                                 ../include/paganini/indexing/IndexReader.h \
                                 ../include/paganini/indexing/IndexWriter.h \
                                 ../include/paganini/indexing/BTree.h
