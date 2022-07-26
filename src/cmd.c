/*	SCCS Id: @(#)cmd.c	3.4	2003/02/06	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* Modified 30 Dec 2011 by Alex Smith */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "func_tab.h"
#include "lev.h"
#include <ctype.h>
/* #define DEBUG */	/* uncomment for debugging */

/*
 * Some systems may have getchar() return EOF for various reasons, and
 * we should not quit before seeing at least NR_OF_EOFS consecutive EOFs.
 */
#if defined(SYSV) || defined(DGUX) || defined(HPUX)
#define NR_OF_EOFS	20
#endif

#define CMD_TRAVEL (char)0x90

#define REPARSE_AS_KEY 14490 /* an arbitrary large positive number */

#ifdef DEBUG
/*
 * only one "wiz_debug_cmd" routine should be available (in whatever
 * module you are trying to debug) or things are going to get rather
 * hard to link :-)
 */
extern int NDECL(wiz_debug_cmd);
#endif

#ifdef DUMB	/* stuff commented out in extern.h, but needed here */
extern int NDECL(doapply); /**/
extern int NDECL(dorub); /**/
extern int NDECL(dojump); /**/
extern int NDECL(doextlist); /**/
extern int NDECL(dodrop); /**/
extern int NDECL(doddrop); /**/
extern int NDECL(dodown); /**/
extern int NDECL(doup); /**/
extern int NDECL(donull); /**/
extern int NDECL(dowipe); /**/
extern int NDECL(do_mname); /**/
extern int NDECL(ddocall); /**/
extern int NDECL(dotakeoff); /**/
extern int NDECL(doremring); /**/
extern int NDECL(dowear); /**/
extern int NDECL(doputon); /**/
extern int NDECL(doddoremarm); /**/
extern int NDECL(dokick); /**/
extern int NDECL(dofire); /**/
extern int NDECL(dothrow); /**/
extern int NDECL(doeat); /**/
extern int NDECL(done2); /**/
extern int NDECL(doengrave); /**/
extern int NDECL(dopickup); /**/
extern int NDECL(ddoinv); /**/
extern int NDECL(dotypeinv); /**/
extern int NDECL(dolook); /**/
extern int NDECL(doprgold); /**/
extern int NDECL(doprwep); /**/
extern int NDECL(doprarm); /**/
extern int NDECL(doprring); /**/
extern int NDECL(dopramulet); /**/
extern int NDECL(doprtool); /**/
extern int NDECL(dosuspend); /**/
extern int NDECL(doforce); /**/
extern int NDECL(doopen); /**/
extern int NDECL(doclose); /**/
extern int NDECL(dosh); /**/
extern int NDECL(dodiscovered); /**/
extern int NDECL(doset); /**/
extern int NDECL(dotogglepickup); /**/
extern int NDECL(dowhatis); /**/
extern int NDECL(doquickwhatis); /**/
extern int NDECL(dowhatdoes); /**/
extern int NDECL(dohelp); /**/
extern int NDECL(dohistory); /**/
extern int NDECL(doloot); /**/
extern int NDECL(dodrink); /**/
extern int NDECL(dodip); /**/
extern int NDECL(dosacrifice); /**/
extern int NDECL(dopray); /**/
extern int NDECL(doworship); /**/
extern int NDECL(doturn); /**/
extern int NDECL(doredraw); /**/
extern int NDECL(doread); /**/
extern int NDECL(dosave); /**/
extern int NDECL(dosearch); /**/
extern int NDECL(doidtrap); /**/
extern int NDECL(dopay); /**/
extern int NDECL(dosit); /**/
extern int NDECL(dotalk); /**/
extern int NDECL(docast); /**/
extern int NDECL(dovspell); /**/
extern int NDECL(dotele); /**/
extern int NDECL(dountrap); /**/
extern int NDECL(doversion); /**/
extern int NDECL(doextversion); /**/
extern int NDECL(doswapweapon); /**/
extern int NDECL(dowield); /**/
extern int NDECL(dowieldquiver); /**/
extern int NDECL(dozap); /**/
extern int NDECL(doorganize); /**/
#endif /* DUMB */

#ifdef OVL1
static int NDECL((*timed_occ_fn));
#endif /* OVL1 */

STATIC_PTR int NDECL(doprev_message);
STATIC_PTR int NDECL(timed_occupation);
STATIC_PTR int NDECL(doextcmd);
STATIC_PTR int NDECL(domonability);
STATIC_PTR int NDECL(dotravel);
STATIC_PTR int NDECL(doautoexplore);
STATIC_PTR int NDECL(doinvite);
# ifdef WIZARD
STATIC_PTR int NDECL(wiz_wish);
STATIC_PTR int NDECL(wiz_identify);
STATIC_PTR int NDECL(wiz_map);
STATIC_PTR int NDECL(wiz_genesis);
STATIC_PTR int NDECL(wiz_where);
STATIC_PTR int NDECL(wiz_detect);
STATIC_PTR int NDECL(wiz_panic);
STATIC_PTR int NDECL(wiz_polyself);
STATIC_PTR int NDECL(wiz_level_tele);
STATIC_PTR int NDECL(wiz_level_change);
STATIC_PTR int NDECL(wiz_show_seenv);
STATIC_PTR int NDECL(wiz_show_vision);
STATIC_PTR int NDECL(wiz_mon_polycontrol);
STATIC_PTR int NDECL(wiz_show_wmodes);
STATIC_PTR int NDECL(wiz_multiplayer_yield);
#if defined(__BORLANDC__) && !defined(_WIN32)
extern void FDECL(show_borlandc_stats, (winid));
#endif
#ifdef DEBUG_MIGRATING_MONS
STATIC_PTR int NDECL(wiz_migrate_mons);
#endif
STATIC_DCL void FDECL(count_obj, (struct obj *, long *, long *, BOOLEAN_P, BOOLEAN_P));
STATIC_DCL void FDECL(obj_chain, (winid, const char *, struct obj *, long *, long *));
STATIC_DCL void FDECL(mon_invent_chain, (winid, const char *, struct monst *, long *, long *));
STATIC_DCL void FDECL(mon_chain, (winid, const char *, struct monst *, long *, long *));
STATIC_DCL void FDECL(contained, (winid, const char *, long *, long *));
STATIC_PTR int NDECL(wiz_show_stats);
#  ifdef PORT_DEBUG
STATIC_DCL int NDECL(wiz_port_debug);
#  endif
# else
extern int NDECL(tutorial_redisplay);
# endif
STATIC_PTR int NDECL(doattributes);
STATIC_PTR int NDECL(doconduct); /**/

#ifdef OVLB
STATIC_DCL void FDECL(enlght_line, (const char *,const char *,const char *));
STATIC_DCL char *FDECL(enlght_combatinc, (const char *,int,int,char *));
#ifdef UNIX
static void NDECL(end_of_input);
#endif
#endif /* OVLB */

static const char* readchar_queue="";

STATIC_DCL char *NDECL(parse);
STATIC_DCL boolean FDECL(help_dir, (CHAR_P,const char *));

#ifdef OVL1

STATIC_PTR int
doprev_message()
{
    return nh_doprev_message();
}

/* Count down by decrementing multi */
STATIC_PTR int
timed_occupation()
{
	(*timed_occ_fn)();
	if (multi > 0)
		multi--;
	return multi > 0;
}

/* If you have moved since initially setting some occupations, they
 * now shouldn't be able to restart.
 *
 * The basic rule is that if you are carrying it, you can continue
 * since it is with you.  If you are acting on something at a distance,
 * your orientation to it must have changed when you moved.
 *
 * The exception to this is taking off items, since they can be taken
 * off in a number of ways in the intervening time, screwing up ordering.
 *
 *	Currently:	Take off all armor.
 *			Picking Locks / Forcing Chests.
 *			Setting traps.
 */
void
reset_occupations()
{
	reset_remarm();
	reset_pick();
	reset_trapset();
}

/* If a time is given, use it to timeout this function, otherwise the
 * function times out by its own means.
 */
void
set_occupation(fn, txt, xtime)
int NDECL((*fn));
const char *txt;
int xtime;
{
	if (xtime) {
		occupation = timed_occupation;
		timed_occ_fn = fn;
	} else
		occupation = fn;
	occtxt = txt;
	occtime = 0;
	return;
}

#ifdef REDO

static char NDECL(popch);

/* Provide a means to redo the last command.  The flag `in_doagain' is set
 * to true while redoing the command.  This flag is tested in commands that
 * require additional input (like `throw' which requires a thing and a
 * direction), and the input prompt is not shown.  Also, while in_doagain is
 * TRUE, no keystrokes can be saved into the saveq.
 */
#define BSIZE 20
static char pushq[BSIZE], saveq[BSIZE];
static NEARDATA int phead, ptail, shead, stail;

static char
popch() {
	/* If occupied, return '\0', letting tgetch know a character should
	 * be read from the keyboard.  If the character read is not the
	 * ABORT character (as checked in pcmain.c), that character will be
	 * pushed back on the pushq.
	 */
	if (occupation) return '\0';
	if (in_doagain) return(char)((shead != stail) ? saveq[stail++] : '\0');
	else		return(char)((phead != ptail) ? pushq[ptail++] : '\0');
}

char
pgetchar() {		/* curtesy of aeb@cwi.nl */
	register int ch;

	if(!(ch = popch()))
		ch = nhgetch();
	return((char)ch);
}

/* A ch == 0 resets the pushq */
void
pushch(ch)
char ch;
{
	if (!ch)
		phead = ptail = 0;
	if (phead < BSIZE)
		pushq[phead++] = ch;
	return;
}

/* A ch == 0 resets the saveq.	Only save keystrokes when not
 * replaying a previous command.
 */
void
savech(ch)
char ch;
{
	if (!in_doagain) {
		if (!ch)
			phead = ptail = shead = stail = 0;
		else if (shead < BSIZE)
			saveq[shead++] = ch;
	}
	return;
}
#endif /* REDO */

#endif /* OVL1 */
#ifdef OVLB

static boolean accessed_via_extcmd = FALSE;

STATIC_PTR int
doextcmd()	/* here after # - now read a full-word command */
{
	int idx, retval;
        static char save_cm_ext[BUFSZ];

	/* keep repeating until we don't run help or quit */
	do {
	    idx = get_ext_cmd();
	    if (idx < 0) return 0;	/* quit */

            /* Make #20#wait, etc., and redo work correctly */
            {
                char *r;
                /* Note that this depends on # being unrebindable. */
                save_cm = save_cm_ext;
                Sprintf(save_cm, "#%s", extcmdlist[idx].ef_txt);
#ifdef REDO
                savech(0);
                /* We have to duplicate some characters; ungetc()
                   obviously doesn't work inside key replay, so
                   we instead double the characters that would be
                   ungetted. */
                savech('#');
                savech('#');
                for (r = save_cm+1; *r; r++) savech(*r);
                savech('\n');
#endif
            }

            if (extcmdlist[idx].ef_funct == 0)
              return -(int)REPARSE_AS_KEY -
                (int)(unsigned char)extcmdlist[idx].replacewithkey;

            accessed_via_extcmd = TRUE;
	    retval = (*extcmdlist[idx].ef_funct)();
            accessed_via_extcmd = FALSE;
	} while (extcmdlist[idx].ef_funct == doextlist);

	return retval;
}

boolean
doing_extended_command_override()
{
  return accessed_via_extcmd;
}

int
doextlist()	/* here after #? - now list all commands */
{
	register const struct ext_func_tab *efp;
	char	 buf[BUFSZ], kbuf[BUFSZ];
	winid datawin;

	datawin = create_nhwindow(NHW_TEXT);
	putstr(datawin, 0, "");
	putstr(datawin, 0, "            Commands List");
	putstr(datawin, 0, "");
	putstr(datawin, 0,
" This is a full list of commands available to have keys bound to them.");
	putstr(datawin, 0, "");

	for(efp = extcmdlist; efp->ef_txt; efp++) {
                Sprintf(kbuf, "#%s", efp->ef_txt);
		Sprintf(buf, "%2s%21s - %s.",
                        key_for_cmd(kbuf) == kbuf ? "" :
                        key_for_cmd(kbuf), kbuf, efp->ef_desc);
		putstr(datawin, 0, buf);
	}
	display_nhwindow(datawin, FALSE);
	destroy_nhwindow(datawin);
	return 0;
}

#if defined(TTY_GRAPHICS) || defined(GL_GRAPHICS) || defined(SDL_GRAPHICS)
#define MAX_EXT_CMD 90		/* Change if we ever have > 90 ext cmds */
/*
 * This is currently used only by the tty port and is
 * controlled via runtime option 'extmenu'
 */
int
extcmd_via_menu()	/* here after # - now show pick-list of possible commands */
{
    const struct ext_func_tab *efp;
    menu_item *pick_list = (menu_item *)0;
    winid win;
    anything any;
    const struct ext_func_tab *choices[MAX_EXT_CMD];
    char buf[BUFSZ];
    char cbuf[QBUFSZ], prompt[QBUFSZ], fmtstr[20];
    int i, n, nchoices, acount;
    int ret,  biggest;
    int accelerator, prevaccelerator;
    int  matchlevel = 0;

    ret = 0;
    cbuf[0] = '\0';
    biggest = 0;
    while (!ret) {
	    i = n = 0;
	    accelerator = 0;
	    any.a_void = 0;
	    /* populate choices */
	    for(efp = extcmdlist; efp->ef_txt; efp++) {
		if (!matchlevel || !strncmp(efp->ef_txt, cbuf, matchlevel)) {
			choices[i++] = efp;
			if ((int)strlen(efp->ef_desc) > biggest) {
				biggest = strlen(efp->ef_desc);
				Sprintf(fmtstr,"%%-%ds", biggest + 15);
			}
#ifdef DEBUG
			if (i >= MAX_EXT_CMD - 2) {
			    impossible("Exceeded %d extended commands in doextcmd() menu",
					MAX_EXT_CMD - 2);
			    return 0;
			}
#endif
		}
	    }
	    choices[i] = (struct ext_func_tab *)0;
	    nchoices = i;
	    /* if we're down to one, we have our selection so get out of here */
	    if (nchoices == 1) {
		for (i = 0; extcmdlist[i].ef_txt != (char *)0; i++)
			if (!strncmpi(extcmdlist[i].ef_txt, cbuf, matchlevel)) {
				ret = i;
				break;
			}
		break;
	    }

	    /* otherwise... */
	    win = create_nhwindow(NHW_MENU);
	    start_menu(win);
	    prevaccelerator = 0;
	    acount = 0;
	    for(i = 0; choices[i]; ++i) {
		accelerator = choices[i]->ef_txt[matchlevel];
		if (accelerator != prevaccelerator || nchoices < (ROWNO - 3)) {
		    if (acount) {
 			/* flush the extended commands for that letter already in buf */
			Sprintf(buf, fmtstr, prompt);
			any.a_char = prevaccelerator;
			add_menu(win, NO_GLYPH, &any, any.a_char, 0,
					ATR_NONE, buf, FALSE);
			acount = 0;
		    }
		}
		prevaccelerator = accelerator;
		if (!acount || nchoices < (ROWNO - 3)) {
		    Sprintf(prompt, "%s [%s]", choices[i]->ef_txt,
				choices[i]->ef_desc);
		} else if (acount == 1) {
		    Sprintf(prompt, "%s or %s", choices[i-1]->ef_txt,
				choices[i]->ef_txt);
		} else {
		    Strcat(prompt," or ");
		    Strcat(prompt, choices[i]->ef_txt);
		}
		++acount;
	    }
	    if (acount) {
		/* flush buf */
		Sprintf(buf, fmtstr, prompt);
		any.a_char = prevaccelerator;
		add_menu(win, NO_GLYPH, &any, any.a_char, 0, ATR_NONE, buf, FALSE);
	    }
	    Sprintf(prompt, "Extended Command: %s", cbuf);
	    end_menu(win, prompt);
	    n = select_menu(win, PICK_ONE, &pick_list);
	    destroy_nhwindow(win);
	    if (n==1) {
		if (matchlevel > (QBUFSZ - 2)) {
			free((genericptr_t)pick_list);
#ifdef DEBUG
			impossible("Too many characters (%d) entered in extcmd_via_menu()",
				matchlevel);
#endif
			ret = -1;
		} else {
			cbuf[matchlevel++] = pick_list[0].item.a_char;
			cbuf[matchlevel] = '\0';
			free((genericptr_t)pick_list);
		}
	    } else {
		if (matchlevel) {
			ret = 0;
			matchlevel = 0;
		} else
			ret = -1;
	    }
    }
    return ret;
}
#endif

