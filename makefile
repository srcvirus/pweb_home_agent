CPP = g++
WARNING = -Wall
DEBUGON = -g
CFLAGS = -c $(DEBUGON)
LDFLAGS = -lpthread -lboost_system -lboost_thread -lssl -lcrypto -lleveldb -lcql

SRCDIR = src
OBJDIR = obj
BINDIR = bin

SRCS := $(shell find $(SRCDIR) -name '*.cpp')
SRCDIRS = $(shell find $(SRCDIR) -type d | sed 's/$(SRCDIR)/./g' )
OBJS := $(patsubst $(SRCDIRS)/%.cpp,$(OBJDIR)/%.o,$(SRCS))

all: $(BINDIR)/pweb_home_agent_test

$(BINDIR)/pweb_home_agent_test: $(OBJS)
	$(CPP) $(DEBUGON) $(OBJS) $(LDFLAGS) -o $@

$(OBJDIR)/%.o: $(SRCDIRS)/%.cpp
	$(CPP) $(CFLAGS) $< -o $@

clean:
			rm -rf $(OBJDIR)/*
			rm -rf $(BINDIR)/*


