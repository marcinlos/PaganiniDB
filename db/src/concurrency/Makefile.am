AM_CPPFLAGS = -I ../include

noinst_LTLIBRARIES = libconcurrency.la
libconcurrency_la_SOURCES = MutexPthread.cpp \
                            FileLocker.cpp

libconcurrency_la_includedir = ${includedir}/paganini/concurrency

libconcurrency_la_include_HEADERS = ../include/paganini/concurrency/SystemError.h \
                                    ../include/paganini/concurrency/MutexPthread.h \
                                    ../include/paganini/concurrency/FileLocker.h
