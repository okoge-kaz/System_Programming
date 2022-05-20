#!/bin/sh
key=key
in=/dev/null
out=/dev/null
binary=no
verbose=no
all=no
cmp='cmp -s'
while getopts :k:i:o:bvaw OPT; do
  case $OPT in
    k) key="$OPTARG" ;;
    i) in="$OPTARG" ;;
    o) out="$OPTARG" ;;
    b) binary=yes ;;
    v) verbose=yes ;;
    a) all=yes ;;
    w) cmp='diff -wbu' ;;
    \?) echo "Usage: $0 [-b] [-v] [-a] [-w] [-k name] [-i infile] [-o outfile] cmd..." ; exit 127 ;;
  esac
done
shift `expr $OPTIND - 1`
cmd="$*"

tmpout=$(mktemp)
tmperr=$(mktemp)
trap "rm -f '$tmpout' '$tmperr'" EXIT

eval "$cmd <'$in' >'$tmpout' 2>'$tmperr'"
status=$?

[ "$in" != /dev/null ] && cmd="$cmd < $in"

if eval "$cmp '$tmpout' '$out'" >/dev/null 2>&1; then
  echo "✅  OK 🎉 [$key] $cmd "
  if [ "$all" = yes ]; then
    if [ "$binary" = yes ]; then
      [ "$in" != /dev/null ] && echo "--- stdin" && od -c $in
      echo "--- stdout expected" && od -c "$out"
      echo "--- stdout" && od -c "$tmpout"
    else
      [ "$in" != /dev/null ] && echo "--- stdin" && cat $in
      echo "--- stdout expected" && cat "$out"
      echo "--- stdout" && cat "$tmpout"
    fi
    echo "--- exit status: $status"
  fi
  exit 0
else
  echo "❌  NG 😱 [$key] $cmd "
  if [ \( "$all" = yes \) -o \( "$verbose" = yes \) ]; then
    if [ "$binary" = yes ]; then
      [ "$in" != /dev/null ] && echo "--- stdin" && od -c $in
      echo "--- stdout expected" && od -c "$out"
      echo "--- stdout" && od -c "$tmpout"
    else
      [ "$in" != /dev/null ] && echo "--- stdin" && cat $in
      echo "--- stdout expected" && cat "$out"
      echo "--- stdout" && cat "$tmpout"
    fi
    echo "--- exit status: $status"
  fi
  exit 0   # exit 1 doesn't fit the make command...
fi