/* #monster command - use special monster ability while polymorphed */
STATIC_PTR int
domonability()
{
	if (can_breathe(youmonst.data)) return dobreathe();
	else if (attacktype(youmonst.data, AT_SPIT)) return dospit();
	else if (attacktype(youmonst.data, AT_MAGC))
	    return castum((struct monst *)0,
	                   &youmonst.data->mattk[attacktype(youmonst.data, 
			                         AT_MAGC)]);
	else if (youmonst.data->mlet == S_NYMPH) return doremove();
	else if (attacktype(youmonst.data, AT_GAZE)) return dogaze();
	else if (is_were(youmonst.data)) return dosummon();
	else if (webmaker(youmonst.data)) return dospinweb();
	else if (is_hider(youmonst.data)) return dohide();
	else if (is_mind_flayer(youmonst.data)) return domindblast();
	else if (u.umonnum == PM_GREMLIN) {
	    if(IS_FOUNTAIN(levl[u.ux][u.uy].typ)) {
		if (split_mon(&youmonst, (struct monst *)0))
		    dryup(u.ux, u.uy, TRUE);
	    } else There("is no fountain here.");
	} else if (is_unicorn(youmonst.data)) {
	    use_unicorn_horn((struct obj *)0);
	    return 1;
	} else if (youmonst.data->msound == MS_SHRIEK) {
	    You("shriek.");
	    if(u.uburied)
		pline("Unfortunately sound does not carry well through rock.");
	    else aggravate();
	} else if (Upolyd)
		pline("Any special ability you may have is purely reflexive.");
	else You("don't have a special ability in your normal form!");
	return 0;
}

#if 0 /* OBSOLETE */
STATIC_PTR int
enter_explore_mode()
{
	if(!discover && !wizard) {
		pline("Beware!  From explore mode there will be no return to normal game.");
		if (yn("Do you want to enter explore mode?") == 'y') {
			clear_nhwindow(WIN_MESSAGE);
			You("are now in non-scoring explore mode.");
			discover = TRUE;
		}
		else {
			clear_nhwindow(WIN_MESSAGE);
			pline("Resuming normal game.");
		}
	}
	return 0;
}
#endif

/* #invite command - invite another player */
STATIC_PTR int
doinvite()
{
  struct monst *mtmp;
  char lockname[BUFSZ];
  char *invitepid = getenv("INVITEPID");
  int invitepidlen = invitepid ? strlen(invitepid) : 0;
  if (invitepid) strcpy(lockname, invitepid);
  char buf[BUFSZ];
  int fd;
  xchar x, y; /* same type as u.ux, u.uy */
  /* Save compatibility kludge - no inviting if there's no Waiting
     Room, sign of a game started on a binary without multiplayer. */
  if (!find_level("wait"))
  {
    pline("This dungeon is too old for you to have guests.");
    return 0;
  }
  /* The conditions for an invite: you must be within view of the dlvl
     1 upstairs, and that square must be unoccupied. Also, solo mode
     and explore mode disallow multiplayer, not for technical reasons,
     but for consistency reasons. */
  if (solo)
  {
    You_cant("use other players to help you play solo!");
    return 0;
  }
  if (heaven_or_hell_mode || hell_and_hell_mode) {
      You_cant("seek help against the hordes of hell!");
      return 0;
  }

  if (discover && !wizard)
  {
    You_cant("invite players in explore mode.");
    return 0;
  }
  if (depth(&u.uz) != 1 || !sstairs.sx || !cansee(sstairs.sx,sstairs.sy)) {
    You_cant("invite players if you can't see the dungeon level 1 exit ladder.");
    return 0;
  }
  if (u.ux == sstairs.sx && u.uy == sstairs.sy)
  {
    You_cant("invite players if you're blocking the exit ladder.");
    return 0;
  }

  /* Avoid multiplayer-unsafe or yield-unsafe circumstances. */
  if (u.usteed)
  { /* multiplayer-unsafe (pointer from lockfile 0 to other lockfiles) */
    You_cant("ride a steed in multiplayer.");
    return 0;
  }
  if (u.ustuck)
  { /* yield-unsafe */
    You_cant("invite a player while %s has you trapped!", a_monnam(u.ustuck));
    return 0;
  }
  /* This limitation is not because it would crash the program, but because
     it effectively leashes the pet to all players on the level at once,
     which is not at all what people expect. */
  if (number_leashed()) {
    You_cant("leash pets in multiplayer. Unleash them before inviting.");
    return 0;
  }

  /* This leaks a tiny bit of information. Oh well... */
  if ((mtmp = m_at(sstairs.sx,sstairs.sy))) {
    if (canseemon(mtmp))
      You_cant("invite players with %s blocking the ladder.", a_monnam(mtmp));
    else
      You_cant("invite players. Perhaps something is blocking the ladder?");
    return 0;
  }

  /* Necessary if there's even a slight possibility we might yield; is
     harmless if we don't */
  checkpoint_level();

  /* If invitation is legal from this dungeon's point of view, ask who to
     invite. For the time being, use lockfile names. (Ideally we want a
     list eventually.) */
  getlin("Who do you want to invite?", lockname + invitepidlen);
  if (!lockname[invitepidlen] || lockname[invitepidlen] == '\033')
    return 0; /* cancelled */

  /* If we don't have a control pipe yet, set one up now.
     If we cancel and continue as a single-player game from there, we just
     have a control pipe nobody is using; no great problem. */
  if (multiplayer_pipe_fd() == -1)
    setup_multiplayer_pipe('c');

  fd = other_multiplayer_pipe_fd(lockname, 'a');
  if (fd < 0) {
    pline("%s doesn't seem to be waiting to join a game.",
	  lockname + invitepidlen);
    return 0;
  }
  /* mplock is the multiplayer lockname, that's used to identify a
     particular running process. We need to send our own here. */
  Sprintf(buf, "%s y\n", mplock);
  /* TODO: SIGPIPE due to a really unlikely race condition */
  if(write(fd, buf, strlen(buf)) < 0) {
    impossible("The game you're trying to invite seems to have crashed.");
    return 0;
  }
  close(fd);
  /* Now await the reply or user cancel. */
  pline("Waiting for response... (ESC = stop waiting)");
  suppress_more();
  switch (mp_await_reply_or_yield()) {
  case 0: /* declined */
    pline("%s declined your invitation.", lockname + invitepidlen);
    break;
  case 1: /* accepted */
    pline("Invitation accepted, connecting...");
    suppress_more();
    /* Before the connection can be set up, we need to move the lockfiles
       to a name not associated with any player, so that if players die
       they can start a single-player game without filename clashes. */
    if (!iflags.multiplayer) {
      /* We need to pick a currently unused name first time. We ensure
         that all games have a unique u.ubirthday, so we just use that. */
      int ln, fd;
      char errbuf[BUFSZ];  
      Sprintf(iflags.mp_lock_name, "%llu", (long long)u.ubirthday);
      /* Rename all nonlocal level files. */
      for (ln = maxledgerno(); ln; ln--) {
        if (ledger_is_local(ln)) continue;
        if (rename_levelfile(ln, iflags.mp_lock_name, errbuf) < 0) {
          /* We can't do much here but panic, and let the other game
             fail with a communication error when its timer runs out.
             The partially renamed lockfiles will make recovery nasty,
             but potentially still possible; and it's hard to see how
             this could go wrong halfway through the list of lockfiles
             anyway except if someone tampers with the files by hand,
             or the system is very low on disk space. */
          panic("%s",errbuf);
        }
      }
      /* "Level file" -1 is used to count the number of players in the
         dungeon (so that a process knows when to clean up and when to
         let others clean up); its actual content is irrelevant, with
         the processes instead being counted by advisory locks (which
         works correctly even if one or more of the processes crash,
         although a crash will still block all other players on the
         level if done() isn't called). */
      iflags.multiplayer = TRUE;
      fd = create_levelfile(-1, errbuf);
      if (fd < 0) panic("%s",errbuf);
      flock(fd, LOCK_SH);
      iflags.shared_lockfd = fd;
    }
    /* The other end of the connection badly needs to know the dungeon
       layout right now; in fact, it's waiting on a timer. So we do
       that as soon as the lockfiles are set up correctly. */
    fd = other_multiplayer_pipe_fd(lockname, 'c');
    if (fd < 0) panic("Multiplayer control pipe not ready");
    save_dungeon(fd, TRUE, FALSE);
    savelevchn(fd, WRITE_SAVE);
    /* It needs to know the player's coordinates, too, to do things
       in the right order. x and y are the same size as u.ux and
       u.uy, or this wouldn't work. Likewise, we need to ensure that
       the games have a consistent view of time. Finally, we need
       to tell it where to look for the lockfiles; that is, the
       current filename, and the birthday so that it can find them
       after a save and restore. */
    x = sstairs.sx; y = sstairs.sy;
    if (write(fd, &x, sizeof(u.ux)) <= 0 ||
        write(fd, &y, sizeof(u.uy)) <= 0 ||
        write(fd, &monstermoves, sizeof(monstermoves)) <= 0 || 
        write(fd, iflags.mp_lock_name, BUFSZ) <= 0 ||
        write(fd, &u.ubirthday, sizeof(u.ubirthday)) <= 0) {
      panic("Multiplayer control pipe write failure");
    }
    /* Now the other end has control, and we're just waiting on a
       yield. */
    while (mp_await_reply_or_yield() != 2) {}
    uncheckpoint_level();
    break;
  case 3: /* cancelled */
    /* This one's a bit tricky. Either the other end's crashed, in
       which case we'll never get a reply, or it hasn't, in which case
       we will and it'll be indistinguishable from a valid reply in an
       entirely different context. Worse still, if the other end
       accepts, it'll find us unable to supply the data. TODO: A more
       robust solution to this is needed eventually; for the time
       being, we just continue and hope the response happens while
       we're either driving or yielded (which is the vast majority of
       the time; problems happen if it comes while we're rplining
       another game, or changing levels.) */
    pline("Invitation cancelled.");
    break;
  default:
    impossible("Invalid remote connection response");
    break;
  }
  return 0;
}

#ifdef WIZARD

/* #yield command - test multiplayer yield routines */
STATIC_PTR int
wiz_multiplayer_yield()
{
  if (!wizard) {
    pline("Unavailable command #yield.");
    return 0;
  }
  if (checkpoint_level() < 0) {
    pline("Checkpoint failed, not uncheckpointing!");
    return 0;
  }
  pline("Checkpointed. Clear this message to uncheckpoint again...");
  display_nhwindow(WIN_MESSAGE, FALSE); /* force --More-- */
  if (uncheckpoint_level() < 0) {
    /* Uncheckpointing shouldn't go wrong if checkpointing worked (at
       the other end, if necessary). */
    impossible("Uncheckpoint failed in wiz_multiplayer_yield!");
  }
  return 0;
}

/* ^W command - wish for something */
STATIC_PTR int
wiz_wish()	/* Unlimited wishes for debug mode by Paul Polderman */
{
	if (wizard) {
	    boolean save_verbose = flags.verbose;

	    flags.verbose = FALSE;
	    makewish();
	    flags.verbose = save_verbose;
	    (void) encumber_msg();
	} else
	    pline("Unavailable command '^W'.");
	return 0;
}

/* ^I command - identify hero's inventory */
STATIC_PTR int
wiz_identify()
{
	if (wizard)	identify_pack(0);
	else		pline("Unavailable command '^I'.");
	return 0;
}

/* ^F command - reveal the level map and any traps on it */
STATIC_PTR int
wiz_map()
{
	if (wizard) {
	    struct trap *t;
	    long save_Hconf = HConfusion,
		 save_Hhallu = HHallucination;

	    HConfusion = HHallucination = 0L;
	    for (t = ftrap; t != 0; t = t->ntrap) {
		t->tseen = 1;
		map_trap(t, TRUE);
	    }
	    do_mapping();
	    HConfusion = save_Hconf;
	    HHallucination = save_Hhallu;
	} else
	    pline("Unavailable command '^F'.");
	return 0;
}

/* ^G command - generate monster(s); a count prefix will be honored */
STATIC_PTR int
wiz_genesis()
{
	if (wizard)	(void) create_particular();
	else		pline("Unavailable command '^G'.");
	return 0;
}

/* ^O command - display dungeon layout */
STATIC_PTR int
wiz_where()
{
	if (wizard) (void) print_dungeon(FALSE, (schar *)0, (xchar *)0);
	else	    pline("Unavailable command '^O'.");
	return 0;
}

/* ^E command - detect unseen (secret doors, traps, hidden monsters) */
STATIC_PTR int
wiz_detect()
{
	if(wizard)  (void) findit();
	else	    pline("Unavailable command '^E'.");
	return 0;
}

/* ^V command - level teleport, or tutorial review */
STATIC_PTR int
wiz_level_tele()
{
	if (wizard)	level_tele();
	else if(flags.tutorial)
	    tutorial_redisplay();
	return 0;
}

/* #monpolycontrol command - choose new form for shapechangers, polymorphees */
STATIC_PTR int
wiz_mon_polycontrol()
{
    iflags.mon_polycontrol = !iflags.mon_polycontrol;
    pline("Monster polymorph control is %s.",
	  iflags.mon_polycontrol ? "on" : "off");
    return 0;
}

/* #levelchange command - adjust hero's experience level */
STATIC_PTR int
wiz_level_change()
{
    char buf[BUFSZ];
    int newlevel;
    int ret;

    getlin("To what experience level do you want to be set?", buf);
    (void)mungspaces(buf);
    if (buf[0] == '\033' || buf[0] == '\0') ret = 0;
    else ret = sscanf(buf, "%d", &newlevel);

    if (ret != 1) {
      pline("%s",Never_mind);
	return 0;
    }
    if (newlevel == u.ulevel) {
	You("are already that experienced.");
    } else if (newlevel < u.ulevel) {
	if (u.ulevel == 1) {
	    You("are already as inexperienced as you can get.");
	    return 0;
	}
	if (newlevel < 1) newlevel = 1;
	while (u.ulevel > newlevel)
	    losexp("#levelchange");
    } else {
	if (u.ulevel >= MAXULEV) {
	    You("are already as experienced as you can get.");
	    return 0;
	}
	if (newlevel > MAXULEV) newlevel = MAXULEV;
	while (u.ulevel < newlevel)
	    pluslvl(FALSE);
    }
    u.ulevelmax = u.ulevel;
    return 0;
}

/* #panic command - test program's panic handling */
STATIC_PTR int
wiz_panic()
{
	if (yn("Do you want to call panic() and end your game?") == 'y')
		panic("crash test.");
        return 0;
}

/* #polyself command - change hero's form */
STATIC_PTR int
wiz_polyself()
{
        polyself(TRUE);
        return 0;
}

