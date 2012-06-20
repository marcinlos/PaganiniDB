AM_CPPFLAGS = -I ../include

noinst_LTLIBRARIES = libconcurrency.la
libconcurrency_la_SOURCES = pthread/Mutex.cpp \
                            pthread/Semaphore.cpp \
                            pthread/Thread.cpp \
                            FilePageLocker.cpp \
                            ThreadPageLocker.cpp

libconcurrency_la_includedir = ${includedir}/paganini/concurrency
libconcurrency_la_pthread_includedir = ${includedir}/paganini/concurrency/pthread

libconcurrency_la_pthread_include_HEADERS = ../include/paganini/concurrency/pthread/Mutex.h \
                                            ../include/paganini/concurrency/pthread/Semaphore.h \
                                            ../include/paganini/concurrency/pthread/Thread.h 

libconcurrency_la_include_HEADERS = ../include/paganini/concurrency/SystemError.h \
                                    ../include/paganini/concurrency/ReadWriteLock.h \
                                    ../include/paganini/concurrency/PageLock.h \
                                    ../include/paganini/concurrency/GenericPageLocker.h \
                                    ../include/paganini/concurrency/FilePageLocker.h \
                                    ../include/paganini/concurrency/ThreadPageLocker.h
                                    
libconcurrency_la_LDFLAGS = -pthread
