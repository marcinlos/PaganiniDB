SUBDIRS = error_msg paging row io

AM_CPPFLAGS = -I include

lib_LTLIBRARIES = libpaganini.la
libpaganini_la_SOURCES = 
libpaganini_la_LIBADD = error_msg/liberrormsg.la \
                        paging/libpaging.la \
                        row/librow.la \
                        io/libio.la

libpaganini_la_includedir = ${includedir}/paganini
libpaganini_la_include_HEADERS = include/paganini/paganini.h

utildir = ${includedir}/paganini/util
util_HEADERS = include/paganini/util/bits.h \
               include/paganini/util/Bitmap.h \
               include/paganini/util/format.h \
               include/paganini/util/IndexedIterator.h \
               include/paganini/util/IndexedView.h \
               include/paganini/util/Singleton.h

noinst_PROGRAMS = test insp
test_SOURCES = test.cpp
test_LDADD = libpaganini.la

insp_SOURCES = inspector/cmd.cpp \
               inspector/main.cpp \
               inspector/operations.cpp
                    
insp_LDADD = libpaganini.la

