# Breton Desktop Makefile
#
# Magnus Larsen
# Feb 2020
CXXFLAGS = -g -Wall -std=c++03
LDFLAGS = -g -lSDL -lSDL_image -lSDL_ttf

SHORTNAME=game/breton
SRCDIR=src
OBJDIR=obj
include sources.mk
OBJECTS=$(SOURCE_NAMES:%=$(OBJDIR)/%.o)


$(SHORTNAME): $(OBJECTS)
	echo Linking $@..
	$(CXX) $^ -o $@ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cc
	echo CXX $@..
	@mkdir -p `dirname $@`
	$(CXX) -c $^ -o $@ $(CXXFLAGS)

clean:
	rm -rf $(OBJDIR) $(SHORTNAME)
