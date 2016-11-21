CC        := clang++
SRCDIR    := src
HEADERDIR := include
BUILDDIR  := build
TARGETDIR := bin
TARGET    := $(TARGETDIR)/irin
BOOSTDIR  := /usr/local/Cellar/boost/1.62.0

SRCEXT    := cc
HEADEREXT := h
SOURCES   := $(shell find $(SRCDIR) -type f -name "*.$(SRCEXT)")
HEADERS   := $(shell find $(HEADERDIR) -type f -name "*.$(HEADEREXT)")
OBJECTS   := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS    := -std=c++14
DEVFLAGS  := $(CFLAGS) -g -Wall -Wextra
PRODFLAGS := $(CFLAGS) -O2
INC       := $(BOOSTDIR)/include
LIBDIR    := lib
LIBS      := -lboost_system-mt -lboost_thread-mt -lboost_filesystem-mt -lboost_log-mt -lboost_log_setup-mt -lboost_program_options-mt

GOOGLETESTDIR := gtest

$(TARGET): $(OBJECTS)
	@echo "Linking..."
	$(CC) $^ -o $(TARGET) -L $(BOOSTDIR)/lib $(LIBS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@echo "Compiling..."
	@mkdir -p $(BUILDDIR)/server
	@mkdir -p $(BUILDDIR)/services
	@mkdir -p $(TARGETDIR)
	$(CC) $(DEVFLAGS) -I $(INC) -I include -DBOOST_LOG_DYN_LINK -c -o $@ $< 

gtestcompile:
	@mkdir -p $(BUILDDIR)/gtest
	$(CC) $(DEVFLAGS) -isystem $(GOOGLETESTDIR)/include -I $(GOOGLETESTDIR) -pthread -c $(GOOGLETESTDIR)/src/gtest-all.cc -o $(BUILDDIR)/gtest/gtest-all.o
	$(CC) $(DEVFLAGS) -isystem $(GOOGLETESTDIR)/include -I $(GOOGLETESTDIR) -pthread -c $(GOOGLETESTDIR)/src/gtest_main.cc -o $(BUILDDIR)/gtest/gtest_main.o

gtestarchive: gtestcompile
	@mkdir -p $(LIBDIR)/gtest
	ar -rv $(LIBDIR)/gtest/gtest.a $(BUILDDIR)/gtest/gtest-all.o
	ar -rv $(LIBDIR)/gtest/gtest_main.a $(BUILDDIR)/gtest/gtest_main.o

testcompile:
	@mkdir -p $(BUILDDIR)/test
	$(CC) $(DEVFLAGS) -I include -I $(INC) -isystem $(GOOGLETESTDIR)/include -pthread -c test/http-message-decoder-test.cc -o $(BUILDDIR)/test/http-message-decoder-test.o 

testlink: $(TARGET) gtestarchive testcompile
	@mkdir -p $(TARGETDIR)/test
	$(CC) $(BUILDDIR)/test/http-message-decoder-test.o -o $(TARGETDIR)/test/http-message-decoder-test $(BUILDDIR)/server/http-message-decoder.o $(LIBDIR)/gtest/gtest.a $(LIBDIR)/gtest/gtest_main.a 

test: testlink
	./bin/test/http-message-decoder-test

clean:
	@echo "Cleaning..."
	rm -r -f $(BUILDDIR) $(TARGETDIR) $(LIBDIR)/gtest

.PHONY: clean
