SUBDIRS = error_msg util paging row io indexing concurrency database inspect

AM_CPPFLAGS = -I include

lib_LTLIBRARIES = libpaganini.la
libpaganini_la_SOURCES = 
libpaganini_la_LIBADD = error_msg/liberrormsg.la \
                        util/libutil.la \
                        paging/libpaging.la \
                        row/librow.la \
                        io/libio.la \
                        indexing/libindexing.la \
                        concurrency/libconcurrency.la \
                        inspect/libinspect.la

libpaganini_la_includedir = ${includedir}/paganini
libpaganini_la_include_HEADERS = include/paganini/paganini.h


noinst_PROGRAMS = test insp dbshell \
                  threads \
                  locks \
                  thread_locks \
                  reader_writer 
                  
test_SOURCES = test.cpp
test_LDADD = libpaganini.la

insp_SOURCES = inspector/cmd.cpp \
               inspector/main.cpp \
               inspector/operations.cpp
                    
insp_LDADD = libpaganini.la

dbshell_SOURCES = tools/dbshell/shell.cpp \
                  tools/dbshell/Interpreter.cpp 
                  
dbshell_LDADD = libpaganini.la


threads_SOURCES = tests/ThreadsTest.cpp
threads_LDADD = libpaganini.la

reader_writer_SOURCES = tests/ReaderWriterTest.cpp
reader_writer_LDADD = libpaganini.la

locks_SOURCES = tests/LockingTest.cpp
locks_LDADD = libpaganini.la

thread_locks_SOURCES = tests/LockingThreadTest.cpp
thread_locks_LDADD = libpaganini.la
