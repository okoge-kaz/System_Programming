#!/bin/sh
key=key
in=/dev/null
param=
binary=
verbose=
all=
while getopts :k:i:p:bva OPT; do
  case $OPT in
    k) key="$OPTARG" ;;
    i) in="$OPTARG" ; [ "$key" = key ] && key=$(basename "$OPTARG" .txt) ;;
    p) param="$OPTARG" ;;
    b) binary="-b" ;;
    v) verbose="-v" ;;
    a) all="-a" ;;
    \?) echo "Usage: $0 [-b] [-v] [-a] [-k name] [-i infile] cmd params..." ; exit 127 ;;
  esac
done
shift `expr $OPTIND - 1`
cmd=$1
shift
args="$*"

# run cat and obtain the result
tmpout=$(mktemp)
trap "rm -f '$tmpout'" EXIT
eval "sh $args <'$in' >'$tmpout' 2>/dev/null"
rm -f .test.tmp*.txt
eval "sh ./test.sh $binary $verbose $all -k '$key' -i '$in' -o '$tmpout' $cmd $param $args"
rm -f .test.tmp*.txt
