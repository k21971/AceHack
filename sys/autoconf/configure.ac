dnl	ACEHACK autoconf template
dnl     Modified 28 Dec 2011 by Alex Smith
AC_INIT(ACEHACK, 3.6.0, ais523@bham.ac.uk)
dnl Avoid AC_REVISION() because we check configure into CVS
dnl and it will cause confusing revision numbers to be included
dnl in configure if developers forget to check in configure.ac
dnl before running bootstrap.sh and checking in configure.
# $Revision: 1.10 $
AC_CONFIG_SRCDIR(Files)
AC_SUBST(SRCDIR,[../$srcdir/src])
AC_CONFIG_AUX_DIR(sys/autoconf)

dnl  ----- Precious variables
AC_ARG_VAR(CC, [C compiler])
AC_ARG_VAR(CHGRP, [sets GAMEUID (set this to true as a no op)])
AC_ARG_VAR(CHOWN, [sets GAMEGRP (set this to true as a no op)])
AC_ARG_VAR(CHMOD, [sets file modes (set this to true as a no op)])
AC_ARG_VAR(ZLIB_CFLAGS, [Compiler flags to find zlib header files])
AC_ARG_VAR(ZLIB_LIBS, [Linker flags to find zlib library files])
AC_ARG_VAR(MPLOCKLOC, [Directory to place multiplayer communication files in])

AC_PROG_LN_S
AC_PROG_AWK

dnl  ----- Check options
dnl  ----- Window systems

