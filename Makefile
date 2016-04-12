APPNAME  := xdrawrect

CC       := gcc
SRCDIR   := src
BUILDDIR := build
TARGET   := bin/$(APPNAME)
DESTDIR  := /usr/local/bin

SRCEXT   := c
SOURCES  := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS  := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS   := -std=c11 -Wall -pedantic

LIBS     := -lX11 -lXrandr
INC      := -Iinclude

TAG      := \033[31;1m**\033[0m

$(TARGET): $(OBJECTS)
	@echo "$(TAG) Linking $(TARGET)";
	@echo "$(CC) $^ -o $(TARGET) $(LIBS)"; $(CC) $^ -o $(TARGET) $(LIBS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@) bin
	@echo "$(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

install:
	@echo "$(TAG) Installing $(APPNAME)";
	@echo "cp $(TARGET) $(DESTDIR)/$(APPNAME)"; cp $(TARGET) $(DESTDIR)/$(APPNAME)

uninstall:
	@echo "$(TAG) Uninstalling $(APPNAME)";
	@echo "rm $(DESTDIR)/$(APPNAME)"; rm $(DESTDIR)/$(APPNAME)

clean:
	@echo "$(TAG) Cleaning up";
	@echo "$(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)

# vim:ts=2 sw=2 noet nolist