/* #seenv command */
STATIC_PTR int
wiz_show_seenv()
{
	winid win;
	int x, y, v, startx, stopx, curx;
	char row[COLNO+1];

	win = create_nhwindow(NHW_TEXT);
	/*
	 * Each seenv description takes up 2 characters, so center
	 * the seenv display around the hero.
	 */
	startx = max(1, u.ux-(COLNO/4));
	stopx = min(startx+(COLNO/2), COLNO);
	/* can't have a line exactly 80 chars long */
	if (stopx - startx == COLNO/2) startx++;

	for (y = 0; y < ROWNO; y++) {
	    for (x = startx, curx = 0; x < stopx; x++, curx += 2) {
		if (x == u.ux && y == u.uy) {
		    row[curx] = row[curx+1] = '@';
		} else {
		    v = levl[x][y].seenv & 0xff;
		    if (v == 0)
			row[curx] = row[curx+1] = ' ';
		    else
			Sprintf(&row[curx], "%02x", v);
		}
	    }
	    /* remove trailing spaces */
	    for (x = curx-1; x >= 0; x--)
		if (row[x] != ' ') break;
	    row[x+1] = '\0';

	    putstr(win, 0, row);
	}
	display_nhwindow(win, TRUE);
	destroy_nhwindow(win);
	return 0;
}

/* #vision command */
STATIC_PTR int
wiz_show_vision()
{
	winid win;
	int x, y, v;
	char row[COLNO+1];

	win = create_nhwindow(NHW_TEXT);
	Sprintf(row, "Flags: 0x%x could see, 0x%x in sight, 0x%x temp lit",
		COULD_SEE, IN_SIGHT, TEMP_LIT);
	putstr(win, 0, row);
	putstr(win, 0, "");
	for (y = 0; y < ROWNO; y++) {
	    for (x = 1; x < COLNO; x++) {
		if (x == u.ux && y == u.uy)
		    row[x] = '@';
		else {
		    v = viz_array[y][x]; /* data access should be hidden */
		    if (v == 0)
			row[x] = ' ';
		    else
			row[x] = '0' + viz_array[y][x];
		}
	    }
	    /* remove trailing spaces */
	    for (x = COLNO-1; x >= 1; x--)
		if (row[x] != ' ') break;
	    row[x+1] = '\0';

	    putstr(win, 0, &row[1]);
	}
	display_nhwindow(win, TRUE);
	destroy_nhwindow(win);
	return 0;
}

/* #wmode command */
STATIC_PTR int
wiz_show_wmodes()
{
	winid win;
	int x,y;
	char row[COLNO+1];
	struct rm *lev;

	win = create_nhwindow(NHW_TEXT);
	for (y = 0; y < ROWNO; y++) {
	    for (x = 0; x < COLNO; x++) {
		lev = &levl[x][y];
		if (x == u.ux && y == u.uy)
		    row[x] = '@';
		else if (IS_WALL(lev->typ) || lev->typ == SDOOR)
		    row[x] = '0' + (lev->wall_info & WM_MASK);
		else if (lev->typ == CORR)
		    row[x] = '#';
		else if (IS_ROOM(lev->typ) || IS_DOOR(lev->typ))
		    row[x] = '.';
		else
		    row[x] = 'x';
	    }
	    row[COLNO] = '\0';
	    putstr(win, 0, row);
	}
	display_nhwindow(win, TRUE);
	destroy_nhwindow(win);
	return 0;
}

#endif /* WIZARD */


/* -enlightenment and conduct- */
static winid en_win;
static const char
	You_[] = "You ",
	are[]  = "are ",  were[]  = "were ",
	have[] = "have ", had[]   = "had ",
	can[]  = "can ",  could[] = "could ";
static const char
	have_been[]  = "have been ",
	have_never[] = "have never ", never[] = "never ";

#define enl_msg(prefix,present,past,suffix) \
			enlght_line(prefix, final ? past : present, suffix)
#define you_are(attr)	enl_msg(You_,are,were,attr)
#define you_have(attr)	enl_msg(You_,have,had,attr)
#define you_can(attr)	enl_msg(You_,can,could,attr)
#define you_have_been(goodthing) enl_msg(You_,have_been,were,goodthing)
#define you_have_never(badthing) enl_msg(You_,have_never,never,badthing)
#define you_have_X(something)	enl_msg(You_,have,(const char *)"",something)

static void
enlght_line(start, middle, end)
const char *start, *middle, *end;
{
	char buf[BUFSZ];

	Sprintf(buf, "%s%s%s.", start, middle, end);
	putstr_or_dump(en_win, 0, buf);
}

/* format increased damage or chance to hit */
static char *
enlght_combatinc(inctyp, incamt, final, outbuf)
const char *inctyp;
int incamt, final;
char *outbuf;
{
	char numbuf[24];
	const char *modif, *bonus;

	if (final
#ifdef WIZARD
		|| wizard
#endif
	  ) {
	    Sprintf(numbuf, "%s%d",
		    (incamt > 0) ? "+" : "", incamt);
	    modif = (const char *) numbuf;
	} else {
	    int absamt = abs(incamt);

	    if (absamt <= 3) modif = "small";
	    else if (absamt <= 6) modif = "moderate";
	    else if (absamt <= 12) modif = "large";
	    else modif = "huge";
	}
	bonus = (incamt > 0) ? "bonus" : "penalty";
	/* "bonus to hit" vs "damage bonus" */
	if (!strcmp(inctyp, "damage")) {
	    const char *ctmp = inctyp;
	    inctyp = bonus;
	    bonus = ctmp;
	}
	Sprintf(outbuf, "%s %s %s", an(modif), bonus, inctyp);
	return outbuf;
}

void
enlightenment(final)
int final;	/* 0 => still in progress; 1 => over, survived; 2 => dead */
{
	int ltmp;
	char buf[BUFSZ];

        if (!putstr_or_dump) putstr_or_dump = putstr;

	if (putstr_or_dump == putstr) en_win = create_nhwindow(NHW_MENU);
	putstr_or_dump(en_win, 0, final ? "Final attributes:" : "Current attributes:");
	putstr_or_dump(en_win, 0, "");

#ifdef ELBERETH
	if (u.uevent.uhand_of_elbereth) {
	    static const char * const hofe_titles[3] = {
				"the Hand of Elbereth",
				"the Envoy of Balance",
				"the Glory of Arioch"
	    };
	    you_are(hofe_titles[u.uevent.uhand_of_elbereth - 1]);
	}
#endif
    /* heaven or hell modes */
	if (heaven_or_hell_mode) {
		if (u.ulives > 1)
			Sprintf(buf, "%ld lives", u.ulives);
		else if (u.ulives == 0)
			Sprintf(buf, "no lives");
		else
			Sprintf(buf, "%ld life", u.ulives);
		you_have(buf);
	}

	/* note: piousness 3 matches MIN_QUEST_ALIGN (quest.h) */
	if (u.ualign.record >= 20)	you_are("piously aligned");
	else if (u.ualign.record > 13)	you_are("devoutly aligned");
	else if (u.ualign.record > 8)	you_are("fervently aligned");
	else if (u.ualign.record > 3)	you_are("stridently aligned");
	else if (u.ualign.record == 3)	you_are("aligned");
	else if (u.ualign.record > 0)	you_are("haltingly aligned");
	else if (u.ualign.record == 0)	you_are("nominally aligned");
	else if (u.ualign.record >= -3)	you_have("strayed");
	else if (u.ualign.record >= -8)	you_have("sinned");
	else you_have("transgressed");
#ifdef WIZARD
	if (wizard) {
		Sprintf(buf, " %d", u.ualign.record);
		enl_msg("Your alignment ", "is", "was", buf);
	}
#endif

	/*** Resistances to troubles ***/
	if (Fire_resistance) you_are("fire resistant");
	if (Cold_resistance) you_are("cold resistant");
	if (Sleep_resistance) you_are("sleep resistant");
	if (Disint_resistance) you_are("disintegration-resistant");
	if (Shock_resistance) you_are("shock resistant");
	if (Poison_resistance) you_are("poison resistant");
	if (Drain_resistance) you_are("level-drain resistant");
	if (Sick_resistance) you_are("immune to sickness");
	if (Antimagic) you_are("magic-protected");
	if (Acid_resistance) you_are("acid resistant");
	if (Stone_resistance)
		you_are("petrification resistant");
	if (Invulnerable) you_are("invulnerable");
	if (u.uedibility) you_can("recognize detrimental food");

	/*** Troubles ***/
	if (Halluc_resistance)
		enl_msg("You resist", "", "ed", " hallucinations");
	if (final) {
		if (Hallucination) you_are("hallucinating");
		if (Stunned) you_are("stunned");
		if (Confusion) you_are("confused");
		if (Blinded) you_are("blinded");
		if (Sick) {
			if (u.usick_type & SICK_VOMITABLE)
				you_are("sick from food poisoning");
			if (u.usick_type & SICK_NONVOMITABLE)
				you_are("sick from illness");
		}
	}
	if (Stoned) you_are("turning to stone");
	if (Slimed) you_are("turning into slime");
	if (Strangled) you_are((u.uburied) ? "buried" : "being strangled");
	if (Glib) {
		Sprintf(buf, "slippery %s", makeplural(body_part(FINGER)));
		you_have(buf);
	}
	if (Fumbling) enl_msg("You fumble", "", "d", "");
	if (Wounded_legs
#ifdef STEED
	    && !u.usteed
#endif
			  ) {
		Sprintf(buf, "wounded %s", makeplural(body_part(LEG)));
		you_have(buf);
	}
#if defined(WIZARD) && defined(STEED)
	if (Wounded_legs && u.usteed && wizard) {
	    Strcpy(buf, x_monnam(u.usteed, ARTICLE_YOUR, (char *)0, 
		    SUPPRESS_SADDLE | SUPPRESS_HALLUCINATION, FALSE));
	    *buf = highc(*buf);
	    enl_msg(buf, " has", " had", " wounded legs");
	}
#endif
	if (Sleeping) enl_msg("You ", "fall", "fell", " asleep");
	if (Hunger) enl_msg("You hunger", "", "ed", " rapidly");

	/*** Vision and senses ***/
	if (See_invisible) enl_msg(You_, "see", "saw", " invisible");
	if (Blind_telepat) you_are("telepathic");
	if (Warning) you_are("warned");
	if (Warn_of_mon && flags.warntype) {
		Sprintf(buf, "aware of the presence of %s",
			(flags.warntype & M2_ORC) ? "orcs" :
			(flags.warntype & M2_DEMON) ? "demons" :
			something); 
		you_are(buf);
	}
	if (Undead_warning) you_are("warned of undead");
	if (Searching) you_have("automatic searching");
	if (Clairvoyant) you_are("clairvoyant");
	if (Infravision) you_have("infravision");
	if (Detect_monsters) you_are("sensing the presence of monsters");
	if (u.umconf) you_are("going to confuse monsters");

	/*** Appearance and behavior ***/
	if (Adornment) {
	    int adorn = 0;

	    if(uleft && uleft->otyp == RIN_ADORNMENT) adorn += uleft->spe;
	    if(uright && uright->otyp == RIN_ADORNMENT) adorn += uright->spe;
	    if (adorn < 0)
		you_are("poorly adorned");
	    else
		you_are("adorned");
	}
	if (Invisible) you_are("invisible");
	else if (Invis) you_are("invisible to others");
	/* ordinarily "visible" is redundant; this is a special case for
	   the situation when invisibility would be an expected attribute */
	else if ((HInvis || EInvis || pm_invisible(youmonst.data)) && BInvis)
	    you_are("visible");
	if (Displaced) you_are("displaced");
	if (Stealth) you_are("stealthy");
	if (Aggravate_monster) enl_msg("You aggravate", "", "d", " monsters");
	if (Conflict) enl_msg("You cause", "", "d", " conflict");

	/*** Transportation ***/
	if (Jumping) you_can("jump");
	if (Teleportation) you_can("teleport");
	if (Teleport_control) you_have("teleport control");
	if (Lev_at_will) you_are("levitating, at will");
	else if (Levitation) you_are("levitating");	/* without control */
	else if (Flying) you_can("fly");
	if (Wwalking) you_can("walk on water");
	if (Swimming) you_can("swim");        
	if (Breathless) you_can("survive without air");
	else if (Amphibious) you_can("breathe water");
	if (Passes_walls) you_can("walk through walls");
#ifdef STEED
	/* If you die while dismounting, u.usteed is still set.  Since several
	 * places in the done() sequence depend on u.usteed, just detect this
	 * special case. */
	if (u.usteed && (final < 2 || strcmp(killer, "riding accident"))) {
	    Sprintf(buf, "riding %s", y_monnam(u.usteed));
	    you_are(buf);
	}
#endif
	if (u.uswallow) {
	    Sprintf(buf, "swallowed by %s", a_monnam(u.ustuck));
#ifdef WIZARD
	    if (wizard) Sprintf(eos(buf), " (%u)", u.uswldtim);
#endif
	    you_are(buf);
	} else if (u.ustuck) {
	    Sprintf(buf, "%s %s",
		    (Upolyd && sticks(youmonst.data)) ? "holding" : "held by",
		    a_monnam(u.ustuck));
	    you_are(buf);
	}

	/*** Physical attributes ***/
	if (u.uhitinc)
	    you_have(enlght_combatinc("to hit", u.uhitinc, final, buf));
	if (u.udaminc)
	    you_have(enlght_combatinc("damage", u.udaminc, final, buf));
	if (Slow_digestion) you_have("slower digestion");
	if (Regeneration) enl_msg("You regenerate", "", "d", "");
	if (u.uspellprot || Protection) {
	    int prot = 0;

	    if(uleft && uleft->otyp == RIN_PROTECTION) prot += uleft->spe;
	    if(uright && uright->otyp == RIN_PROTECTION) prot += uright->spe;
	    if (HProtection & INTRINSIC) prot += u.ublessed;
	    prot += u.uspellprot;

	    if (prot < 0)
		you_are("ineffectively protected");
	    else
		you_are("protected");
	}
	if (Protection_from_shape_changers)
		you_are("protected from shape changers");
	if (Polymorph) you_are("polymorphing");
	if (Polymorph_control) you_have("polymorph control");
	if (u.ulycn >= LOW_PM) {
		Strcpy(buf, an(mons[u.ulycn].mname));
		you_are(buf);
	}
	if (Upolyd) {
	    if (u.umonnum == u.ulycn) Strcpy(buf, "in beast form");
	    else Sprintf(buf, "polymorphed into %s", an(youmonst.data->mname));
#ifdef WIZARD
	    if (wizard) Sprintf(eos(buf), " (%d)", u.mtimedone);
#endif
	    you_are(buf);
	}
	if (Unchanging) you_can("not change from your current form");
	if (Fast) you_are(Very_fast ? "very fast" : "fast");
	if (Reflecting) you_have("reflection");
	if (Free_action) you_have("free action");
	if (Fixed_abil) you_have("fixed abilities");
	if (Lifesaved)
		enl_msg("Your life ", "will be", "would have been", " saved");
	if (u.twoweap) you_are("wielding two weapons at once");

	/*** Miscellany ***/
	if (Luck) {
	    ltmp = abs((int)Luck);
	    Sprintf(buf, "%s%slucky",
		    ltmp >= 10 ? "extremely " : ltmp >= 5 ? "very " : "",
		    Luck < 0 ? "un" : "");
#ifdef WIZARD
	    if (wizard) Sprintf(eos(buf), " (%d)", Luck);
#endif
	    you_are(buf);
	}
#ifdef WIZARD
	 else if (wizard) enl_msg("Your luck ", "is", "was", " zero");
#endif
	if (u.moreluck > 0) you_have("extra luck");
	else if (u.moreluck < 0) you_have("reduced luck");
	if (carrying(LUCKSTONE) || stone_luck(TRUE)) {
	    ltmp = stone_luck(FALSE);
	    if (ltmp <= 0)
		enl_msg("Bad luck ", "does", "did", " not time out for you");
	    if (ltmp >= 0)
		enl_msg("Good luck ", "does", "did", " not time out for you");
	}

	if (u.ugangr) {
	    Sprintf(buf, " %sangry with you",
		    u.ugangr > 6 ? "extremely " : u.ugangr > 3 ? "very " : "");
#ifdef WIZARD
	    if (wizard) Sprintf(eos(buf), " (%d)", u.ugangr);
#endif
	    enl_msg(u_gname(), " is", " was", buf);
	} else
	    /*
	     * We need to suppress this when the game is over, because death
	     * can change the value calculated by can_pray(), potentially
	     * resulting in a false claim that you could have prayed safely.
	     */
	  if (!final) {
#if 0
	    /* "can [not] safely pray" vs "could [not] have safely prayed" */
	    Sprintf(buf, "%s%ssafely pray%s", can_pray(FALSE) ? "" : "not ",
		    final ? "have " : "", final ? "ed" : "");
#else
	    Sprintf(buf, "%ssafely pray", can_pray(FALSE) ? "" : "not ");
#endif
#ifdef WIZARD
	    if (wizard) Sprintf(eos(buf), " (%d)", u.ublesscnt);
#endif
	    you_can(buf);
	}

    {
	const char *p;

	buf[0] = '\0';
	if (final < 2) {    /* still in progress, or quit/escaped/ascended */
	    p = "survived after being killed ";
	    switch (u.umortality) {
	    case 0:  p = !final ? (char *)0 : "survived";  break;
	    case 1:  Strcpy(buf, "once");  break;
	    case 2:  Strcpy(buf, "twice");  break;
	    case 3:  Strcpy(buf, "thrice");  break;
	    default: Sprintf(buf, "%d times", u.umortality);
		     break;
	    }
	} else {		/* game ended in character's death */
	    p = "are dead";
	    switch (u.umortality) {
	    case 0:  impossible("dead without dying?");
	    case 1:  break;			/* just "are dead" */
	    default: Sprintf(buf, " (%d%s time!)", u.umortality,
			     ordin(u.umortality));
		     break;
	    }
	}
	if (p) enl_msg(You_, "have been killed ", p, buf);
    }

    if (putstr_or_dump == putstr) {
	display_nhwindow(en_win, TRUE);
	destroy_nhwindow(en_win);
    } else {
      putstr_or_dump(0, 0, "-----------------------------------"
                     "-----------------------------------");
    }
    return;
}

