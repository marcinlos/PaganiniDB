AM_CPPFLAGS = -I ../include

noinst_LTLIBRARIES = libconcurrency.la
libconcurrency_la_SOURCES = MutexPthread.cpp \
                            SemaphorePthread.cpp \
                            FilePageLocker.cpp \
                            ThreadPageLocker.cpp

libconcurrency_la_includedir = ${includedir}/paganini/concurrency

libconcurrency_la_include_HEADERS = ../include/paganini/concurrency/SystemError.h \
                                    ../include/paganini/concurrency/MutexPthread.h \
                                    ../include/paganini/concurrency/SemaphorePthread.h \
                                    ../include/paganini/concurrency/ReadWriteLock.h \
                                    ../include/paganini/concurrency/PageLock.h \
                                    ../include/paganini/concurrency/GenericPageLocker.h \
                                    ../include/paganini/concurrency/FilePageLocker.h \
                                    ../include/paganini/concurrency/ThreadPageLocker.h
                                    
libconcurrency_la_LDFLAGS = -pthread
