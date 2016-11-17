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
DEVFLAGS  := $(CFLAGS) -g
PRODFLAGS := $(CFLAGS) -O2
INC       := $(BOOSTDIR)/include
LIBS      := -lboost_system-mt -lboost_thread-mt -lboost_filesystem-mt -lboost_log-mt -lboost_log_setup-mt -lboost_program_options-mt

$(TARGET): $(OBJECTS)
	@echo "Linking..."
	$(CC) $^ -o $(TARGET) -L $(BOOSTDIR)/lib $(LIBS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@echo "Compiling..."
	@mkdir -p $(BUILDDIR)/server
	@mkdir -p $(TARGETDIR)
	$(CC) $(DEVFLAGS) -I $(INC) -I include -DBOOST_LOG_DYN_LINK -c -o $@ $< 

clean:
	@echo "Cleaning..."
	rm -r -f $(BUILDDIR) $(TARGETDIR)

.PHONY: clean
