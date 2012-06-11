SUBDIRS = error_msg util paging row io indexing inspect

AM_CPPFLAGS = -I include

lib_LTLIBRARIES = libpaganini.la
libpaganini_la_SOURCES = 
libpaganini_la_LIBADD = error_msg/liberrormsg.la \
                        util/libutil.la \
                        paging/libpaging.la \
                        row/librow.la \
                        io/libio.la \
                        indexing/libindexing.la \
                        inspect/libinspect.la

libpaganini_la_includedir = ${includedir}/paganini
libpaganini_la_include_HEADERS = include/paganini/paganini.h


noinst_PROGRAMS = test insp dbshell
test_SOURCES = test.cpp
test_LDADD = libpaganini.la

insp_SOURCES = inspector/cmd.cpp \
               inspector/main.cpp \
               inspector/operations.cpp
                    
insp_LDADD = libpaganini.la

dbshell_SOURCES = tools/dbshell/shell.cpp \
                  tools/dbshell/Interpreter.cpp 
                  
dbshell_LDADD = libpaganini.la

