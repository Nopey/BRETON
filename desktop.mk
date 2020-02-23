# Breton Desktop Makefile
#
# Magnus Larsen
# Feb 2020
CXXFLAGS = -g -Wall
LDFLAGS = -g -lSDL -lSDL_image

SHORTNAME=breton
SRCDIR=src
OBJDIR=obj
SOURCE_NAMES=main.cc load_screen.cc clickzone.cc
OBJECTS=$(SOURCE_NAMES:%.cc=$(OBJDIR)/%.o)
# SOURCES=$(SOURCE_NAMES:%=$(SRCDIR)/%)

$(SHORTNAME): $(OBJECTS)
	echo Linking $@..
	$(CXX) $^ -o $@ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cc
	echo CXX $@..
	@mkdir -p `dirname $@`
	$(CXX) -c $^ -o $@ $(CXXFLAGS)

clean:
	rm -f $(OBJDIR) $(SHORTNAME)
