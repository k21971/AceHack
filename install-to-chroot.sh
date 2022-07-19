#!/bin/bash
#
# Ideas and some parts from the original dgl-create-chroot (by joshk@triplehelix.org, modifications by jilles@stack.nl)
# More by <paxed@alt.org>
# More by Michael Andrew Streib <dtype@dtype.org>
#
# Adapted for install of unnethack in NAO-Style chroot by Tangles
#
# Licensed under the MIT License
# https://opensource.org/licenses/MIT

# autonamed chroot directory. Can rename.
DATESTAMP=`date +%Y%m%d-%H%M%S`
NAO_CHROOT="/opt/nethack/chroot"
# config outside of chroot
NETHACK_GIT="/home/build/AceHack"
# the user & group from dgamelaunch config file.
USRGRP="games:games"
# COMPRESS from include/config.h; the compression binary to copy. leave blank to skip.
COMPRESSBIN="/bin/gzip"
# fixed data to copy (leave blank to skip)
NH_GIT="/home/build/AceHack"
NH_BRANCH="master"
# HACKDIR from include/config.h; aka nethack subdir inside chroot
NHSUBDIR="acehack"
# END OF CONFIG
##############################################################################

errorexit()
{
    echo "Error: $@" >&2
    exit 1
}

findlibs()
{
  for i in "$@"; do
      if [ -z "`ldd "$i" | grep 'not a dynamic executable'`" ]; then
         echo $(ldd "$i" | awk '{ print $3 }' | egrep -v ^'\(' | grep lib)
         echo $(ldd "$i" | grep 'ld-linux' | awk '{ print $1 }')
      fi
  done
}

set -e

umask 022

echo "Creating inprogress and userdata directories"
mkdir -p "$NAO_CHROOT/dgldir/inprogress-ace"
chown "$USRGRP" "$NAO_CHROOT/dgldir/inprogress-ace"
mkdir -p "$NAO_CHROOT/dgldir/extrainfo-ace"
chown "$USRGRP" "$NAO_CHROOT/dgldir/extrainfo-ace"

echo "Making $NAO_CHROOT/$NHSUBDIR"
mkdir -p "$NAO_CHROOT/$NHSUBDIR"
chown "$USRGRP" "$NAO_CHROOT/$NHSUBDIR"

echo "Copying AceHack playground stuff"
cp "$NETHACK_GIT/dat/nhdat" "$NAO_CHROOT/$NHSUBDIR"
chmod 644 "$NAO_CHROOT/$NHSUBDIR/nhdat"

echo "Creating AceHack variable dir stuff."
mkdir -p "$NAO_CHROOT/$NHSUBDIR/save"
mkdir -p "$NAO_CHROOT/$NHSUBDIR/dumps"
touch "$NAO_CHROOT/$NHSUBDIR/logfile"
touch "$NAO_CHROOT/$NHSUBDIR/perm"
touch "$NAO_CHROOT/$NHSUBDIR/record"
touch "$NAO_CHROOT/$NHSUBDIR/xlogfile"
touch "$NAO_CHROOT/$NHSUBDIR/livelog"

# everything created so far needs the chown.
( cd $NAO_CHROOT/$NHSUBDIR ; chown -R "$USRGRP" * )

# Everything below here should remain owned by root.

NETHACKBIN="$NETHACK_GIT/src/acehack"
if [ -n "$NETHACKBIN" -a ! -e "$NETHACKBIN" ]; then
  errorexit "Cannot find acehack binary $NETHACKBIN"
fi

if [ -n "$NETHACKBIN" -a -e "$NETHACKBIN" ]; then
  echo "Copying $NETHACKBIN"
  cd "$NAO_CHROOT/$NHSUBDIR"
  NHBINFILE="`basename $NETHACKBIN`-$DATESTAMP"
  cp "$NETHACKBIN" "$NHBINFILE"
  rm -f acehack
  ln -s "$NHBINFILE" acehack
  LIBS="$LIBS `findlibs $NETHACKBIN`"
  cd "$NAO_CHROOT"
fi

RECOVER="$NETHACK_GIT/util/recover"

if [ -n "$RECOVER" -a -e "$RECOVER" ]; then
  echo "Copying $RECOVER"
  cp "$RECOVER" "$NAO_CHROOT/$NHSUBDIR"
  LIBS="$LIBS `findlibs $RECOVER`"
  cd "$NAO_CHROOT"
fi


LIBS=`for lib in $LIBS; do echo $lib; done | sort | uniq`
echo "Copying libraries:" $LIBS
for lib in $LIBS; do
  mkdir -p "$NAO_CHROOT`dirname $lib`"
  if [ -f "$NAO_CHROOT$lib" ]
  then
    echo "$NAO_CHROOT$lib already exists - skipping."
  else
    cp $lib "$NAO_CHROOT$lib"
  fi
done

echo "Finished."
