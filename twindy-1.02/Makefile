#!/usr/bin/make -f
# Makefile for mod-twindy #
# ----------------------- #
# Created by falkTX
#

CC  ?= gcc
CXX ?= g++

# --------------------------------------------------------------
# Set build and link flags

BASE_FLAGS = -Wall -pipe -MD -MP -DLINUX -DPHIL_LIGHT -Isrc -Isrc/ExtraComponents -Isrc/PreferencesPages
BASE_OPTS  = -O2 -mtune=generic -msse -msse2 -fdata-sections -ffunction-sections -fPIC -DPIC
LINK_OPTS  = -fdata-sections -ffunction-sections -Wl,--gc-sections -Wl,-O1 -Wl,--as-needed -Wl,--strip-all

ifeq ($(DEBUG),true)
BASE_FLAGS += -DDEBUG -O0 -g -DTWINDY_DEBUG
LINK_OPTS   =
else
BASE_FLAGS += -DNDEBUG $(BASE_OPTS) -fvisibility=hidden
CXXFLAGS   += -fvisibility-inlines-hidden
endif

BUILD_C_FLAGS   = $(BASE_FLAGS) -std=c99 -std=gnu99 $(CFLAGS)
BUILD_CXX_FLAGS = $(BASE_FLAGS) -std=c++0x -std=gnu++0x $(CXXFLAGS) $(CPPFLAGS)
LINK_FLAGS      = $(LINK_OPTS) -Wl,--no-undefined $(LDFLAGS)

# --------------------------------------------------------------
# Basic setup

ifeq ($(DEBUG),true)
BINDIR = $(CURDIR)
OBJDIR = $(CURDIR)/build/Debug
else
BINDIR = $(CURDIR)
OBJDIR = $(CURDIR)/build/Release
endif

ifeq ($(DEBUG),true)
TARGET = $(BINDIR)/twindy-dbg
else
TARGET = $(BINDIR)/twindy
endif

# --------------------------------------------------------------
# Common

all: $(TARGET)

$(OBJDIR)/juce_amalgamated.cpp.o: src/juce_amalgamated.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling $<"
	$(CXX) $< $(BUILD_CXX_FLAGS) $(shell pkg-config --cflags alsa freetype2 x11 xext xinerama) -w -c -o $@

$(OBJDIR)/ExtraComponents/%.cpp.o: src/ExtraComponents/%.cpp
	-@mkdir -p $(OBJDIR)/ExtraComponents
	@echo "Compiling $<"
	$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@

$(OBJDIR)/PreferencesPages/%.cpp.o: src/PreferencesPages/%.cpp
	-@mkdir -p $(OBJDIR)/PreferencesPages
	@echo "Compiling $<"
	$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@

$(OBJDIR)/%.cpp.o: src/%.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling $<"
	$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@

$(OBJDIR)/%.c.o: src/%.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling $<"
	$(CC) $< $(BUILD_C_FLAGS) -c -o $@

discovery: $(OBJDIR)/juce_amalgamated.cpp.o $(OBJDIR)/AudioDiscovery.cpp.o
	-@mkdir -p $(BINDIR)
	@echo "Linking discovery"
	$(CXX) $^ $(LINK_FLAGS) $(shell pkg-config --libs alsa) -lpthread -ldl -o $@

clean:
	rm -f $(OBJS)
	rm -f $(BINDIR)/twindy
	rm -f $(BINDIR)/twindy-dbg

# --------------------------------------------------------------
# twindy

OBJS = $(OBJDIR)/juce_amalgamated.cpp.o \
	$(OBJDIR)/TwindyApp.cpp.o \
	$(OBJDIR)/TwindyErrorDisplay.cpp.o \
	$(OBJDIR)/TwindyLAF.cpp.o \
	$(OBJDIR)/TwindyPreferences.cpp.o \
	$(OBJDIR)/TwindyRootWindow.cpp.o \
	$(OBJDIR)/TwindyRootWindowXMethods.cpp.o \
	$(OBJDIR)/TwindyTabs.cpp.o \
	$(OBJDIR)/TwindyUpperPanel.cpp.o \
	$(OBJDIR)/TwindyWindow.cpp.o \
	$(OBJDIR)/ExtraComponents/DrawableTextButton.cpp.o \
	$(OBJDIR)/ExtraComponents/TracktionScheme.cpp.o \
	$(OBJDIR)/ExtraComponents/TwindyTabbedComponent.cpp.o \
	$(OBJDIR)/ExtraComponents/TwindyToggleButton.cpp.o \
	$(OBJDIR)/PreferencesPages/AboutPage.cpp.o \
	$(OBJDIR)/PreferencesPages/AudioPreferences.cpp.o \
	$(OBJDIR)/PreferencesPages/MidiPreferences.cpp.o
# 	$(OBJDIR)/PreferencesPages/ButtonPreferences.cpp.o \
# 	$(OBJDIR)/PreferencesPages/MiscPreferences.cpp.o \
# 	$(OBJDIR)/PreferencesPages/StartupPreferences.cpp.o \
# 	$(OBJDIR)/PreferencesPages/TracktionSchemeEditor.cpp.o

$(BINDIR)/twindy: $(OBJS)
	-@mkdir -p $(BINDIR)
	@echo "Linking twindy"
	$(CXX) $^ $(LINK_FLAGS) $(shell pkg-config --libs alsa freetype2 x11 xext xinerama) -lpthread -ldl -o $@

$(BINDIR)/twindy-dbg: $(OBJS)
	-@mkdir -p $(BINDIR)
	@echo "Linking twindy-dbg"
	$(CXX) $^ $(LINK_FLAGS) $(shell pkg-config --libs alsa freetype2 x11 xext xinerama) -lpthread -ldl -o $@

# --------------------------------------------------------------

-include $(OBJS:%.o=%.d)
