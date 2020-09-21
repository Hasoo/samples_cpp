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
INCLUDEFLAGS := -I$(BOOST_PATH)/include -I$(POCO_PATH)/include -I$(RABBITMQ_PATH)/include

TARGET := test

HEADERDIR := .
SRCDIR := .
OBJDIR := .
BINDIR := .

HEADERS := $(wildcard $(HEADERDIR)/*.h)
SRCS := $(wildcard $(SRCDIR)/*.cpp)
OBJS := $(subst $(SRCDIR)/,$(OBJDIR)/,$(SRCS:.cpp=.o))

CXXFLAGS += $(INCLUDEFLAGS) -I$(HEADERDIR)
LDFLAGS += $(LIBFLAGS)

.PHONY: all clean distclean

all: $(BINDIR)/$(TARGET)

$(BINDIR)/$(TARGET): $(OBJS)
	$(CXX) $? -o $@ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -fr $(OBJDIR)/*.o $(BINDIR)/$(TARGET)
	rm -fr core

distclean: clean
