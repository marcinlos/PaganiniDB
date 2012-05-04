SUBDIRS = error_msg paging row

AM_CPPFLAGS = -iquote include

lib_LTLIBRARIES = libpaganini.la
libpaganini_la_SOURCES = 
libpaganini_la_LIBADD = error_msg/liberrormsg.la \
                        paging/libpaging.la \
                        row/librow.la

libpaganini_la_includedir = ${includedir}/paganini
libpaganini_la_include_HEADERS = include/paganini.h \
                                 include/util/bits.h \
                                 include/util/format.h \
                                 include/util/IndexedIterator.h \
                                 include/util/IndexedView.h

noinst_PROGRAMS = test insp
test_SOURCES = test.cpp
test_LDADD = libpaganini.la

insp_SOURCES = inspector/cmd.cpp \
               inspector/main.cpp \
               inspector/operations.cpp
                    
insp_LDADD = libpaganini.la