STATIC_OVL void
unspoilered_intrinsics()
{
	winid tmpwin;
        int n = 0;
        menu_item* selected;
        anything any;

        any.a_void = 0;
	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);

        /* Intrinsic list
           This lists only intrinsics that produce messages when gained
           and/or lost, to avoid giving away information not in vanilla
           NetHack. */

        /* Resistances */
        if (HFire_resistance) add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
                                       "You are fire resistant.", FALSE), n++;
        if (HCold_resistance) add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
                                       "You are cold resistant.", FALSE), n++;
        if (HSleep_resistance) add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
                                        "You are sleep resistant.", FALSE), n++;
        if (HDisint_resistance) add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
                                         "You are disintegration-resistant.", FALSE), n++;
        if (HShock_resistance) add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
                                        "You are shock resistant.", FALSE), n++;
        if (HPoison_resistance) add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
                                         "You are poison resistant.", FALSE), n++;
        if (HDrain_resistance) add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
                                        "You are level-drain resistant.", FALSE), n++;
        if (HSick_resistance) add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
                                       "You are immune to sickness.", FALSE), n++;
        /* Senses */
        /* See invis is a tricky one, as you can't necessarily
           determine it from messages (e.g. if you quaff an unIDed
           potion of see invis). Still, it's given anyway. */
        if (HSee_invisible) add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
                                     "You see invisible.", FALSE), n++;
        if (HTelepat) add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
                               "You are telepathic.", FALSE), n++;
        if (HWarning) add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
                               "You are warned.", FALSE), n++;
        if (HSearching) add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
                                 "You have automatic searching.", FALSE), n++;
        if (HInfravision) add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
                                   "You have infravision.", FALSE), n++;
        /* Appearance, behaviour */
        if (HInvis && Invisible) add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
                                          "You are invisible.", FALSE), n++;
        if (HInvis && !Invisible) add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
                                           "You are invisible to others.", FALSE), n++;
        if (HStealth) add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
                               "You are stealthy.", FALSE), n++;
        if (HAggravate_monster) add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
                                         "You aggravte monsters.", FALSE), n++;
        if (HConflict) add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
                                "You cause conflict.", FALSE), n++;
        /* Movement */
        if (HJumping) add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
                               "You can jump.", FALSE), n++;
        if (HTeleportation) add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
                               "You can teleport.", FALSE), n++;
        if (HTeleport_control) add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
                               "You have teleport control.", FALSE), n++;
        if (HSwimming) add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
                                "You can swim.", FALSE), n++;
        if (HMagical_breathing) add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
                                         "You can survive without air.", FALSE), n++;
        if (HProtection) add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
                                  "You are protected.", FALSE), n++;
        if (HPolymorph) add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
                                 "You are polymorhing.", FALSE), n++;
        if (HPolymorph_control) add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
                                         "You have polymorph control.", FALSE), n++;
        if (HFast) add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
                            "You are fast.", FALSE), n++;
        if (!n) add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
                         "You have no intrinsic abilities.", FALSE), n++;

	end_menu(tmpwin, "Your Intrinsic Abilities");
	n = select_menu(tmpwin, PICK_NONE, &selected);
	destroy_nhwindow(tmpwin);
}

STATIC_PTR int
doattributes()
{
	winid tmpwin;
	menu_item *selected;
	anything any;
	int genidx, n, r, wcu;
        long wc;
	char buf[BUFSZ], buf2[BUFSZ];
	static const char fmtstr[] = "%-10s: %-12s (originally %s)";
	static const char fmtstr_noorig[] = "%-10s: %s";
	static const char deity_fmtstr[] = "%-17s%s";
	any.a_void = 0;
	buf[0] = buf2[0] = '\0';
	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, iflags.menu_headings, "Stats", FALSE);

	/* Starting and current name, race, role, gender, alignment, abilities */
	Sprintf(buf, fmtstr_noorig, "name", plname);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	Sprintf(buf, fmtstr, "race", Upolyd ? youmonst.data->mname : urace.noun, urace.noun);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	Sprintf(buf, fmtstr_noorig, "role",
		((Upolyd ? u.mfemale : flags.female) && urole.name.f) ?
                urole.name.f : urole.name.m);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	genidx = is_neuter(youmonst.data) ? 2 : flags.female;
	Sprintf(buf, fmtstr, "gender", genders[genidx].adj, genders[flags.initgend].adj);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	Sprintf(buf, fmtstr, "alignment", align_str(u.ualign.type),
                align_str(u.ualignbase[A_ORIGINAL]));
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
        if (u.ulevel < 30)
          Sprintf(buf, "%-10s: %d (exp: %ld, %ld needed)", "level",
                  u.ulevel, u.uexp, newuexp(u.ulevel));
        else
          Sprintf(buf, "%-10s: %d (exp: %ld)", "level", u.ulevel, u.uexp);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
        if (ACURR(A_STR) > 18) {
          if (ACURR(A_STR) > STR18(100))
            Sprintf(buf,"abilities : St:%2d ",ACURR(A_STR)-100);
          else if (ACURR(A_STR) < STR18(100))
            Sprintf(buf, "abilities : St:18/%02d ",ACURR(A_STR)-18);
          else
            Sprintf(buf,"abilities : St:18/** ");
	} else
          Sprintf(buf, "abilities : St:%-1d ",ACURR(A_STR));
	Sprintf(eos(buf), "Dx:%-1d Co:%-1d In:%-1d Wi:%-1d Ch:%-1d",
		ACURR(A_DEX), ACURR(A_CON), ACURR(A_INT), ACURR(A_WIS), ACURR(A_CHA));
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
        if (AMAX(A_STR) > 18) {
          if (AMAX(A_STR) > STR18(100))
            Sprintf(buf,"naturally : St:%2d ",AMAX(A_STR)-100);
          else if (AMAX(A_STR) < STR18(100))
            Sprintf(buf, "naturally : St:18/%02d ",AMAX(A_STR)-18);
          else
            Sprintf(buf,"naturally : St:18/** ");
	} else
          Sprintf(buf, "naturally : St:%-1d ",AMAX(A_STR));
	Sprintf(eos(buf), "Dx:%-1d Co:%-1d In:%-1d Wi:%-1d Ch:%-1d",
		AMAX(A_DEX), AMAX(A_CON), AMAX(A_INT), AMAX(A_WIS), AMAX(A_CHA));

        wc = weight_cap();
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
        Sprintf(buf, "burden    : %ld (", wc + inv_weight());
        /* We want to show the /nearest/ capacity breakpoint, which involves
           checking capacity to a slightly higher resolution than near_capacity
           does. So we use calc_capacity instead, and give it 1/4 the weightcap
           as argument; this bumps up the result half a burden level. */
        switch (calc_capacity(wc/4)) {
        case UNENCUMBERED:
        case SLT_ENCUMBER: Sprintf(eos(buf), "burdened at "); wcu = 2; break;
        case MOD_ENCUMBER: Sprintf(eos(buf), "stressed at "); wcu = 3; break;
        case HVY_ENCUMBER: Sprintf(eos(buf), "strained at "); wcu = 4; break;
        case EXT_ENCUMBER: Sprintf(eos(buf), "overtaxed at "); wcu = 5; break;
        default:           Sprintf(eos(buf), "overloaded at "); wcu = 6; break;
        }
        Sprintf(eos(buf), "%ld)", wc * wcu / 2 + 1);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);

	/* Deity list */
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, iflags.menu_headings, "Deities", FALSE);
	Sprintf(buf2, deity_fmtstr, align_gname(A_CHAOTIC),
	    (u.ualignbase[A_ORIGINAL] == u.ualign.type
		&& u.ualign.type == A_CHAOTIC) ? " (s,c)" :
	    (u.ualignbase[A_ORIGINAL] == A_CHAOTIC)       ? " (s)" :
	    (u.ualign.type   == A_CHAOTIC)       ? " (c)" : "");
	Sprintf(buf, fmtstr_noorig, "Chaotic", buf2);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);

	Sprintf(buf2, deity_fmtstr, align_gname(A_NEUTRAL),
	    (u.ualignbase[A_ORIGINAL] == u.ualign.type
		&& u.ualign.type == A_NEUTRAL) ? " (s,c)" :
	    (u.ualignbase[A_ORIGINAL] == A_NEUTRAL)       ? " (s)" :
	    (u.ualign.type   == A_NEUTRAL)       ? " (c)" : "");
	Sprintf(buf, fmtstr_noorig, "Neutral", buf2);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);

	Sprintf(buf2, deity_fmtstr, align_gname(A_LAWFUL),
	    (u.ualignbase[A_ORIGINAL] == u.ualign.type &&
		u.ualign.type == A_LAWFUL)  ? " (s,c)" :
	    (u.ualignbase[A_ORIGINAL] == A_LAWFUL)        ? " (s)" :
	    (u.ualign.type   == A_LAWFUL)        ? " (c)" : "");
	Sprintf(buf, fmtstr_noorig, "Lawful", buf2);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);

        /* A list of submenus, based on the DYWYPI? at end of game. */
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, iflags.menu_headings,
                 "Other Information", FALSE);
        any.a_int = 'i';
        add_menu(tmpwin, NO_GLYPH, &any, 'i', 0, ATR_NONE,
                 "Inventory", MENU_UNSELECTED);
        any.a_int = 'a';
        add_menu(tmpwin, NO_GLYPH, &any, 'a', 0, ATR_NONE,
                 "Intrinsic abilities", MENU_UNSELECTED);
        any.a_int = 'v';
        add_menu(tmpwin, NO_GLYPH, &any, 'v', 0, ATR_NONE,
                 "Vanquished creatures", MENU_UNSELECTED);
        if (num_genocides() > 0) {
            any.a_int = 'g';
            add_menu(tmpwin, NO_GLYPH, &any, 'g', 0, ATR_NONE,
                     "Genocided creatures", MENU_UNSELECTED);
        }
        any.a_int = 'c';
        add_menu(tmpwin, NO_GLYPH, &any, 'c', 0, ATR_NONE,
                 "Conducts followed", MENU_UNSELECTED);
        any.a_int = 's';
        add_menu(tmpwin, NO_GLYPH, &any, 's', 0, ATR_NONE,
                 "Score breakdown", MENU_UNSELECTED);
        if (wizard || discover) {
            any.a_int = 'w';
            add_menu(tmpwin, NO_GLYPH, &any, 'w', 0, ATR_NONE,
                     "Wizard/Explore mode spoilers", MENU_UNSELECTED);
        }

	end_menu(tmpwin, "Your Statistics");
	n = select_menu(tmpwin, PICK_ONE, &selected);
	destroy_nhwindow(tmpwin);

        r = 0;

        if (n == 1) switch(selected[0].item.a_int) {
        case 'i': r = ddoinv(); break;
        case 'a': unspoilered_intrinsics(); break;
        case 'v': list_vanquished('Y', FALSE); break;
        case 'g': list_genocided('y', FALSE); break;
        case 'c': r = doconduct(); break;
        case 's': calculate_score(-1,TRUE,
#ifndef GOLDOBJ
                                  u.ugold
#else
                                  money_cnt(invent)
#endif
                                  + hidden_gold()); break;
        case 'w': enlightenment(0);
        }
        if (n == 1) free((genericptr_t) selected);
	return r;
}

/* KMH, #conduct
 * (shares enlightenment's tense handling)
 */
STATIC_PTR int
doconduct()
{
	show_conduct(0);
	return 0;
}

void
show_conduct(final)
int final;
{
	char buf[BUFSZ];
	int ngenocided;

        if (!putstr_or_dump) putstr_or_dump = putstr;

	/* Create the conduct window */
	if (putstr_or_dump == putstr) en_win = create_nhwindow(NHW_MENU);
	putstr_or_dump(en_win, 0, "Voluntary challenges:");
	putstr_or_dump(en_win, 0, "");

	if (heaven_or_hell_mode) {
		if (hell_and_hell_mode) {
			you_have_been("following the path of Hell and Hell");
		}
		else
			you_have_been("following the path of Heaven or Hell");
	}

	if (!u.uconduct.food)
	    enl_msg(You_, "have gone", "went", " without food");
	    /* But beverages are okay */
	else if (!u.uconduct.unvegan)
	    you_have_X("followed a strict vegan diet");
	else if (!u.uconduct.unvegetarian)
	    you_have_been("vegetarian");

	if (!u.uconduct.gnostic)
	    you_have_been("an atheist");

	if (!u.uconduct.weaphit)
	    you_have_never("hit with a wielded weapon");
	else {
	    Sprintf(buf, "used a wielded weapon %ld time%s",
		    u.uconduct.weaphit, plur(u.uconduct.weaphit));
	    you_have_X(buf);
	}
	if (!u.uconduct.killer)
	    you_have_been("a pacifist");

	if (!u.uconduct.literate)
	    you_have_been("illiterate");
	else {
	    Sprintf(buf, "wrote or read %ld time%s",
		    u.uconduct.literate, plur(u.uconduct.literate));
	    you_have_X(buf);
	}

	if (!u.uconduct.heptagrams)
	    enl_msg(You_, "have never drawn", "never drew", " a magic heptagram");
	else {
	    Sprintf(buf, " a magic heptagram %ld time%s",
		    u.uconduct.heptagrams, plur(u.uconduct.heptagrams));
	    enl_msg(You_, "have drawn", "drew", buf);
	}

	ngenocided = num_genocides();
	if (ngenocided == 0) {
	    you_have_never("genocided any monsters");
	} else {
	    Sprintf(buf, "genocided %d type%s of monster%s",
		    ngenocided, plur(ngenocided), plur(ngenocided));
	    you_have_X(buf);
	}

	if (!u.uconduct.polypiles)
	    you_have_never("polymorphed an object");
	else {
	    Sprintf(buf, "polymorphed %ld item%s",
		    u.uconduct.polypiles, plur(u.uconduct.polypiles));
	    you_have_X(buf);
	}

	if (!u.uconduct.polyselfs)
	    you_have_never("changed form");
	else {
	    Sprintf(buf, "changed form %ld time%s",
		    u.uconduct.polyselfs, plur(u.uconduct.polyselfs));
	    you_have_X(buf);
	}

	if (!u.uconduct.wishes)
	    you_have_X("used no wishes");
	else {
	    Sprintf(buf, "used %ld wish%s",
		    u.uconduct.wishes, (u.uconduct.wishes > 1L) ? "es" : "");
	    you_have_X(buf);

	    if (!u.uconduct.wisharti)
		enl_msg(You_, "have not wished", "did not wish",
			" for any artifacts");
	}

        if (solo)
            enl_msg(You_, "are", "were", " playing solo");
        if (iflags.multiplayer)
            enl_msg(You_, "have", "had", " human-controlled allies");

        if (putstr_or_dump == putstr) {
            display_nhwindow(en_win, TRUE);
            destroy_nhwindow(en_win);
        } else {
          putstr_or_dump(0, 0, "-----------------------------------"
                         "-----------------------------------");
        }
}

