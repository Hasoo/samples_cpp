# ------------------------------------------------
# Generic Makefile
#
# Author: Hasoo Kim<vaxzeem@i-heart.co.kr>
# Date  : 2010-11-05
#
# Changelog :
#   0.01 - first version
# ------------------------------------------------

CXX := g++
CXXFLAGS += -c -g -Wall -std=c++17
LDFLAGS +=
LIBFLAGS := -L$(POCO_PATH)/lib -lPocoUtil -lPocoFoundation -L$(RABBITMQ_PATH)/lib -lrabbitmq
INCLUDEFLAGS := -I$(BOOST_PATH)/include -I$(POCO_PATH)/include -I$(RABBITMQ_PATH)/include -I../rabbitmq-cpp/include

TARGET := test

HEADERDIR := .
SRCDIR := .
OBJDIR := .
BINDIR := .

HEADERS := $(wildcard $(HEADERDIR)/*.h)
SRCS := $(wildcard $(SRCDIR)/*.cc) $(wildcard ../rabbitmq-cpp/src/*.cc)
OBJS := $(subst $(SRCDIR)/,$(OBJDIR)/,$(SRCS:.cc=.o))

CXXFLAGS += $(INCLUDEFLAGS) -I$(HEADERDIR)
LDFLAGS += $(LIBFLAGS)

.PHONY: all clean distclean

all: $(BINDIR)/$(TARGET)

$(BINDIR)/$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cc
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -fr $(OBJDIR)/*.o $(BINDIR)/$(TARGET)
	rm -fr core

distclean: clean
