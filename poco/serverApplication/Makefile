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
CXXFLAGS += -g -Wall -std=c++17
LDFLAGS +=
LIBFLAGS := -L$(GTEST_PATH)/lib -lgtest -L$(POCO_PATH)/lib -lPocoUtil -lPocoFoundation -pthread -lstdc++fs
INCLUDEFLAGS := -I$(GTEST_PATH)/include -I$(POCO_PATH)/include

TARGET := test

HEADERDIR := .
SRCDIR := .
OBJDIR := .
BINDIR := .

HEADERS := $(wildcard $(HEADERDIR)/*.h)
SRCS := $(wildcard $(SRCDIR)/*.cpp)
#SRCS := main.cpp
OBJS := $(subst $(SRCDIR)/,$(OBJDIR)/,$(SRCS:.cpp=.o))

CXXFLAGS += $(INCLUDEFLAGS) -I$(HEADERDIR)
LDFLAGS += $(LIBFLAGS)

.PHONY: all clean distclean

all: $(BINDIR)/$(TARGET)

$(BINDIR)/$(TARGET): $(OBJS)
#	$(CXX) -o $@ $? $(LDFLAGS)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $< 

clean:
	rm -fr $(OBJS) $(BINDIR)/$(TARGET)
	rm -f ./log/*

distclean: clean