unsigned long
encode_conduct()
{
  unsigned long c = 0UL;

  if (!u.uconduct.food)         c |= 0x0001UL;
  if (!u.uconduct.unvegan)      c |= 0x0002UL;
  if (!u.uconduct.unvegetarian) c |= 0x0004UL;
  if (!u.uconduct.gnostic)      c |= 0x0008UL;
  if (!u.uconduct.weaphit)      c |= 0x0010UL;
  if (!u.uconduct.killer)       c |= 0x0020UL;
  if (!u.uconduct.literate)     c |= 0x0040UL;
  /* heptagrams, genocides are given a higher number later on to avoid
     clashing with the "traditional" conduct encoding */
  if (!u.uconduct.polypiles)    c |= 0x0080UL;
  if (!u.uconduct.polyselfs)    c |= 0x0100UL;
  if (!u.uconduct.wishes)       c |= 0x0200UL;
  if (!u.uconduct.wisharti)     c |= 0x0400UL;
  if (!num_genocides())         c |= 0x0800UL;
  /* Slash'EM xlogfile does not record celibacy, presumably either by
     mistake or for compatibility with vanilla. So it's safe to just
     take the next available number for heptagrams. */
  if (!u.uconduct.heptagrams)   c |= 0x1000UL;

  return c;
}

#endif /* OVLB */
#ifdef OVL1

#ifndef M
# ifndef NHSTDC
#  define M(c)		(0x80 | ((unsigned char)c))
# else
#  define M(c)		(((unsigned char)c) - (unsigned char)128)
# endif /* NHSTDC */
#endif
#ifndef C
#define C(c)		(0x1f & ((unsigned char)c))
#endif

struct ext_func_tab extcmdlist[] = {
  {"adjust", "adjust inventory letters", doorganize, TRUE, 1, C('i'), M('a'), 0, 0},
  {"annotate", "name current level", donamelevel, TRUE, 5, C('f'), 0, 0, 0},
  {"apply", "use a tool or dip into a potion", doapply, FALSE, 12, 'a', 0, 0, 0},
  {"autoexplore", "travel somewhere interesting", doautoexplore, FALSE,
   11, 'v', 0, 0, 0},
  {"autopickup", "toggle the autopickup option", dotogglepickup, TRUE, 10,
   '@', 0, 0, 0},
  {"cast", "cast a spell from memory", docast, TRUE, 11, 'Z', 0, 0, 0},
  {"chat", "talk to someone or ride a monster", dotalk, TRUE, 1, 'c', 'p', M('c'), 0},
  {"conduct", "list which challenges you have adhered to", doconduct,
   TRUE, 2, 0, 0, 0, 0},
  {"countgold", "show gold, debt, credit, and unpaid items", doprgold, TRUE,
   10, '$', 0, 0, 0},
  {"describeitem", "show partial inventory or describe an item",
   dotypeinv, TRUE, 10, 'I', M('0'), 0, 0},
  {"discoveries", "show your knowledge about items", dodiscovered, TRUE, 11,
   '\\', 0, 0, 0},
  {"dip", "dip an object into something", dodip, FALSE, 1, M('d'), 0, 0, 0},
  {"drop", "drop many items", doddrop, FALSE, 12, 'D', 0, 0, 0},
  {"dropone", "drop one item", dodrop, FALSE, 13, 'd', 0, 0, 0},
  {"donothing", "do nothing, consuming no time", donull0, FALSE, 15, 0, 0, 0, 0},
  {"east", "move, attack, or interact east", 0, FALSE, 21, 'l', '6', 0, 'l'},
  {"eastfar", "move east as far as possible", 0, FALSE, 22, 'L', 0, 0, 'L'},
  {"eastfarcareful", "move east until something interesting happens",
   0, FALSE, 23, C('l'), 0, 0, C('l')},
  {"eastmove", "move east unless it would attack", 0, FALSE, 23,
   0, 0, 0, M(C('l'))},
  {"eat", "eat an item from inventory or the floor", doeat, FALSE, 10, 'e', 0, 0, 0},
  {"engrave", "write on the floor", doengrave, FALSE, 11, 'E', 0, 0, 0},
  {"enhance", "advance or check weapons skills", enhance_weapon_skill,
   TRUE, 1, C('e'), M('e'), 0, 0},
  {"equip", "wear or put on equipment", dowear, FALSE, 10, 'W', 'P', 0, 0},
  {"equipment", "show, equip or unequip equipped items",
   doddoremarm, FALSE, 10, 'A', '*', 0, 0},
  {"farlook", "say what is on a distant square", doquickwhatis, TRUE, 10,
   ';', 0, 0, 0},
  {"farmove", "move repeatedly, stopping for anything interesting", 0, TRUE, 20,
   'g', '5', M('5'), 'g'},
  {"fight", "attack even if no hostile monster is visible", 0, TRUE, 21,
   'F', '-', 0, 'F'},
  {"fire", "throw your quivered item", dofire, FALSE, 11, 'f', 0, 0, 0},
  {"force", "force a lock", doforce, FALSE, 1, M('f'), 0, 0, 0},
  {"goup", "move up stairs or a ladder", doup, FALSE, 10, '<', 0, 0, 0},
  {"godown", "move down stairs or a ladder", dodown, FALSE, 11, 0, 0, 0, 0},
  {"help", "open the in-game help", dohelp, TRUE, 10, '?', 0, 0, 0},
  {"heptagram", "draw or repair a heptagram with fingers",
   doheptagram, FALSE, 11, '.', 0, 0},
  {"inventory", "list, describe or use items", ddoinv, TRUE, 10, 'i', 0, 0, 0},
  {"invoke", "use artifact powers, or break/ignite/rub on an item",
   doinvoke, TRUE, 1, 'V', 0, 0, 0},
  {"invite", "allow another player to join your game", doinvite, TRUE, 5, 0, 0, 0, 0},
  {"jump", "jump, teleport, ride, or dismount", dojump, FALSE, 1, 'G', M('j'), 0, 0},
  {"kick", "kick an adjacent object or monster", dokick, FALSE, 10, C('d'), 0, 0, 0},
  {"lookhere", "describe the current square", dolook, TRUE, 10, ':', 0, 0, 0},
  {"loot", "loot a box on the floor", doloot, FALSE, 1, 0, 0, 0, 0},
  {"messages", "show message history", doprev_message, TRUE, 10, C('p'), 0, 0, 0},
  {"monster", "use a monster's special ability", domonability, TRUE, 1,
   'M', M('m'), 0, 0},
  {"move", "move even if something's in the way", 0, TRUE, 20, 'm', 0, 0, 'm'},
  {"name", "name a monster, item or type of object", ddocall, TRUE, 1,
   'C', M('n'), M('N'), 0},
  {"north", "move, attack, or interact north", 0, FALSE, 21, 'k', '8', 0, 'k'},
  {"northfar", "move south as far as possible", 0, FALSE, 22, 'K', 0, 0, 'K'},
  {"northfarcareful", "move south until something interesting happens",
   0, FALSE, 23, C('k'), 0, 0, C('k')},
  {"northmove", "move north unless it would attack", 0, FALSE, 23,
   0, 0, 0, M(C('k'))},
  {"northeast", "move, attack, or interact northeast", 0, FALSE, 21, 'u', '9', 0, 'u'},
  {"northeastfar", "move northeast as far as possible", 0, FALSE, 22, 'U', 0, 0, 'U'},
  {"northeastfarcareful", "move northeast until something interesting happens",
   0, FALSE, 23, C('u'), 0, 0, C('u')},
  {"northeastmove", "move northeast unless it would attack", 0, FALSE, 23,
   0, 0, 0, M(C('u'))},
  {"northwest", "move, attack, or interact northwest", 0, FALSE, 21, 'y', '7', 0, 'y'},
  {"northwestfar", "move northwest as far as possible", 0, FALSE, 22, 'Y', 0, 0, 'Y'},
  {"northwestfarcareful", "move northwest until something interesting happens",
   0, FALSE, 23, C('y'), 0, 0, C('y')},
  {"northwestmove", "move northwest unless it would attack", 0, FALSE, 23,
   0, 0, 0, M(C('y'))},
  {"offer", "offer a sacrifice to the gods", dosacrifice, FALSE, 1, M('o'), 0, 0, 0},
  {"open", "open, close, or unlock a door", doopen, FALSE, 11, 'o', M('l'), 0, 0},
  {"options", "change game options", doset, TRUE, 10, 'O', 0, 0, 0},
  {"overview", "show an overview of the dungeon", dooverview, TRUE, 5, C('o'), 0, 0, 0},
  {"pickup", "pick up one or more items", dopickup, FALSE, 10, ',', 0, 0, 0},
  {"pray", "pray to the gods for help", dopray, TRUE, 1, M('p'), 0, 0, 0},
  {"quaff", "drink a potion or other liquid", dodrink, FALSE, 11, 'q', 0, 0, 0},
  {"quit", "exit the game and delete its savefile", done2, TRUE, 1, M('q'), 0, 0, 0},
  {"quiver", "ready an item for firing", dowieldquiver, FALSE, 10, 'Q', 0, 0, 0},
  {"read", "read text written on an item", doread, FALSE, 11, 'r', 0, 0, 0},
  {"redo", "repeat the previous command", 0, TRUE, 20, C('a'), 0, 0, DOAGAIN},
  {"redraw", "redraw the screen", doredraw, TRUE, 10, C('r'), 0, 0, 0},
  {"repeat", "run a command more than once or by name", doextcmd,
   TRUE, 11, '#', 0, 0, 0},
#ifdef STEED
  {"ride", "ride (or stop riding) a monster", doride, FALSE, 2, 0, 0, 0, 0},
#endif
  {"rub", "rub a lamp or a stone", dorub, FALSE, 1, M('r'), 0, 0, 0},
  {"save", "quicksave or abandon the game", dosave, TRUE, 10, 'S', 0, 0, 0},
  {"search", "wait 1 turn, searching around you", dosearch, TRUE, 13, 's', 0, 0},
  {"sit", "sit down", dosit, FALSE, 1, M('s'), 0, 0, 0},
  {"south", "move, attack, or interact south", 0, FALSE, 21, 'j', '2', 0, 'j'},
  {"southfar", "move south as far as possible", 0, FALSE, 22, 'J', 0, 0, 'J'},
  {"southfarcareful", "move south until something interesting happens",
   0, FALSE, 23, C('j'), 0, 0, C('j')},
  {"southmove", "move south unless it would attack", 0, FALSE, 23,
   0, 0, 0, M(C('j'))},
  {"southeast", "move, attack, or interact southeast", 0, FALSE, 21, 'n', '3', 0, 'n'},
  {"southeastfar", "move southeast as far as possible", 0, FALSE, 22, 'N', 0, 0,
   'N'},
  {"southeastfarcareful", "move southeast until something interesting happens",
   0, FALSE, 23, C('n'), 0, 0, C('n')},
  {"southeastmove", "move southeast unless it would attack", 0, FALSE, 23,
   0, 0, 0, M(C('n'))},
  {"southwest", "move, attack, or interact southwest", 0, FALSE, 21, 'b', '1', 0, 'b'},
  {"southwestfar", "move southwest as far as possible", 0, FALSE, 22, 'B', 0, 0,
   'B'},
  {"southwestfarcareful", "move southwest until something interesting happens",
   0, FALSE, 23, C('b'), 0, 0, C('b')},
  {"southwestmove", "move southwest unless it would attack", 0, FALSE, 23,
   0, 0, 0, M(C('b'))},
  {"spellbook", "display and change letters of spells", dovspell, TRUE,
   11, '+', 0, 0, 0},
  {"stats", "show your statistics and intrinsics", doattributes, TRUE,
   8, C('x'), 0, 0, 0},
#ifdef SUSPEND
  {"suspend", "pause and background AceHack",
   dosuspend, TRUE, 5, C('z'), 0, 0, 0},
#endif
  {"swapweapons", "exchange wielded and alternate weapon", doswapweapon, FALSE, 12,
   'x', 0, 0, 0},
  {"teleport", "use intrinsic or magical teleportation ability", dotele, TRUE, 11,
   C('t'), 0, 0, 0},
  {"terrain", "interact with the terrain below your feet",
   doterrain, FALSE, 13, '>', 0, 0, 0},
  {"throw", "throw an item", dothrow, FALSE, 14, 't', 0, 0, 0},
  {"travel", "walk until a given square is reached", dotravel, TRUE,
   12, '_', 0, 0, 0},
  {"turn", "turn undead", doturn, TRUE, 2, M('t'), 0, 0, 0},
#ifdef WIZARD
  /* Key clash on ^V. wiz_level_tele does the right thing. */
  {"tutorial", "redisplay tutorial slides", wiz_level_tele, FALSE, 10,
   C('v'), 0, 0, 0},
#else
  {"tutorial", "redisplay tutorial slides", tutorial_redisplay, FALSE, 10,
   C('v'), 0, 0, 0},
#endif
  {"twoweapon", "toggle two-weapon combat", dotwoweapon, FALSE, 1, 'X', M('2'), 0, 0},
  {"unequip", "take off or remove equipment", dotakeoff, FALSE, 10, 'T', 'R', 0, 0},
  {"untrap", "describe or defuse a remembered trap", dountrap, FALSE, 1,
   '^', M('u'), 0, 0},
  {"versionshort", "list the version number of AceHack",
   doversion, TRUE, 10, 0, 0, 0, 0},
  {"versionhistory", "list the history of AceHack",
   dohistory, TRUE, 11, M('v'), 0, 0, 0},
  {"versionoptions", "list compile time options", doextversion, TRUE, 1, 0, 0, 0, 0},
  {"wait", "do nothing for one turn", donull, FALSE, 15, 0, 0, 0, 0},
  {"west", "move, attack, or interact west", 0, FALSE, 21, 'h', '4', 0, 'h'},
  {"westfar", "move west as far as possible", 0, FALSE, 22, 'H', 0, 0, 'H'},
  {"westfarcareful", "move west until something interesting happens",
   0, FALSE, 23, C('h'), 0, 0, C('h')},
  {"westmove", "move west unless it would attack", 0, FALSE, 23,
   0, 0, 0, M(C('h'))},
  {"whatis", "describe what a symbol means", dowhatis, TRUE, 10, '/', 0, 0, 0},
  {"whatdoes", "describe what a key does", dowhatdoes, TRUE, 10, '&', 0, 0, 0},
  {"wield", "hold an item in your hands", dowield, FALSE, 11, 'w', 0, 0, 0},
  {"wipe", "wipe off your face", dowipe, FALSE, 1, M('w'), 0, 0, 0},
#if defined(WIZARD)
#endif
  {"worship", "communicate with the gods", doworship, TRUE, 10, C('w'), 0, 0, 0},
  {"zap", "zap a wand to use its magic", dozap, FALSE, 10, 'z', 0, 0, 0},
  {"?", "get this list of extended commands", doextlist, TRUE, 0, 0, 0, 0, 0},
#if defined(WIZARD)
	/*
	 * There must be a blank entry here for every entry in the table
	 * below.
	 */
   {(char *)0, (char *)0, donull, TRUE, 0, 0, 0, 0, 0},
   {(char *)0, (char *)0, donull, TRUE, 0, 0, 0, 0, 0},
#ifdef DEBUG_MIGRATING_MONS
   {(char *)0, (char *)0, donull, TRUE, 0, 0, 0, 0, 0},
#endif
   {(char *)0, (char *)0, donull, TRUE, 0, 0, 0, 0, 0},
   {(char *)0, (char *)0, donull, TRUE, 0, 0, 0, 0, 0},
   {(char *)0, (char *)0, donull, TRUE, 0, 0, 0, 0, 0},
#ifdef PORT_DEBUG
   {(char *)0, (char *)0, donull, TRUE, 0, 0, 0, 0, 0},
#endif
   {(char *)0, (char *)0, donull, TRUE, 0, 0, 0, 0, 0},
   {(char *)0, (char *)0, donull, TRUE, 0, 0, 0, 0, 0},
   {(char *)0, (char *)0, donull, TRUE, 0, 0, 0, 0, 0},
   {(char *)0, (char *)0, donull, TRUE, 0, 0, 0, 0, 0},
   {(char *)0, (char *)0, donull, TRUE, 0, 0, 0, 0, 0},
   {(char *)0, (char *)0, donull, TRUE, 0, 0, 0, 0, 0},
   {(char *)0, (char *)0, donull, TRUE, 0, 0, 0, 0, 0},
   {(char *)0, (char *)0, donull, TRUE, 0, 0, 0, 0, 0},
   {(char *)0, (char *)0, donull, TRUE, 0, 0, 0, 0, 0},
   {(char *)0, (char *)0, donull, TRUE, 0, 0, 0, 0, 0},
   {(char *)0, (char *)0, donull, TRUE, 0, 0, 0, 0, 0},
#ifdef DEBUG
   {(char *)0, (char *)0, donull, TRUE, 0, 0, 0, 0, 0},
#endif
   {(char *)0, (char *)0, donull, TRUE, 0, 0, 0, 0, 0},
#endif
   {(char *)0, (char *)0, donull, TRUE, 0, 0, 0, 0, 0}	/* sentinel */
};

