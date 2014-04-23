EXE = mog
SRC = sources
PREFIX = /usr
EXTRADIR = ./build/linux

GAMEDIR = $(PREFIX)/games
STARTUP = $(GAMEDIR)/$(EXE)
BINDIR = $(PREFIX)/share/games/$(EXE)
ICNDIR = $(PREFIX)/share/pixmaps
APPDIR = $(PREFIX)/share/applications

DATA = graphics rooms sound leeme.txt liesmich.txt readme.txt MoG-FAQ.txt LICENSE
ICON = mog.png
DESKTOP = mog.desktop

OBJS = \
	$(SRC)/bitmaps.o		$(SRC)/debug-report.o		\
	$(SRC)/demons.o			$(SRC)/drawing.o			\
	$(SRC)/filehandling.o	$(SRC)/gameaux.o			\
	$(SRC)/gamecycle.o		$(SRC)/gameinteractions.o	\
	$(SRC)/gameobjects.o	$(SRC)/gametiles.o			\
	$(SRC)/ingamecycle.o	$(SRC)/keyboard.o			\
	$(SRC)/line_clip.o		$(SRC)/loadpcx.o			\
	$(SRC)/main.o			$(SRC)/passwords.o			\
	$(SRC)/path.o			$(SRC)/debug.o				\
	$(SRC)/render.o			$(SRC)/roomchange.o			\
	$(SRC)/shrines.o		$(SRC)/sound.o

CC = g++
CFLAGS = -g3 -O2 -Wno-write-strings `sdl-config --cflags`
LDFLAGS = -s -lSDL_sound `sdl-config --libs` -lSDL_image -lSDL_mixer -lvorbisfile
RM = rm -f
CP = cp -r
MD = mkdir -p
ECHO = echo
CHMOD = chmod
STRIP = strip

all: $(EXE)

%.o: %.cpp
	$(CC) -c $(CFLAGS) $< -o $@

$(EXE): $(OBJS)
	$(CC) $^ $(LDFLAGS) -o $@
	@$(STRIP) $@
	@$(ECHO) " o If there are no errors, the game compiled succesfully"

clean:
	@$(ECHO) " o Cleaning up source directory"
	@$(RM) $(SRC)/*.o $(SRC)/*.bak core $(EXE)

install: all
	@$(ECHO) " o Creating install directory $(BINDIR)"
	@$(MD) "$(BINDIR)"
	@$(ECHO) " o Installing game and data to $(BINDIR)"
	@$(CP) "$(EXE)" $(DATA) "$(BINDIR)"
	@$(ECHO) " o Creating startup script $(STARTUP)"
	@$(MD) "$(GAMEDIR)"
	@$(ECHO) "#!/bin/sh" >"$(STARTUP)"
	@$(ECHO) "cd \"$(BINDIR)\" && ./$(EXE); cd -" >>"$(STARTUP)"
	@$(CHMOD) 755 "$(STARTUP)"
	@$(ECHO) " o Creating application menu entry"
	@$(MD) "$(ICNDIR)"
	@$(CP) "$(EXTRADIR)/$(ICON)" "$(ICNDIR)"
	@$(MD) "$(APPDIR)"
	@$(CP) "$(EXTRADIR)/$(DESKTOP)" "$(APPDIR)"
	@$(ECHO) ""

uninstall:
	@$(ECHO) " o Removing game and data from $(BINDIR)"
	@$(RM) -r "$(BINDIR)"
	@$(ECHO) " o Removing startup script $(STARTUP)"
	@$(RM) "$(STARTUP)"
	@$(ECHO) " o Removing application menu entry"
	@$(RM) "$(ICNDIR)/$(ICON)" "$(APPDIR)/$(DESKTOP)"
	@$(ECHO) ""

