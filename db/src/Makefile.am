SUBDIRS = error_msg paging

AM_CPPFLAGS = -iquote include

lib_LTLIBRARIES = libpaganini.la
libpaganini_la_SOURCES = 
libpaganini_la_LIBADD = error_msg/liberrormsg.la paging/libpaging.la

libpaganini_la_includedir = ${includedir}/paganini
libpaganini_la_include_HEADERS = include/paganini.h

noinst_PROGRAMS = test insp
test_SOURCES = test.c
test_LDADD = libpaganini.la

insp_SOURCES = inspector/cmd.c \
               inspector/main.c \
               inspector/operations.c 
                    
insp_LDADD = libpaganini.la