#if defined(WIZARD)
static const struct ext_func_tab debug_extcmdlist[] = {
	{"levelchange", "(debug mode) change experience level", wiz_level_change, TRUE},
	{"lightsources", "(debug mode) show mobile light sources", wiz_light_sources, TRUE},
#ifdef DEBUG_MIGRATING_MONS
	{"migratemons", "(debug mode) migrate n random monsters", wiz_migrate_mons, TRUE},
#endif
	{"monpolycontrol", "(debug mode) control monster polymorphs", wiz_mon_polycontrol, TRUE},
	{"panic", "(debug mode) test panic routine (fatal to game)", wiz_panic, TRUE},
	{"polyself", "(debug mode) polymorph self", wiz_polyself, TRUE},
#ifdef PORT_DEBUG
	{"portdebug", "(debug mode) wizard port debug command", wiz_port_debug, TRUE},
#endif
	{"seenv", "(debug mode) show seen vectors", wiz_show_seenv, TRUE},
	{"stats", "(debug mode) show memory statistics", wiz_show_stats, TRUE},
	{"timeout", "(debug mode) look at timeout queue", wiz_timeout_queue, TRUE},
	{"vision", "(debug mode) show vision array", wiz_show_vision, TRUE},
	{"yield", "(debug mode) test multiplayer yield routines",
         wiz_multiplayer_yield, TRUE},
#ifdef DEBUG
	{"wizdebug", "(debug mode) wizard debug command", wiz_debug_cmd, TRUE},
#endif
	{"wmode", "(debug mode) show wall modes", wiz_show_wmodes, TRUE},
        {"detect", "(debug mode) detect secret doors/traps", wiz_detect, TRUE},
        {"map", "(debug mode) map this level", wiz_map, TRUE},
        {"genesis", "(debug mode) create a monster", wiz_genesis, TRUE},
        {"identify", "(debug mode) identify inventory", wiz_identify, TRUE},
        {"where", "(debug mode) show dungeon layout", wiz_where, TRUE},
        {"wish", "(debug mode) create an item", wiz_wish, TRUE},
	{(char *)0, (char *)0, donull, TRUE}
};

#define unctrl(c)	((unsigned char)(c) <= (unsigned char)C('z') && \
                         c != 0 ? (0x60 | (c)) : (c))
#define unmeta(c)	(0x7f & (c))

char*
key_for_cmd(cmd)
char* cmd;
{
  static char buf[8];
  struct ext_func_tab *a = extcmdlist;
  while (a->ef_txt) {
    if (!strcmp(a->ef_txt,cmd+1)) {
      buf[1] = 0;
      if (isprint(a->binding1)) {*buf=a->binding1; return buf;}
      if (isprint(a->binding2)) {*buf=a->binding2; return buf;}
      if (isprint(a->binding3)) {*buf=a->binding3; return buf;}
      buf[0] = '^';
      buf[2] = 0;
      if (isprint(unctrl(a->binding1)))
        {buf[1]=highc(unctrl(a->binding1)); return buf;}
      if (isprint(unctrl(a->binding2)))
        {buf[1]=highc(unctrl(a->binding2)); return buf;}
      if (isprint(unctrl(a->binding3)))
        {buf[1]=highc(unctrl(a->binding3)); return buf;}
      return cmd;
    }
    a++;
  }
  return cmd;
}

/* Unlike key_for_cmd, this doesn't use the leading #, because it's
   likely to be called with varying data rather than a literal. */
const char*
desc_for_cmd(cmd)
const char* cmd;
{
  struct ext_func_tab *a = extcmdlist;
  while (a->ef_txt) {
    if (!strcmp(a->ef_txt,cmd))
      return a->ef_desc;
    a++;
  }
  return 0;
}

const char*
cmd_for_key(c)
char c;
{
  struct ext_func_tab *tlist;
  for (tlist = extcmdlist; tlist->ef_txt; tlist++) {
    if ((c & 0xff) == (tlist->binding1 & 0xff) ||
        (c & 0xff) == (tlist->binding2 & 0xff) ||
        (c & 0xff) == (tlist->binding3 & 0xff))
      return tlist->ef_txt;
  }
  return 0;
}

/* Rebinds key c to command s (as a #command string without the #).
   Silently fails if the command in question doesn't exist. */
void
rebind_key(c,s)
char c;
char* s;
{
  struct ext_func_tab *tlist;
  /* Reject attempts to rebind #, so that all commands are always typable */
  if (c == '#') return;
  /* First find the command (if any) using c, and unbind it */
  for (tlist = extcmdlist; tlist->ef_txt; tlist++) {
    if ((c & 0xff) == (tlist->binding1 & 0xff)) {
      tlist->binding1 = tlist->binding2;
      tlist->binding2 = tlist->binding3;
      tlist->binding3 = 0;
    } else if ((c & 0xff) == (tlist->binding2 & 0xff)) {
      tlist->binding2 = tlist->binding3;
      tlist->binding3 = 0;
    } else if ((c & 0xff) == (tlist->binding3 & 0xff)) {
      tlist->binding3 = 0;
    }
  }
  /* Now find the command named #s, and bind that
     binding1 is the most "important" binding, so we should remove
     binding3 first if there's a conflict, and insert the new binding
     at binding1 so that it's displayed in help text, and it's removed
     last if further bindings are done in the config file or via O. */
  for (tlist = extcmdlist; tlist->ef_txt; tlist++) {
    if (!strcmp(tlist->ef_txt, s)) {
      tlist->binding3 = tlist->binding2;
      tlist->binding2 = tlist->binding1;
      tlist->binding1 = c;
    }
  }
}

/* Rebind a key based on a config file specification:
   a:#apply
   ^E:#enhance
   M-d:#dip
   Silently fails on bad syntax
*/
void
rebind_key_from_string(s)
char* s;
{
  char* r = strstr(s, ":#");
  if (!r || r-s < 1 || r-s > 3) return;
  else if (r-s == 1) rebind_key(s[0], s+3);
  else if (r-s == 2 && *s == '^') rebind_key(C(s[1]), s+4);
  else if (r-s == 3 && *s == 'M' && s[1] == '-') rebind_key(M(s[1]), s+5);
}

/*
 * Insert debug commands into the extended command list.  This function
 * assumes that the last entry will be the help entry.
 *
 * You must add entries in ext_func_tab every time you add one to the
 * debug_extcmdlist().
 */
void
add_debug_extended_commands()
{
	int i, j, k, n;

	/* count the # of help entries */
	for (n = 0; extcmdlist[n].ef_txt[0] != '?'; n++)
	    ;

	for (i = 0; debug_extcmdlist[i].ef_txt; i++) {
	    for (j = 0; j < n; j++)
		if (strcmp(debug_extcmdlist[i].ef_txt, extcmdlist[j].ef_txt) < 0) break;

	    /* insert i'th debug entry into extcmdlist[j], pushing down  */
	    for (k = n; k >= j; --k)
		extcmdlist[k+1] = extcmdlist[k];
	    extcmdlist[j] = debug_extcmdlist[i];
	    n++;	/* now an extra entry */
	}
}


static const char template[] = "%-18s %4ld  %6ld";
static const char count_str[] = "                   count  bytes";
static const char separator[] = "------------------ -----  ------";

STATIC_OVL void
count_obj(chain, total_count, total_size, top, recurse)
	struct obj *chain;
	long *total_count;
	long *total_size;
	boolean top;
	boolean recurse;
{
	long count, size;
	struct obj *obj;

	for (count = size = 0, obj = chain; obj; obj = obj->nobj) {
	    if (top) {
		count++;
		size += sizeof(struct obj) + obj->oxlth + obj->onamelth;
	    }
	    if (recurse && obj->cobj)
		count_obj(obj->cobj, total_count, total_size, TRUE, TRUE);
	}
	*total_count += count;
	*total_size += size;
}

STATIC_OVL void
obj_chain(win, src, chain, total_count, total_size)
	winid win;
	const char *src;
	struct obj *chain;
	long *total_count;
	long *total_size;
{
	char buf[BUFSZ];
	long count = 0, size = 0;

	count_obj(chain, &count, &size, TRUE, FALSE);
	*total_count += count;
	*total_size += size;
	Sprintf(buf, template, src, count, size);
	putstr(win, 0, buf);
}

STATIC_OVL void
mon_invent_chain(win, src, chain, total_count, total_size)
	winid win;
	const char *src;
	struct monst *chain;
	long *total_count;
	long *total_size;
{
	char buf[BUFSZ];
	long count = 0, size = 0;
	struct monst *mon;

	for (mon = chain; mon; mon = mon->nmon)
	    count_obj(mon->minvent, &count, &size, TRUE, FALSE);
	*total_count += count;
	*total_size += size;
	Sprintf(buf, template, src, count, size);
	putstr(win, 0, buf);
}

STATIC_OVL void
contained(win, src, total_count, total_size)
	winid win;
	const char *src;
	long *total_count;
	long *total_size;
{
	char buf[BUFSZ];
	long count = 0, size = 0;
	struct monst *mon;

	count_obj(invent, &count, &size, FALSE, TRUE);
	count_obj(fobj, &count, &size, FALSE, TRUE);
	count_obj(level.buriedobjlist, &count, &size, FALSE, TRUE);
	count_obj(migrating_objs, &count, &size, FALSE, TRUE);
	/* DEADMONSTER check not required in this loop since they have no inventory */
	for (mon = fmon; mon; mon = mon->nmon)
	    count_obj(mon->minvent, &count, &size, FALSE, TRUE);
	for (mon = migrating_mons; mon; mon = mon->nmon)
	    count_obj(mon->minvent, &count, &size, FALSE, TRUE);

	*total_count += count; *total_size += size;

	Sprintf(buf, template, src, count, size);
	putstr(win, 0, buf);
}

STATIC_OVL void
mon_chain(win, src, chain, total_count, total_size)
	winid win;
	const char *src;
	struct monst *chain;
	long *total_count;
	long *total_size;
{
	char buf[BUFSZ];
	long count, size;
	struct monst *mon;

	for (count = size = 0, mon = chain; mon; mon = mon->nmon) {
	    count++;
	    size += sizeof(struct monst) + mon->mxlth + mon->mnamelth;
	}
	*total_count += count;
	*total_size += size;
	Sprintf(buf, template, src, count, size);
	putstr(win, 0, buf);
}

/*
 * Display memory usage of all monsters and objects on the level.
 */
static int
wiz_show_stats()
{
	char buf[BUFSZ];
	winid win;
	long total_obj_size = 0, total_obj_count = 0;
	long total_mon_size = 0, total_mon_count = 0;

	win = create_nhwindow(NHW_TEXT);
	putstr(win, 0, "Current memory statistics:");
	putstr(win, 0, "");
	Sprintf(buf, "Objects, size %d", (int) sizeof(struct obj));
	putstr(win, 0, buf);
	putstr(win, 0, "");
	putstr(win, 0, count_str);

	obj_chain(win, "invent", invent, &total_obj_count, &total_obj_size);
	obj_chain(win, "fobj", fobj, &total_obj_count, &total_obj_size);
	obj_chain(win, "buried", level.buriedobjlist,
				&total_obj_count, &total_obj_size);
	obj_chain(win, "migrating obj", migrating_objs,
				&total_obj_count, &total_obj_size);
	mon_invent_chain(win, "minvent", fmon,
				&total_obj_count,&total_obj_size);
	mon_invent_chain(win, "migrating minvent", migrating_mons,
				&total_obj_count, &total_obj_size);

	contained(win, "contained",
				&total_obj_count, &total_obj_size);

	putstr(win, 0, separator);
	Sprintf(buf, template, "Total", total_obj_count, total_obj_size);
	putstr(win, 0, buf);

	putstr(win, 0, "");
	putstr(win, 0, "");
	Sprintf(buf, "Monsters, size %d", (int) sizeof(struct monst));
	putstr(win, 0, buf);
	putstr(win, 0, "");

	mon_chain(win, "fmon", fmon,
				&total_mon_count, &total_mon_size);
	mon_chain(win, "migrating", migrating_mons,
				&total_mon_count, &total_mon_size);

	putstr(win, 0, separator);
	Sprintf(buf, template, "Total", total_mon_count, total_mon_size);
	putstr(win, 0, buf);

#if defined(__BORLANDC__) && !defined(_WIN32)
	show_borlandc_stats(win);
#endif

	display_nhwindow(win, FALSE);
	destroy_nhwindow(win);
	return 0;
}

void
sanity_check()
{
	obj_sanity_check();
	timer_sanity_check();
}

#ifdef DEBUG_MIGRATING_MONS
static int
wiz_migrate_mons()
{
	int mcount = 0;
	char inbuf[BUFSZ];
	struct permonst *ptr;
	struct monst *mtmp;
	d_level tolevel;
	getlin("How many random monsters to migrate? [0]", inbuf);
	if (*inbuf == '\033') return 0;
	mcount = atoi(inbuf);
	if (mcount < 0 || mcount > (COLNO * ROWNO) || Is_botlevel(&u.uz))
		return 0;
	while (mcount > 0) {
		if (Is_stronghold(&u.uz))
		    assign_level(&tolevel, &valley_level);
		else
		    get_level(&tolevel, depth(&u.uz) + 1);
		ptr = rndmonst();
		mtmp = makemon(ptr, 0, 0, NO_MM_FLAGS);
		if (mtmp) migrate_to_level(mtmp, ledger_no(&tolevel),
				MIGR_RANDOM, (coord *)0);
		mcount--;
	}
	return 0;
}
#endif

#endif /* WIZARD */

/* Holds the key that getdir errored out on. Mostly only useful if it
   was told not to loop. */
static int getdir_errorkey = 0;

