##############################################################################
#
# Makefile for PAKMAN (Package Manager)
#
# Version: 1.0
#
# Author: Marcelo Gornstein
#
# NOTES:
#		+ ITS NOW BUILDING WITH STATIC LIBRARIES.
#
##############################################################################
CC			=	gcc
CFLAGS_FREEBSD	+=	-g -Wall -pedantic -ansi -std=c99 -Wmissing-prototypes -pipe -O2
CFLAGS_LINUX	+=	-g -Wall -pedantic -Wmissing-prototypes -pipe -O2  -DLINUX -D__USE_BSD
CFLAGS	= $(CFLAGS_FREEBSD)
#CFLAGS	= $(CFLAGS_LINUX)
CFLAGS	=	-g -Wall -pedantic -ansi -std=c99 -Wstrict-prototypes -Wmissing-prototypes -pipe -O2 -I/netlabs/include -I/netlabs/include/gdbm
#LDFLAGS	+=	-lgdbm -lmd
LDFLAGS_LINUX		+=	-lgdbm -lcrypto
LDFLAGS_FREEBSD	+=	/netlabs/lib/libgdbm.a /usr/lib/libmd.a
LDFLAGS	=	$(LDFLAGS_FREEBSD)
#LDFLAGS	=	$(LDFLAGS_LINUX)
#LIBS		=	
OBJDIR	=	obj
BUILDDIR	=	build
SRCDIR	=	src
NAME		=	pakman
SRCS		=	\
	$(SRCDIR)/cmd_create.c \
	$(SRCDIR)/cmd_createdb.c \
	$(SRCDIR)/cmd_deinstall.c \
	$(SRCDIR)/cmd_install.c \
	$(SRCDIR)/cmd_help.c \
	$(SRCDIR)/cmd_query.c \
	$(SRCDIR)/cmd_search.c \
	$(SRCDIR)/cmd_show.c \
	$(SRCDIR)/cmd_verify.c \
	$(SRCDIR)/cmd_na.c \
	$(SRCDIR)/cmd_update.c \
	$(SRCDIR)/error.c \
	$(SRCDIR)/cmd.c \
	$(SRCDIR)/file.c \
	$(SRCDIR)/db.c \
	$(SRCDIR)/utils.c \
	$(SRCDIR)/pkg.c \
	$(SRCDIR)/shafile.c \
	$(SRCDIR)/main.c
OBJS		=	$(SRCS:.c=.o)

##############################################################################

all:	$(OBJS)
	$(CC) $(CFLAGS) `pwd`/*.o  $(LIBS) -o `pwd`/$(BUILDDIR)/${NAME} $(LDFLAGS)
	strip `pwd`/$(BUILDDIR)/${NAME}

##############################################################################

.c.o: 
	$(CC) $(CFLAGS) $< -c -o `basename $@`

##############################################################################

install: all
	install -S -o root -g wheel -m 755 `pwd`/build/$(NAME) /sbin/$(NAME)

##############################################################################

clean:
	rm -f $(OBJDIR)/*
	rm -f $(BUILDDIR)/*
##############################################################################

