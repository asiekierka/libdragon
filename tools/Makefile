hN64_GCCPREFIX ?= $(N64_INST)
INSTALLDIR ?= $(N64_INST)
CFLAGS   += -std=gnu11 -O2 -Wall -Werror -Wno-unused-result -Wno-error=unknown-pragmas -Wno-error=sign-compare -I../include -MMD
CXXFLAGS += -std=c++11 -O2 -Wall -Werror -Wno-unused-result -Wno-error=unknown-pragmas -Wno-error=sign-compare -MMD
LDFLAGS   = 

all:

%.o: %.c
	@echo "    [CC] $@"
	$(CC) $(CFLAGS) -c -o $@ $<
%.o: %.cpp
	@echo "    [CXX] $@"
	$(CXX) $(CXXFLAGS) -c -o $@ $<
%.a:
	@echo "    [AR] $@"
	rm -f $@
	$(AR) rcs $@ $^

common-clean:
	rm -f common/*.o common/*.a common/*.d
common/assetcomp.a: common/assetcomp.o common/lz4_compress.o \
				    common/aplib_compress.o common/shrinkler_compress.o

-include $(wildcard common/*.d)

mkasset_OBJS = mkasset/mkasset.o common/assetcomp.a
mksprite_OBJS = mksprite/mksprite.o common/assetcomp.a
audioconv64_OBJS = audioconv64/audioconv64.o
mkdfs_OBJS = mkdfs/mkdfs.o
dumpdfs_OBJS = dumpdfs/dumpdfs.o
n64tool_OBJS = n64tool.o
n64sym_OBJS = n64sym.o
chksum64_OBJS = chksum64.o
ed64romconfig_OBJS = ed64romconfig.o

TOOLS = n64tool n64sym chksum64 ed64romconfig audioconv64 mkdfs dumpdfs mkasset mksprite

# Define a variable that has value ".exe" on Windows and "" on other platforms
EXE = $(if $(findstring Windows,$(OS)),.exe,)

define TOOL_template
.PHONY: $(1)-install $(1)-clean
$(1)_DIR ?= $$(dir $$(firstword $$($(1)_OBJS)))
$(1)_BIN ?= $$($(1)_DIR)$(1)$(EXE)
ifneq ($$($(1)_DIR),./)
.PHONY: $(1)
$(1): $$($(1)_BIN)
endif
$$($(1)_BIN): $$($(1)_OBJS)
	@echo "    [TOOL] $(1)"
	$(CXX) $(LDFLAGS) -o $$@ $$^
$(1)-install: $(1)
	mkdir -p $(INSTALLDIR)/bin
	install -m 0755 $$($(1)_BIN) $(INSTALLDIR)/bin
$(1)-clean:
	rm -f $$($(1)_BIN) $$($(1)_DIR)/*.o $$($(1)_DIR)/*.d
-include $$(wildcard $$($(1)_DIR)/*.d)
endef

$(foreach tool,$(TOOLS),$(eval $(call TOOL_template,$(tool))))
all: $(TOOLS)
install: $(foreach tool,$(TOOLS),$(tool)-install)
clean: $(foreach tool,$(TOOLS),$(tool)-clean) common-clean
.PHONY: all install clean

ifneq ($(V),1)
.SILENT:
endif
