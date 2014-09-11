CPP = g++
WARNING = -Wall
DEBUGON = -g
CFLAGS = -c $(DEBUGON) -o2
LDFLAGS = -lpthread -lboost_system -lboost_thread -lssl -lcrypto -lcql -lboost_program_options -ltcmalloc -llog4cpp
LIBDIR = -L/usr/local/lib

SRCDIR = src
OBJDIR = obj
BINDIR = bin

SRCS := $(shell find $(SRCDIR) -name '*.cpp')
SRCDIRS = $(shell find $(SRCDIR) -type d | sed 's/$(SRCDIR)/./g' )
OBJS := $(patsubst $(SRCDIRS)/%.cpp,$(OBJDIR)/%.o,$(SRCS))

all: $(BINDIR)/pweb_home_agent

$(BINDIR)/pweb_home_agent: $(OBJS)
	$(CPP) $(DEBUGON) $(OBJS) $(LDFLAGS) $(LIBDIR) -o $@

$(OBJDIR)/%.o: $(SRCDIRS)/%.cpp
	$(CPP) $(CFLAGS) $< -o $@

clean:
			rm -rf $(OBJDIR)/*
			rm -rf $(BINDIR)/*