# NETHACK_WIN_ENABLE(proxy, auto)
nethack_default_win=""
# Note: Order is important: The last listed enabled port will be the default.
NETHACK_WIN_ENABLE(tty, auto)
NETHACK_WIN_ENABLE(x11, no)
NETHACK_WIN_ENABLE(sdl, no)
NETHACK_WIN_ENABLE(gl, no)
NETHACK_WIN_ENABLE(mswin, no, [use Microsoft Window's native graphics])

dnl  ----- Figure out a C compiler to use; set @CC@
AC_PROG_CC
AC_SUBST(LFLAGS)

dnl  ----- Check OS
AC_CANONICAL_HOST

dnl  ----- Check location to store multiplayer lockfiles
# We need to find a multiplayer lockfile location that we can write to
# without constantly grinding the hard disk. We want the first of
# /var/lock, /var/run, /dev/shm, and /tmp that exists and that we can
# write to without special permissions, and also that is mounted on a
# partition of type shmfs or tmpfs. If none of them work, we fall back
# to FILE_AREA_LEVL, which should be PREFIX/acehackdir.
#
# We also want the user to be able to override the location. We do
# this using the MPLOCKLOC shell variable.
AC_MSG_CHECKING([the appropriate location for multiplayer lockfiles])
# According to autoconf docs, this is the only portable way to do this
# sort of thing. (And even then, it fails on directory names that
# contain spaces/tabs/newlines, on some shells, but I wouldn't expect
# that to work anyway.)
MPLOCKLOC=${MPLOCKLOC=`(find /opt/nethack/chroot -maxdepth 0 -and -writable -printf '%p/acehack\n'; echo "\${PREFIX}") | head -n 1`}
AC_MSG_RESULT(${MPLOCKLOC})
AC_DEFINE_UNQUOTED(MPLOCKLOC, "${MPLOCKLOC}", [Define to an appropriate directory for multiplayer lockfiles.])
AC_SUBST(MPLOCKLOC)

dnl  ----- Early checks for libraries, as needed

dnl PKG_PROG_PKG_CONFIG([])
if test "$enable_x11_graphics" != "no"; then
    # We have to check for libXaw before we define SYSV,
    # since the X11 include files change their behaviour
    # depending on whether the compiler defines it or not.
    NETHACK_CHECK_LIBXAW([LIBXAW], have_libXaw=yes, have_libXaw=no)
fi

dnl  ------ Main OS types
case $host_os in
     mingw32 )		AC_DEFINE(WIN32,[],[An MS-Windows system])
			AC_SUBST(HOST_MSWIN,[])
			AC_SUBST(HOST_UNIX,[#])
			SYSTEM="Syswinnt"
			SYSSRC="\${SYSMSWINSRC}"
			SYSOBJ="\${SYSMSWINOBJ}"
			;;
     *bsd* )		AC_DEFINE(BSD,[],[A BSD like system])
			AC_SUBST(HOST_MSWIN,[#])
			AC_SUBST(HOST_UNIX,[])
			SYSTEM="Sysunix"
			SYSSRC="\${SYSUNIXSRC}"
			SYSOBJ="\${SYSUNIXOBJ}"
			;;
     * )		AC_DEFINE(SYSV,[],[A System V like system])
			AC_SUBST(HOST_MSWIN,[#])
			AC_SUBST(HOST_UNIX,[])
			SYSTEM="Sysunix"
			SYSSRC="\${SYSUNIXSRC}"
			SYSOBJ="\${SYSUNIXOBJ}"
			;;
esac
AC_SUBST(SYSTEM)
AC_SUBST(SYSSRC)
AC_SUBST(SYSOBJ)

if test $host_os = mingw32; then
    CFLAGS="$CFLAGS -mno-cygwin -isystem /usr/include/mingw"
    LFLAGS="$LDFLAGS -mno-cygwin -L/usr/lib/mingw"

    AC_MSG_CHECKING([for how to build NetHack key dynamic libraries])
    if test "$GCC" = yes; then
	AC_MSG_RESULT([gcc])
	AC_SUBST(NHKEYDLL_GCC,[])
    else
	AC_MSG_RESULT([unknown])
	if test "$enable_tty_graphics" = yes; then
	    AC_MSG_ERROR([Don't know how to build NetHack key DLLs with $CC])
	else
	    AC_MSG_WARN([Can't enable tty graphics (key dynamic libraries)])
	    NETHACK_AUTO_WIN_DISABLE(tty)
	fi
    fi
fi

dnl ------ Common subtypes
case $host_os in
     *cygwin* )		AC_DEFINE(CYGWIN32,[],[Unix on win32]);;
     *linux* )		AC_DEFINE(LINUX,[],[GNU Linux]);;
esac


dnl  ----- Check for programs
AC_PROG_LEX
AC_PROG_YACC
AC_CHECK_TOOLS(CHOWN,[chown true])
AC_CHECK_TOOLS(CHGRP,[chgrp true])
AC_CHECK_TOOLS(CHMOD,[chmod true])
AC_CHECK_TOOL(WINDRES,windres,no)
NETHACK_PROG_COMPRESS

dnl  ----- Check for libraries
AC_CHECK_FUNCS(flock)

dnl NETHACK_CHECK_LIBPNG(NETHACK_LIBPNG, have_libpng=yes, have_libpng=no)

dnl  ----- Select window systems

WINSRC=
WINOBJ=
WINLIB=
CNF_SHARE_DATND=

OPTIONS_FILE=
OPTIONS_FILES=

WINTTYLIB=
if test "$enable_tty_graphics" != "no"; then
    if test $host_os = mingw32; then
	AC_SUBST(EXTRA_UNSHARE,["nhdefkey.dll nh340key.dll nhraykey.dll"])
	CFLAGS="$CFLAGS -DWIN32CON"
	WINTTYLIB=""
	WINSRC="${WINSRC} \$(WINTTYSRC)"
	WINOBJ="${WINOBJ} \$(WINTTYOBJ)"
	WINLIB="${WINLIB} \$(WINTTYLIB)"
	AC_DEFINE(TTY_GRAPHICS, [], [Console output])
    else
	# For TTY, determine the termlib
	# Check for termlib before termcap because Solaris termcap needs libucb.
	TERMLIB_VARIANTS="ncurses curses termlib termcap terminfo"
	for termlib in ${TERMLIB_VARIANTS}; do
	    AC_CHECK_LIB(${termlib}, tgetent,
	      [WINTTYLIB="-l${termlib}"
	      WINSRC="${WINSRC} \$(WINTTYSRC)"
	      WINOBJ="${WINOBJ} \$(WINTTYOBJ)"
	      WINLIB="${WINLIB} \$(WINTTYLIB)"
	      AC_DEFINE(TTY_GRAPHICS, [], [Console output])
	      break])
	done
	if test -z "$termlib"; then
	    if test "$enable_tty_graphics" = yes; then
		AC_MSG_ERROR([Can't find tgetent (needed for tty graphics)])
	    else
		AC_MSG_WARN([Can't enable tty graphics (no tgetent)])
		NETHACK_AUTO_WIN_DISABLE(tty)
	    fi
	fi
	dnl  ----- Check for tparm in curses
	AC_CHECK_HEADERS(curses.h termcap.h)
	AC_CHECK_LIB(curses, tparm, AC_DEFINE(HAVE_TPARM, 1, [Define this if you have the tparm function in an included lib.]),)

    fi

    OPTIONS_FILE="options"
    OPTIONS_FILES="options"
fi
if test "$enable_tty_graphics" = "no" -a $host_os = mingw32; then
    LFLAGS="$LFLAGS -mwindows"
fi
AC_SUBST(WINTTYLIB)

dnl if test "$enable_proxy_graphics" != "no"; then
dnl     if test "$have_libpng" = "no"; then
dnl 	if test "$enable_proxy_graphics" = "yes"; then
dnl 	    AC_MSG_ERROR([Need libpng for proxy graphics])
dnl 	else
dnl 	    AC_MSG_WARN([Can't enable proxy graphics (no useable libpng)])
dnl 	fi
dnl     else
dnl 	if test "$enable_proxy_graphics" = "yes"; then
dnl 	    PKG_CHECK_MODULES(NETHACK_PROXY, nhproxy >= 0.9.0)
dnl 	else
dnl 	    PKG_CHECK_MODULES(NETHACK_PROXY, nhproxy >= 0.9.0, :, :)
dnl 	fi
dnl 	if test -n "$NETHACK_PROXY_LIBS"; then
dnl 	    CFLAGS="${CFLAGS} ${NETHACK_LIBPNG_CFLAGS}"
dnl 	    LFLAGS="${LFLAGS} ${NETHACK_LIBPNG_LFLAGS}"
dnl 	    WINSRC="${WINSRC} \$(WINPROXYSRC)"
dnl 	    WINOBJ="${WINOBJ} \$(WINPROXYOBJ)"
dnl 	    WINLIB="${WINLIB} ${NETHACK_PROXY_LIBS}"
dnl 	    CNF_SHARE_DATND="${CNF_SHARE_DATND} \$(CNF_SHARE_PROXYND)"
dnl 	    AC_DEFINE(PROXY_GRAPHICS, [], [proxy graphics])
dnl 	fi
dnl     fi
dnl fi
dnl AC_SUBST(NETHACK_PROXY_CFLAGS)

if test "$enable_x11_graphics" != "no"; then
    if test $have_libXaw != yes; then
      AC_MSG_ERROR([Need X Athena Widgets Library for X11 graphics])
    fi
    CFLAGS="${CFLAGS} ${LIBXAW_CFLAGS}"
    WINSRC="${WINSRC} \$(WINX11SRC)"
    WINOBJ="${WINOBJ} \$(WINX11OBJ)"
    WINLIB="${WINLIB} ${LIBXAW_LIBS}"
    CNF_SHARE_DATND="${CNF_SHARE_DATND} \$(CNF_SHARE_X11ND)"
    AC_DEFINE(X11_GRAPHICS, [], [X11 graphics])

    OPTIONS_FILE="options"
    OPTIONS_FILES="options"
fi

if test "$enable_sdl_graphics" != "no"; then
    CFLAGS="${CFLAGS} \${SDLGL_CFLAGS}"
    WINSRC="${WINSRC} \$(WINGLSRC)"
    WINOBJ="${WINOBJ} \$(WINGLOBJ)"
    WINLIB="${WINLIB} \${WINSDLLIB}"
    CNF_SHARE_DATND="${CNF_SHARE_DATND} \$(CNF_SHARE_GLND)"
    AC_DEFINE(SDL_GRAPHICS, [], [SDL graphics with software renderer])

    OPTIONS_FILE="options"
    OPTIONS_FILES="options"
fi
if test "$enable_gl_graphics" != "no"; then
    CFLAGS="${CFLAGS} \${SDLGL_CFLAGS}"
    WINSRC="${WINSRC} \$(WINGLSRC)"
    WINOBJ="${WINOBJ} \$(WINGLOBJ)"
    WINLIB="${WINLIB} \${WINGLLIB}"
    CNF_SHARE_DATND="${CNF_SHARE_DATND} \$(CNF_SHARE_GLND)"
    AC_DEFINE(GL_GRAPHICS, [], [SDL graphics with hardware renderer])

    OPTIONS_FILE="options"
    OPTIONS_FILES="options"
fi

if test "$enable_mswin_graphics" != "no"; then
    AC_MINGW32

    if test "$WINDRES" = "no"; then
	AC_MSG_ERROR([Need windres for Microsoft Window's native graphics])
    fi
    WINSRC="${WINSRC} \$(WINMSSRC)"
    WINOBJ="${WINOBJ} \$(WINMSOBJ)"
    WINLIB="${WINLIB} \$(WINMSLIB)"
    CNF_SHARE_DATND="${CNF_SHARE_DATND} \$(CNF_SHARE_MSND)"
    AC_DEFINE(MSWIN_GRAPHICS, [], [Microsoft Window's native graphics])

    OPTIONS_FILE="guioptions"
    OPTIONS_FILES="$OPTIONS_FILES guioptions"
fi

AC_SUBST(WINSRC)
AC_SUBST(WINOBJ)
AC_SUBST(WINLIB)
AC_SUBST(CNF_SHARE_DATND)
AC_SUBST(OPTIONS_FILE)
AC_SUBST(OPTIONS_FILES)

AC_MSG_CHECKING(for valid window system configuration)
if test -n "$nethack_wins"; then
    AC_MSG_RESULT([yes])
    for win in $nethack_wins; do
	AC_DEFINE_UNQUOTED(DEFAULT_WINDOW_SYS,["$win"],
	  [The default windowing system])
	break
    done
else
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([need to enable at least one window system])
fi


dnl  ----- Check for header files
AC_HEADER_STDC
# Due to what may be a bug in autoconf, we have to call AC_CHECK_HEADERS()
# to get the various HAVE_xxx_H preprocessor variables detected by
# AC_HEADER_STDC active. These are needed for AC_CHECK_DECLS(), below.
AC_CHECK_HEADERS([string.h])
AC_CHECK_HEADER(regex.h, [have_regex_h=yes],[have_regex_h=no],/* TEST_REGEX */)

dnl ------ Check for data types
dnl -- Regex-Check for MENU_COLOR
if test "$have_regex_h" = "yes"; then
  AC_CHECK_TYPE(struct re_pattern_buffer,
    [AC_DEFINE(MENU_COLOR_REGEX, [], [Use regular expressions])
     AC_DEFINE(MENU_COLOR_REGEX_GNU, [], [GNU regex is available])
    ],
    [AC_CHECK_TYPE(regex_t,
      [AC_DEFINE(MENU_COLOR_REGEX, [], [Use regular expressions])
	AC_DEFINE(MENU_COLOR_REGEX_POSIX, [], [Posix regex is available])
      ],,
      [AC_INCLUDES_DEFAULT([])
	#include <regex.h>
      ])
    ],[AC_INCLUDES_DEFAULT([])
	#include <regex.h>
      ])
fi
dnl This check is never going to fail on modern systems, and seems
dnl buggy when combined with SDL, so it's disabled.
dnl AC_CHECK_TYPE(void,,AC_DEFINE(NOVOID, [], [No void type available]))


dnl  ----- Check for library functions
AC_CHECK_DECLS([strncmpi, strnicmp], AC_DEFINE(STRNCMPI, [], [Use strncmpi]))

dnl ------ Check needed functions for TIMED_DELAY
AC_CHECK_FUNC(msleep, cf_result=yes, cf_result=no)
if test "$cf_result" = no; then
	AC_CHECK_FUNC(usleep, AC_DEFINE(TIMED_DELAY, [], [Using usleep]), [])
else
	AC_DEFINE(TIMED_DELAY, [], [Using msleep])
fi

dnl  ----- Check for program needed to build Guidebook
AC_CHECK_PROG(HAVE_NROFF, nroff, "yes", "no")
AC_CHECK_PROG(HAVE_TBL, tbl, "yes", "no")
if test "$HAVE_NROFF" != "no" ; then
if test "$HAVE_TBL" != "no" ; then
	AC_SUBST(GUIDEBOOK,[Guidebook])
	AC_SUBST(GUIDEBOOK_TXT,[Guidebook.txt])
fi
fi


dnl  ----- Check for compile-time options
# We use auto for data librarian to avoid breaking the rule that autoconf
# should use the same defaults as the manual configuration, but all platforms
# that support autoconf also support DLB, so the automatic choice is always yes.
NETHACK_ARG(data-librarian,[Data librarian],DLB,auto,
  [enable_data_librarian=yes])
NETHACK_ARG(sinks,[Kitchen sinks],SINKS,yes)
# NETHACK_ARG(light-src-spell,[Spells are light sources],LIGHT_SRC_SPELL,yes)
NETHACK_ARG(wallified-maze,[Fancy mazes],WALLIFIED_MAZE,yes)
NETHACK_ARG(reincarnation,[Special Rogue-like levels],REINCARNATION,yes)
# NETHACK_ARG(blackmarket,[Blackmarket],BLACKMARKET,yes)
NETHACK_ARG(kops,[Keystone Kops],KOPS,yes)
NETHACK_ARG(seduce,[Succubi/incubi seduction],SEDUCE,yes)
# NETHACK_ARG(invisible-objects,[Invisible objects],INVISIBLE_OBJECTS,yes)
# NETHACK_ARG(unpolypile,[Object unpolymorphing],UNPOLYPILE,yes)
# NETHACK_ARG(wallet-o-p,[Perseus' Wallet],WALLET_O_P,yes)
# NETHACK_ARG(lightsabers,[Lightsabers],LIGHTSABERS,yes)
# NETHACK_ARG(d-saber,[Dimsabers],LIGHTSABERS,yes)
# NETHACK_ARG(p-spoon,[Spoon],P_SPOON,yes)
# NETHACK_ARG(firearms,[Guns and Bullets],FIREARMS,yes)
# NETHACK_ARG(eaten-memory,[Remember which monsters have been eaten],
#  EATEN_MEMORY,yes)
NETHACK_ARG(steed,[Ability to ride monsters],STEED,yes)
# NETHACK_ARG(dwarf,[Dwarf race],DWARF,yes)
NETHACK_ARG(tourist,[Tourist class],TOURIST,yes)
# NETHACK_ARG(yeoman,[Yeoman class],YEOMAN,yes)
# NETHACK_ARG(zouthern,[Zoutherner class],ZOUTHERN,no)
NETHACK_ARG(redo,[Redo last command],REDO,yes)
NETHACK_ARG(clipping,[Allow smaller screens],CLIPPING,yes)
NETHACK_ARG(menu-color,[TTY graphics menu color],MENU_COLOR,yes)
NETHACK_ARG(elbereth,[Engraving the E-word repels monsters],ELBERETH,yes)
# NETHACK_ARG(noartifactwish,[Prevent wishing for special artifacts],
#  NOARTIFACTWISH,no)
# NETHACK_ARG(no-bones,[Disable loading and saving bones levels],NO_BONES,no)
NETHACK_ARG(user-sounds,
  [Allow user-defined mapping from messages to sounds)],
  USER_SOUNDS,no)
NETHACK_ARG(exp-on-botl,[Show experience on bottom line],EXP_ON_BOTL,yes)
NETHACK_ARG(sortloot,[Sort pickup lists alphabetically],SORTLOOT,yes)
NETHACK_ARG(score-on-botl,[Show score on bottom line],SCORE_ON_BOTL,yes)
#NETHACK_ARG(realtime-on-botl,[Show elapsed time on bottom line],REALTIME_ON_BOTL,no)
#NETHACK_ARG(borg,[Borg player (only under DOS)],BORG,no)
# NETHACK_ARG(keep-save,[Keep save files after restore],KEEP_SAVE,no)
# NETHACK_ARG(charon,[Charon's boat, enables Cerebus (not implemented)],CHARON,no)
# NETHACK_ARG(show-dmg,[Display damage reports],SHOW_DMG,yes)
# NETHACK_ARG(show-weight,[Display object weight],SHOW_WEIGHT,yes)
# NETHACK_ARG(other-services,[Shopkeeper services],OTHER_SERVICES,yes)
# NETHACK_ARG(dungeon-growth,[Dungeon growth],DUNGEON_GROWTH,yes)
#NETHACK_ARG(shout,[Shouting and pet commands (not implemented)],SHOUT,no)
# NETHACK_ARG(display-layers,[Support transparent tilesets],DISPLAY_LAYERS,yes)
NETHACK_ARG(goldobj,[Gold is kept on object chains (experimental)],GOLDOBJ,no)
NETHACK_ARG(autopickup-exceptions,[Exceptions to autopickup (experimental)],
  AUTOPICKUP_EXCEPTIONS,yes)
#NETHACK_ARG(qwertz,[QWERTZ keyboard layout support],QWERTZ,yes)
#NETHACK_ARG(simple-mail,[simple mail dgamelaunch support],SIMPLE_MAIL,no)

NETHACK_ARG(xlogfile,[More verbose logfile],XLOGFILE,yes)
AC_MSG_CHECKING([whether to enable xlogfile])
AC_ARG_ENABLE([xlogfile], AC_HELP_STRING([--enable-xlogfile=FILE],
	[More verbose logfile (default=yes, default filename=xlogfile)]),
	if test "$enableval" != no; then enable_xlogfile=yes;
	 if test "$enableval" != yes; then xlogfile=$enableval;
	 else xlogfile="xlogfile"; fi;
	else enable_xlogfile=no; fi,
	enable_xlogfile=yes; xlogfile="xlogfile")
if test "$enable_xlogfile" != "no" ; then
	AC_MSG_RESULT([yes (file $xlogfile)])
	AC_DEFINE_UNQUOTED([XLOGFILE], ["$xlogfile"], [The name of the xlogfile])
else
	AC_MSG_RESULT([no])
fi

# NETHACK_ARG(livelog,[Live game progress logfile],LIVELOGFILE,no)
#AC_MSG_CHECKING([whether to enable live logfile])
#AC_ARG_ENABLE([livelog], AC_HELP_STRING([--enable-livelog=FILE],
#	[Live game progress logfile (default=no, default filename=livelog)]),
#	if test "$enableval" != no; then enable_livelog=yes;
#	 if test "$enableval" != yes; then livelog=$enableval;
#	 else livelog="livelog"; fi;
#	else enable_livelog=no; fi,
#	enable_livelog=no)
#if test "$enable_livelog" != "no" ; then
#	AC_MSG_RESULT([yes (file $livelog)])
#	AC_DEFINE_UNQUOTED([LIVELOGFILE], ["$livelog"], [The name of the live logfile])
#
#	NETHACK_ARG(livelog-shout,[Shoutting into the live game logfile],LIVELOG_SHOUT,no)
#else
#	AC_MSG_RESULT([no])
#fi

AC_MSG_CHECKING([whether to enable wizmode])
AC_ARG_ENABLE([wizmode], AC_HELP_STRING([--enable-wizmode=USER],
	[enable wizard mode for the specified user (default=wizard)]),
	if test "$enableval" != no; then enable_wizmode=yes;
	 if test "$enableval" != yes; then wizard=$enableval;
	 else wizard="wizard"; fi;
	else enable_wizmode=no; fi,
	enable_wizmode=yes;wizard="wizard")
if test "$enable_wizmode" != "no" ; then
	AC_MSG_RESULT([yes (user $wizard)])
	AC_DEFINE_UNQUOTED([WIZARD], ["$wizard"], [The person allowed to use the -D option])
else
	AC_MSG_RESULT([no])
fi

if test "$CHOWN" != "true"; then
    AC_MSG_CHECKING([for owner of installed files])
    AC_ARG_WITH([owner], [AS_HELP_STRING([--with-owner=USER],
	    [Set the owner of installed files (default=games)])],
	    [], [with_owner=games])
    if test "$with_owner" != "no" ; then
	NETHACK_COMMAND_IFELSE(
		[find . -user "$with_owner" -prune -print > /dev/null],
		[AC_MSG_RESULT([$with_owner])
		 AC_SUBST(OWNER,[$with_owner])],
		[AC_MSG_FAILURE([test for user $with_owner failed])])
    else
	CHOWN=true
	AC_MSG_RESULT([not set])
    fi
fi

if test "$CHGRP" != "true"; then
    AC_MSG_CHECKING([for group of installed files])
    AC_ARG_WITH([group], [AS_HELP_STRING([--with-group=GROUP],
	    [Set the group of installed files (default=bin)])],
	    [], [with_group=bin])
    if test "$with_group" != "no" ; then
	NETHACK_COMMAND_IFELSE(
		[find . -group "$with_group" -prune -print > /dev/null],
		[AC_MSG_RESULT([$with_group])
		 AC_SUBST(GROUP,[$with_group])],
		[AC_MSG_FAILURE([test for group $with_group failed])])
    else
	CHGRP=true
	AC_MSG_RESULT([not set])
    fi
fi

NETHACK_LINKS=
NETHACK_LINKS_ADD([dat/data.base])
NETHACK_LINKS_ADD([dat/rumors.tru])
NETHACK_LINKS_ADD([dat/rumors.fal])
NETHACK_LINKS_ADD([dat/quest.txt])
NETHACK_LINKS_ADD([dat/oracles.txt])
NETHACK_LINKS_ADD([dat/dungeon.def])
NETHACK_LINKS_ADD([dat/help])
NETHACK_LINKS_ADD([dat/hh])
NETHACK_LINKS_ADD([dat/cmdhelp])
NETHACK_LINKS_ADD([dat/history])
NETHACK_LINKS_ADD([dat/opthelp])
NETHACK_LINKS_ADD([dat/wizhelp])
NETHACK_LINKS_ADD([include/win32api.h:sys/winnt/win32api.h])

AC_CONFIG_LINKS(${NETHACK_LINKS})

dnl  ----- Process the Makefiles
AC_CONFIG_FILES([Makefile:sys/autoconf/Makefile.top]
	[src/Makefile:sys/autoconf/Makefile.src]
	[doc/Makefile:sys/autoconf/Makefile.doc]
	[dat/Makefile:sys/autoconf/Makefile.dat]
	[util/Makefile:sys/autoconf/Makefile.utl]
	[sys/autoconf/depend.awk:sys/autoconf/depend.in])

AH_TOP([#ifndef AUTOCONF_H
#define AUTOCONF_H])
AH_BOTTOM([#endif /* AUTOCONF_H */])

NETHACK_CONFIG_HEADER([include/autoconf.h:sys/autoconf/autoconf.hin])

AC_OUTPUT