void
rhack(cmd)
register char *cmd;
{
	boolean do_walk, do_rush, prefix_seen, bad_command,
          firsttime = (cmd == 0), inreparse = FALSE;

	iflags.menu_requested = FALSE;
	if (firsttime) {
		flags.nopick = 0;
		cmd = parse();
	}

        if (*cmd == CMD_TRAVEL) inreparse = TRUE; /* travel does its own reparse */

        if (0) {
reparse_direction:
                /* The user gave a prefix command like F or m, but
                   we don't yet know which direction they want to
                   move in. Ask for that now. Note that cmd[0]
                   holds the NetHack 3.4.3 key at this point, not
                   the key the user pressed. */
                cmd[1] = getdir(cmd[0] == 'F' ? "Attack in which direction?" :
                                cmd[0] == 'm' ? "Move in which direction?" :
                                cmd[0] == 'g' ? "Repeatedly move in which direction?" : 0,
                                cmd[0] == 'g' ? GETDIRH_RANGE : GETDIRH_NEXT, 0);
                if (cmd[1] == 0) {
                  cmd[0] = '\033';
                  pline("I don't know what you mean.");
                } else if (u.dz != 0) {
                  /* F, m, g are meaningless with vertical movement */
                  if (cmd[0] == 'F') {
                    pline("But there can't be a monster lurking above or below you!");
                    pline("(With an appropriate weapon, use the %s command "
                          "to attack terrain.)", key_for_cmd("#apply"));
                  } else pline("That doesn't make any sense...");
                  cmd[0] = '\033';
                }
                /* Translate the pressed direction into standard
                   vikeys, no matter what control system is being used. */
                else if (u.dx > 0 && u.dy > 0) cmd[1] = 'n';
                else if (u.dx > 0 && u.dy < 0) cmd[1] = 'u';
                else if (u.dx < 0 && u.dy > 0) cmd[1] = 'b';
                else if (u.dx < 0 && u.dy < 0) cmd[1] = 'y';
                else if (u.dx < 0) cmd[1] = 'h';
                else if (u.dx > 0) cmd[1] = 'l';
                else if (u.dy < 0) cmd[1] = 'k';
                else if (u.dy > 0) cmd[1] = 'j';
                else cmd[0] = '\033';
                u.dx = u.dy = u.dz = 0;
                cmd[2] = 0;
        }
reparse:
	if (*cmd == '\033') {
		flags.move = FALSE;
		return;
	}
#ifdef REDO
	if (*cmd == DOAGAIN && inreparse && !in_doagain && saveq[0]) {
		in_doagain = TRUE;
		stail = 0;
		rhack((char *)0);	/* read and execute command */
		in_doagain = FALSE;
		return;
	}
	/* Special case of *cmd == ' ' handled better below */
	if(!*cmd || *cmd == (char)0377)
#else
	if(!*cmd || *cmd == (char)0377 || (!flags.rest_on_space && *cmd == ' '))
#endif
	{
		nhbell();
		flags.move = FALSE;
		return;		/* probably we just had an interrupt */
	}

	/* handle most movement commands */
	do_walk = do_rush = prefix_seen = FALSE;
	flags.travel = iflags.travel1 = 0;
	switch (inreparse ? *cmd : 0) {
	 case 'g':  if (!cmd[1]) goto reparse_direction;
                    if (movecmd(cmd[1])) {
			flags.run = 2;
			do_rush = TRUE;
		    } else
			prefix_seen = TRUE;
		    break;
	 case 'G':  if (!cmd[1]) goto reparse_direction;
                    if (movecmd(lowc(cmd[1]))) {
			flags.run = 3;
			do_rush = TRUE;
		    } else
			prefix_seen = TRUE;
		    break;
	/* Effects of movement commands and invisible monsters:
	 * m: always move onto space (even if 'I' remembered)
	 * F: always attack space (even if 'I' not remembered)
	 * normal movement: attack if 'I', move otherwise
	 */
	 case 'F':  if (!cmd[1]) goto reparse_direction;
                    if (movecmd(cmd[1])) {
			flags.forcefight = 1;
			do_walk = TRUE;
		    } else
			prefix_seen = TRUE;
		    break;
	 case 'm':  if (!cmd[1]) goto reparse_direction;
                    if (movecmd(cmd[1]) || u.dz) {
			flags.run = 0;
			flags.nopick = 1;
			if (!u.dz) do_walk = TRUE;
			else cmd[0] = cmd[1];	/* "m<" or "m>" */
		    } else
			prefix_seen = TRUE;
		    break;
	 case 'M':  if (!cmd[1]) goto reparse_direction;
                    if (movecmd(lowc(cmd[1]))) {
			flags.run = 1;
			flags.nopick = 1;
			do_rush = TRUE;
		    } else
			prefix_seen = TRUE;
		    break;
	 case CMD_TRAVEL:
		    if (iflags.travelcmd) {
			    flags.travel = 1;
			    iflags.travel1 = 1;
			    flags.run = 8;
			    flags.nopick = !iflags.autoexplore;
			    do_rush = TRUE;
			    break;
		    }
		    /*FALLTHRU*/
         default:   if (!inreparse) break;
                    if (movecmd(*cmd)) {	/* ordinary movement */
			flags.run = 0;	/* only matters here if it was 8 */
			do_walk = TRUE;
		    } else if (movecmd(lowc(*cmd))) {
			flags.run = 1;
			do_rush = TRUE;
		    } else if (movecmd(unctrl(*cmd)) && *cmd == unmeta(*cmd)) {
			flags.run = 3;
			do_rush = TRUE;
		    } else if (movecmd(unctrl(unmeta(*cmd)))) {
                        /* Must be that way round; unmeta(unctrl())
                           doesn't work. flags.run = 1 and do_walk =
                           "run one square", which is basically like
                           shift-moving; no combat, but items are
                           still picked up */
			flags.run = 1;
                        do_walk = TRUE;
                    } else {
                      *cmd = 033;
                      impossible("Reparsing to non-movement command?");
                    }
		    break;
	}

	/* some special prefix handling */
	/* overload 'm' prefix for ',' to mean "request a menu" */
	if (prefix_seen && inreparse && cmd[1] == ',') {
		iflags.menu_requested = TRUE;
		++cmd;
	}

	if (do_walk) {
	    if (multi) flags.mv = TRUE;
	    check_tutorial_command('m');
	    domove();
	    flags.forcefight = 0;
            flags.run = 0;
	    return;
	} else if (do_rush) {
	    if (firsttime) {
		if (!multi) multi = max(COLNO,ROWNO);
		u.last_str_turn = 0;
	    }
	    flags.mv = TRUE;
	    check_tutorial_command('G');
	    domove();
	    return;
	} else if (prefix_seen && cmd[1] == '\033') {	/* <prefix><escape> */
	    /* don't report "unknown command" for change of heart... */
	    bad_command = FALSE;
        } else if (inreparse) {
            /* It's possible to get here if a grid-bug-form player
               attempts to use a command that is normally a movement
               command, but meaningless for a grid bug. */
            pline("That's not a direction!");
            flags.move = FALSE;
            return;
        } else if (*cmd == '#' && cmd[1]) {
            /* command repeat on extended commands; note that a literal #
               is correct here (# can't be rebound to anything but extended
               commands, and the command memory will be modified to start
               with #) */
	    register const struct ext_func_tab *tlist;
	    int res, NDECL((*func));

            for (tlist = extcmdlist; tlist->ef_txt; tlist++) {
                func = ((struct ext_func_tab *)tlist)->ef_funct;
                if (!strcmp(cmd+1, tlist->ef_txt)) {
                    if (!func) continue; /* sanity, shouldn't be reachable */
		    res = (*func)();		/* perform the command */
                    if (func == doextcmd && res < -REPARSE_AS_KEY &&
                        res > -REPARSE_AS_KEY-256) { /* bouncing a key onto us */
                        *cmd = (int)(-REPARSE_AS_KEY - res);
                        inreparse = TRUE;
                        goto reparse;
                    }
                    if (!res) {
                      flags.move = FALSE;
                      multi = 0;
                    }
                    return;
                }
            }
            bad_command = TRUE;
            multi = 0;
	/* handle all other commands */
	} else {
	    register const struct ext_func_tab *tlist;
	    int res, NDECL((*func));

	    for (tlist = extcmdlist; tlist->ef_txt; tlist++) {
		if ((*cmd & 0xff) != (tlist->binding1 & 0xff) &&
                    (*cmd & 0xff) != (tlist->binding2 & 0xff) &&
                    (*cmd & 0xff) != (tlist->binding3 & 0xff)) continue;
		check_tutorial_command(*cmd & 0xff);

		if (u.uburied && !tlist->can_if_buried) {
		    You_cant("do that while you are buried!");
		    res = 0;
		} else {
		    /* we discard 'const' because some compilers seem to have
		       trouble with the pointer passed to set_occupation() */
                    func = ((struct ext_func_tab *)tlist)->ef_funct;
                    /* The only action that sets an occupation is searching.
                       So we may as well special-case it here. */
		    if (func == dosearch && !occupation && multi)
			set_occupation(func, "searching", multi);
                    if (!func) {
                        *cmd = tlist->replacewithkey;
                        inreparse = TRUE;
                        goto reparse;
                    }
		    res = (*func)();		/* perform the command */
                    if (func == doextcmd && res < -REPARSE_AS_KEY &&
                        res > -REPARSE_AS_KEY-256) { /* bouncing a key onto us */
                        *cmd = (int)(-REPARSE_AS_KEY - res);
                        inreparse = TRUE;
                        goto reparse;
                    }
		}
		if (!res) {
		    flags.move = FALSE;
		    multi = 0;
		}
		return;
	    }
	    /* if we reach here, cmd wasn't found in cmdlist[] */
	    bad_command = TRUE;
	}

	if (bad_command) {
	    char expcmd[10];
	    register char *cp = expcmd;

	    while (*cmd && (int)(cp - expcmd) < (int)(sizeof expcmd - 3)) {
		if (*cmd >= 040 && *cmd < 0177) {
		    *cp++ = *cmd++;
		} else if (*cmd & 0200) {
		    *cp++ = 'M';
		    *cp++ = '-';
		    *cp++ = *cmd++ &= ~0200;
		} else {
		    *cp++ = '^';
		    *cp++ = *cmd++ ^ 0100;
		}
	    }
	    *cp = '\0';
	    if (!prefix_seen || !iflags.cmdassist ||
		!help_dir(0, "Invalid direction key!"))
		Norep("Unknown command '%s'.", expcmd);
	}
	/* didn't move */
	flags.move = FALSE;
	multi = 0;
	return;
}

int
xytod(x, y)	/* convert an x,y pair into a direction code */
schar x, y;
{
	register int dd;

	for(dd = 0; dd < 8; dd++)
	    if(x == xdir[dd] && y == ydir[dd]) return dd;

	return -1;
}

void
dtoxy(cc,dd)	/* convert a direction code into an x,y pair */
coord *cc;
register int dd;
{
	cc->x = xdir[dd];
	cc->y = ydir[dd];
	return;
}

/* Translate a traditional-vikeys direction into a movement direction.
   This function should only be used internally and not by anything that
   faces the UI, because the user's controls might not be traditional
   vikeys. */
int
movecmd(sym)	/* also sets u.dz, but returns false for <> */
char sym;
{
	register const char *dp;
	register const char *sdp;
	sdp = sdir;

	u.dz = 0;
	if(!(dp = index(sdp, sym))) return 0;
	u.dx = xdir[dp-sdp];
	u.dy = ydir[dp-sdp];
	u.dz = zdir[dp-sdp];
	if (u.dx && u.dy && u.umonnum == PM_GRID_BUG) {
		u.dx = u.dy = 0;
		return 0;
	}
	return !u.dz;
}

/* Global, is set to 8 for long-distance commands. */
int movecmdui_distance = 1;

/* Like movecmd, but using user-defined keys. */
int
movecmdui(sym)
char sym;
{
        register const struct ext_func_tab *tlist;
        int i;

        movecmdui_distance = 1;

        /* We use replacewithkey to translate the pressed key into
           traditional vikeys, and then use the internal movecmd. */
        for (tlist = extcmdlist; tlist->ef_txt; tlist++) {
          if ((sym & 0xff) != (tlist->binding1 & 0xff) &&
              (sym & 0xff) != (tlist->binding2 & 0xff) &&
              (sym & 0xff) != (tlist->binding3 & 0xff)) continue;
          if(tlist->ef_funct == doup) return movecmd('<');
          if(tlist->ef_funct == dodown) return movecmd('>');
          /* doterrain's meant to replace >, so should also be considered
             as aiming downwards */
          if(tlist->ef_funct == doterrain) return movecmd('>');
          if (!tlist->replacewithkey) return 0;
          i = unmeta((unsigned char)tlist->replacewithkey);
          if (unctrl(i) && unctrl(i) != i) {
            /* i.e. not control-meta-... */
            if (i == tlist->replacewithkey) movecmdui_distance = 3;
            i = unctrl(i);
          }
          if (i != lowc(i)) movecmdui_distance = 8;
          return movecmd(lowc(i));
        }
        return 0;
}

/*
 * uses getdir() but unlike getdir() it specifically
 * produces coordinates using the direction from getdir()
 * and verifies that those coordinates are ok.
 *
 * If the call to getdir() returns 0, Never_mind is displayed.
 * If the resulting coordinates are not okay, emsg is displayed.
 *
 * Returns non-zero if coordinates in cc are valid.
 */
int get_adjacent_loc(prompt,emsg,x,y,cc)
const char *prompt, *emsg;
xchar x,y;
coord *cc;
{
	xchar new_x, new_y;
	if (!getdir(prompt, GETDIRH_NEXT, 1)) {
		pline("%s",Never_mind);
		return 0;
	}
	new_x = x + u.dx;
	new_y = y + u.dy;
	if (cc && isok(new_x,new_y)) {
		cc->x = new_x;
		cc->y = new_y;
	} else {
		if (emsg) pline("%s",emsg);
		return 0;
	}
	return 1;
}

static int nextgetdir = 0;
/* Forces the next return value from getdir.
   Argument is a vikeys direction; the direction will not be
   screwed up by stunning or confusion, so that must be checked
   before using this function. */
void
setnextgetdir(c)
int c;
{
    nextgetdir = c;
}

void
setnextgetdirdxdy(dx,dy)
int dx,dy;
{
    if (dx == 0 && dy == 0) setnextgetdir('.');
    if (dx == 0 && dy  < 0) setnextgetdir('k');
    if (dx == 0 && dy  > 0) setnextgetdir('j');
    if (dx  < 0 && dy == 0) setnextgetdir('h');
    if (dx  < 0 && dy  < 0) setnextgetdir('y');
    if (dx  < 0 && dy  > 0) setnextgetdir('b');
    if (dx  > 0 && dy == 0) setnextgetdir('l');
    if (dx  > 0 && dy  < 0) setnextgetdir('u');
    if (dx  > 0 && dy  > 0) setnextgetdir('n');
}

boolean
isnextgetdirset()
{
    return !!nextgetdir;
}

/* Gets a direction from the player.
   The second argument specifies how to highlight directions.
   Returns 1 on success; on error, sets getdir_errorkey to the
   code of the key actually pressed, and returns 0. If the third
   argument is TRUE, don't allow erroneous input, but rather
   keep asking until the user presses something valid. */
