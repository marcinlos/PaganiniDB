AM_CPPFLAGS = -I ../include

noinst_LTLIBRARIES = libnetworking.la
libnetworking_la_SOURCES = unix/Socket.cpp

libnetworking_la_includedir = ${includedir}/paganini/networking
libnetworking_la_include_unixdir = ${includedir}/paganini/networking/unix

libnetworking_la_include_unix_HEADERS = ../include/paganini/networking/unix/Socket.h \
                                        ../include/paganini/networking/unix/Acceptor.h


libnetworking_la_include_HEADERS = ../include/paganini/networking/NetworkingError.h \
                                   ../include/paganini/networking/address.h \
                                   ../include/paganini/networking/buffers.h