int
getdir(s,how,loop)
const char *s;
int how;
int loop;
{
	char dirsym;
        int dist;
        int redone = FALSE;
        boolean gridbug = u.umonnum == PM_GRID_BUG;

        if (nextgetdir) {
            movecmd(nextgetdir);
            nextgetdir = 0;
            return 1;
        }

getdir_again:
        if (iflags.targethighlight) {
            set_getdir_type(how);
            for (dist = 0; dist < 80; dist++) {
                redraw_glyph(u.ux+dist,u.uy     );
                redraw_glyph(u.ux-dist,u.uy     );
                redraw_glyph(u.ux     ,u.uy+dist);
                redraw_glyph(u.ux     ,u.uy-dist);
                if (!gridbug) {
                  redraw_glyph(u.ux+dist,u.uy+dist);
                  redraw_glyph(u.ux-dist,u.uy+dist);
                  redraw_glyph(u.ux+dist,u.uy-dist);
                  redraw_glyph(u.ux-dist,u.uy-dist);
                }
            }
            flush_screen(1);
        }

#ifdef REDO
	if(in_doagain || *readchar_queue)
	    dirsym = readchar();
	else
#endif
            dirsym = yn_function ((s && *s != '^') ? s :
                                  (s && *s == '^') ? s+1 :
                                  "In what direction?",
                                  (char *)0, '\0');
#ifdef REDO
	savech(dirsym);
#endif

        if (iflags.targethighlight) {
            set_getdir_type(GETDIRH_NONE);
            for (dist = 0; dist < 80; dist++) {
                redraw_glyph(u.ux+dist,u.uy     );
                redraw_glyph(u.ux-dist,u.uy     );
                redraw_glyph(u.ux     ,u.uy+dist);
                redraw_glyph(u.ux     ,u.uy-dist);
                if (!gridbug) {
                  redraw_glyph(u.ux+dist,u.uy+dist);
                  redraw_glyph(u.ux-dist,u.uy+dist);
                  redraw_glyph(u.ux+dist,u.uy-dist);
                  redraw_glyph(u.ux-dist,u.uy-dist);
                }
            }
            flush_screen(1);
        }

	if(dirsym == '.' || dirsym == 's')
		u.dx = u.dy = u.dz = 0;
	else if(!movecmdui(dirsym) && !u.dz) {
		boolean did_help = !loop;
		if(!index(quitchars, dirsym)) {
                    /* If loop is false, caller must handle error messages;
                       show help on a press of ? no matter what */
                    if ((iflags.cmdassist && !redone && loop) || dirsym == '?') {
			did_help = help_dir((s && *s == '^') ? dirsym : 0,
					    "Invalid direction key!");
		    }
		    if (!did_help) pline("What a strange direction!");
                    redone = TRUE;
                    getdir_errorkey = dirsym;
                    if (loop || dirsym == '?') goto getdir_again;
		}
		return 0;
	}
	if(!u.dz && (Stunned || (Confusion && !rn2(5)))) confdir();
        clear_nhwindow(WIN_MESSAGE);
	return 1;
}

STATIC_OVL boolean
help_dir(sym, msg)
char sym;
const char *msg;
{
	winid win;
	char buf[BUFSZ];

	win = create_nhwindow(NHW_TEXT);
	if (!win) return FALSE;
	if (msg) {
		putstr(win, 0, msg);
		putstr(win, 0, "");
	}
	if (letter(sym)) { 
	    sym = highc(sym);
            Sprintf(buf, "(If you're trying to press '^%c', the ^ represents",
                    sym);
            putstr(win, 0, buf);
            Sprintf(buf, "Control; hold Control and press %c, don't press the",
                    sym);
            putstr(win, 0, buf);
            putstr(win, 0, "^ key on your keyboard.)");
            putstr(win, 0, "");
	}
	if (u.umonnum == PM_GRID_BUG) {
	    putstr(win, 0, "Valid direction keys in your current form are:");
	    putstr(win, 0, "             k   ");
	    putstr(win, 0, "             |   ");
	    putstr(win, 0, "          h- . -l");
	    putstr(win, 0, "             |   ");
	    putstr(win, 0, "             j   ");
	} else {
	    putstr(win, 0, "Valid direction keys are:");
	    putstr(win, 0, "          y  k  u");
	    putstr(win, 0, "           \\ | / ");
	    putstr(win, 0, "          h- . -l");
	    putstr(win, 0, "           / | \\ ");
	    putstr(win, 0, "          b  j  n");
	};
	putstr(win, 0, "");
	putstr(win, 0, "          <  up");
	putstr(win, 0, "          >  down");
	putstr(win, 0, "          .  direct at yourself");
	putstr(win, 0, "");
	display_nhwindow(win, FALSE);
	destroy_nhwindow(win);
	return TRUE;
}

#endif /* OVL1 */
#ifdef OVLB

void
confdir()
{
	register int x = (u.umonnum == PM_GRID_BUG) ? 2*rn2(4) : rn2(8);
	u.dx = xdir[x];
	u.dy = ydir[x];
	return;
}

#endif /* OVLB */
#ifdef OVL0

int
isok(x,y)
register int x, y;
{
	/* x corresponds to curx, so x==1 is the first column. Ach. %% */
	return x >= 1 && x <= COLNO-1 && y >= 0 && y <= ROWNO-1;
}

static NEARDATA int last_multi;

/*
 * convert a MAP window position into a movecmd
 */
const char *
click_to_cmd(x, y, mod)
    int x, y, mod;
{
    int dir;
    static char cmd[4];
    cmd[1]=0;

    x -= u.ux;
    y -= u.uy;

    if (iflags.travelcmd) {
        if (abs(x) <= 1 && abs(y) <= 1 ) {
            x = sgn(x), y = sgn(y);
        } else {
            u.tx = u.ux+x;
            u.ty = u.uy+y;
            cmd[0] = CMD_TRAVEL;
            return cmd;
        }

        if(x == 0 && y == 0) {
            /* here */
            if(IS_FOUNTAIN(levl[u.ux][u.uy].typ) || IS_SINK(levl[u.ux][u.uy].typ)) {
                cmd[0]=mod == CLICK_1 ? 'q' : M('d');
                return cmd;
            } else if(IS_THRONE(levl[u.ux][u.uy].typ)) {
                cmd[0]=M('s');
                return cmd;
            } else if((u.ux == xupstair && u.uy == yupstair)
                      || (u.ux == sstairs.sx && u.uy == sstairs.sy && sstairs.up)
                      || (u.ux == xupladder && u.uy == yupladder)) {
                return "<";
            } else if((u.ux == xdnstair && u.uy == ydnstair)
                      || (u.ux == sstairs.sx && u.uy == sstairs.sy && !sstairs.up)
                      || (u.ux == xdnladder && u.uy == ydnladder)) {
                return ">";
            } else if(OBJ_AT(u.ux, u.uy)) {
                cmd[0] = Is_container(level.objects[u.ux][u.uy]) ? M('l') : ',';
                return cmd;
            } else {
                return "."; /* just rest */
            }
        }

        /* directional commands */

        dir = xytod(x, y);

	if (!m_at(u.ux+x, u.uy+y) && !test_move(u.ux, u.uy, x, y, TEST_MOVE)) {
            cmd[1] = (sdir[dir]);
            cmd[2] = 0;
            if (IS_DOOR(levl[u.ux+x][u.uy+y].typ)) {
                /* slight assistance to the player: choose kick/open for them */
                if (levl[u.ux+x][u.uy+y].doormask & D_LOCKED) {
                    cmd[0] = C('d');
                    return cmd;
                }
                if (levl[u.ux+x][u.uy+y].doormask & D_CLOSED) {
                    cmd[0] = 'o';
                    return cmd;
                }
            }
            if (levl[u.ux+x][u.uy+y].typ <= SCORR) {
                cmd[0] = 's';
                cmd[1] = 0;
                return cmd;
            }
        }
    } else {
        /* convert without using floating point, allowing sloppy clicking */
        if(x > 2*abs(y))
            x = 1, y = 0;
        else if(y > 2*abs(x))
            x = 0, y = 1;
        else if(x < -2*abs(y))
            x = -1, y = 0;
        else if(y < -2*abs(x))
            x = 0, y = -1;
        else
            x = sgn(x), y = sgn(y);

        if(x == 0 && y == 0)	/* map click on player to "rest" command */
            return ".";

        dir = xytod(x, y);
    }

    /* move, attack, etc. */
    cmd[1] = 0;
    if(mod == CLICK_1) {
	cmd[0] = (sdir[dir]);
    } else {
	cmd[0] = (sdir[dir] - 'a' + 'A'); /* run command */
    }

    return cmd;
}

STATIC_OVL char *
parse()
{
#ifdef LINT	/* static char in_line[COLNO]; */
	char in_line[COLNO];
#else
	static char in_line[COLNO];
#endif
	int foo;
        int extcmd_char1 = 0;
	boolean prezero = FALSE;
        struct ext_func_tab* ecl_extcmd = extcmdlist;

        while (ecl_extcmd->ef_funct != doextcmd) ecl_extcmd++;

	multi = 0;
	flags.move = 1;
	flush_screen(1); /* Flush screen buffer. Put the cursor on the hero. */
        
	if ((foo = readchar()) == ecl_extcmd->binding1 || 
            foo == ecl_extcmd->binding2 ||
            foo == ecl_extcmd->binding3) {
            clear_nhwindow(WIN_MESSAGE);
            pline("# ");
            mark_synch();
	    for (;;) {
		foo = readchar();
		if (foo >= '0' && foo <= '9') {
		    multi = 10 * multi + foo - '0';
		    if (multi < 0 || multi >= LARGEST_INT) multi = LARGEST_INT;
                    clear_nhwindow(WIN_MESSAGE);
                    Sprintf(in_line, "Count: %d", multi);
                    pline("%s",in_line);
                    mark_synch();
		    last_multi = multi;
		    if (!multi && foo == '0') prezero = TRUE;
		} else if (foo == '#' && iflags.multiplayer) {
                    /* multiplayer chat */
                    char buf[BUFSZ];
                    getlin("##", buf);
                    if (*buf && *buf != '\033')
                      rpline("## %s", buf);
                    foo = '\033';
                    break;
                } else {	/* not a digit */
                    if (multi == 0 && !prezero) {
# ifdef REDO
                        if (!in_doagain)
                          extcmd_char1 = foo;
# else
                        /* TODO: This almost certainly breaks on tiles.
                           And it also breaks on multiplayer, because
                           select() doesn't see ungetc. Perhaps just
                           use pgetchar() as the input function no
                           matter what? */
                        ungetc(foo, stdin);
# endif
                        foo = ecl_extcmd->binding1;
                    }
                    /* otherwise we need another # to mark an extended
                       command, so no ungetc is useful or necessary */
                    break;
                }
	    }
        }

	if (foo == '\033') {   /* esc cancels count (TH) */
	    clear_nhwindow(WIN_MESSAGE);
	    multi = last_multi = 0;
# ifdef REDO
	} else if (foo == DOAGAIN || in_doagain) {
	    multi = last_multi;
	} else {
	    last_multi = multi;
	    savech(0);	/* reset input queue */
	    savech((char)foo);
            if (extcmd_char1) pushch(extcmd_char1); /* must be after savech(0) */
# endif
	}

	if (multi) {
	    multi--;
	    save_cm = in_line;
	} else {
	    save_cm = (char *)0;
	}
	in_line[0] = foo;
	in_line[1] = '\0';

	clear_nhwindow(WIN_MESSAGE);
	if (prezero) in_line[0] = '\033';
	return(in_line);
}

#endif /* OVL0 */
#ifdef OVLB

#ifdef UNIX
static
void
end_of_input()
{
#ifndef NOSAVEONHANGUP
	if (!program_state.done_hup++ && program_state.something_worth_saving)
	    (void) dosave0();
#endif
	exit_nhwindows((char *)0);
	clearlocks();
	terminate(EXIT_SUCCESS);
}
#endif

#endif /* OVLB */
#ifdef OVL0

char
readchar()
{
	register int sym;
	int x = u.ux, y = u.uy, mod = 0;

	if ( *readchar_queue )
	    sym = *readchar_queue++;
	else
#ifdef REDO
	    sym = in_doagain ? Getchar() : nh_poskey(&x, &y, &mod);
#else
	    sym = Getchar();
#endif

#ifdef UNIX
# ifdef NR_OF_EOFS
	if (sym == EOF) {
	    register int cnt = NR_OF_EOFS;
	  /*
	   * Some SYSV systems seem to return EOFs for various reasons
	   * (?like when one hits break or for interrupted systemcalls?),
	   * and we must see several before we quit.
	   */
	    do {
		clearerr(stdin);	/* omit if clearerr is undefined */
		sym = Getchar();
	    } while (--cnt && sym == EOF);
	}
# endif /* NR_OF_EOFS */
	if (sym == EOF)
	    end_of_input();
#endif /* UNIX */

	if(sym == 0) {
	    /* click event */
	    readchar_queue = click_to_cmd(x, y, mod);
	    sym = *readchar_queue++;
	}
	return((char) sym);
}

STATIC_PTR int
dotravel()
{
	/* Keyboard travel command */
	static char cmd[2];
	coord cc;

	if (!iflags.travelcmd) return 0;
	cmd[1]=0;
	cc.x = iflags.travelcc.x;
	cc.y = iflags.travelcc.y;
	if (cc.x == -1 && cc.y == -1) {
	    /* No cached destination, start attempt from current position */
	    cc.x = u.ux;
	    cc.y = u.uy;
	}
	pline("Where do you want to travel to?");
	if (getpos(&cc, TRUE, "the desired destination") < 0) {
		/* user pressed ESC */
		return 0;
	}
	iflags.travelcc.x = u.tx = cc.x;
	iflags.travelcc.y = u.ty = cc.y;
        iflags.autoexplore = FALSE;
	cmd[0] = CMD_TRAVEL;
	readchar_queue = cmd;
	return 0;
}

STATIC_PTR int
doautoexplore()
{
	static char cmd[2];
	iflags.autoexplore = TRUE;
        cmd[0] = CMD_TRAVEL;
        readchar_queue = cmd;
        return 0;
}

#ifdef PORT_DEBUG
# ifdef WIN32CON
extern void NDECL(win32con_debug_keystrokes);
extern void NDECL(win32con_handler_info);
# endif

int
wiz_port_debug()
{
	int n, k;
	winid win;
	anything any;
	int item = 'a';
	int num_menu_selections;
	struct menu_selection_struct {
		char *menutext;
		void NDECL((*fn));
	} menu_selections[] = {
#ifdef WIN32CON
		{"test win32 keystrokes", win32con_debug_keystrokes},
		{"show keystroke handler information", win32con_handler_info},
#endif
		{(char *)0, (void NDECL((*)))0}		/* array terminator */
	};

	num_menu_selections = SIZE(menu_selections) - 1;
	if (num_menu_selections > 0) {
		menu_item *pick_list;
		win = create_nhwindow(NHW_MENU);
		start_menu(win);
		for (k=0; k < num_menu_selections; ++k) {
			any.a_int = k+1;
			add_menu(win, NO_GLYPH, &any, item++, 0, ATR_NONE,
				menu_selections[k].menutext, MENU_UNSELECTED);
		}
		end_menu(win, "Which port debugging feature?");
		n = select_menu(win, PICK_ONE, &pick_list);
		destroy_nhwindow(win);
		if (n > 0) {
			n = pick_list[0].item.a_int - 1;
			free((genericptr_t) pick_list);
			/* execute the function */
			(*menu_selections[n].fn)();
		}
	} else
		pline("No port-specific debug capability defined.");
	return 0;
}
# endif /*PORT_DEBUG*/

#endif /* OVL0 */
#ifdef OVLB
/*
 *   Parameter validator for generic yes/no function to prevent
 *   the core from sending too long a prompt string to the
 *   window port causing a buffer overflow there.
 */
char
yn_function(query,resp, def)
const char *query,*resp;
char def;
{
	char qbuf[QBUFSZ];
	unsigned truncspot, reduction = sizeof(" [N]  ?") + 1;

	if (resp) reduction += strlen(resp) + sizeof(" () ");
	if (strlen(query) < (QBUFSZ - reduction))
		return (*windowprocs.win_yn_function)(query, resp, def);
	paniclog("Query truncated: ", query);
	reduction += sizeof("...");
	truncspot = QBUFSZ - reduction;
	(void) strncpy(qbuf, query, (int)truncspot);
	qbuf[truncspot] = '\0';
	Strcat(qbuf,"...");
	return (*windowprocs.win_yn_function)(qbuf, resp, def);
}
#endif

/*cmd.c*/
