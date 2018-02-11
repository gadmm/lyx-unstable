#!/bin/bash
#debugging mode:
#set -x
#trap read debug
lyx=`which $1`
if [ ! -x "$lyx" ]
then
	echo "Usage: " $0 " path/to/lyx"
	exit 1
fi
if [ ! -x "`which pcregrep`" ]
then
	echo "pcregrep not found"
	exit 1
fi
echo "Using " $lyx
errors=0
files=0
convert () {
	local ret
	((++files))
	echo "convert stable/$1"
    rm -f conv/$1 conv/$1.diff
	$lyx -batch -E lyx conv/$1 stable/$1
	ret=$?
	diff stable/$1 conv/$1 > conv.diff
	if [ ! $ret ]
	then
		# LyX returns an error
		((++errors))
		echo "LyX error with file" $1
		echo
	elif pcregrep -M -f expected.diff.regex conv.diff > /dev/null
	then
		# diff is as expected
		rm conv/$1
		rm conv.diff
	else
		# diff is not at expected
		((++errors))
		echo "Mismatch with file" $1
		cat conv.diff
		mv conv.diff conv/$1.diff
		echo
	fi
}
shopt -s nullglob #do not act weird if *.lyx expands to nothing
for i in stable/*.lyx stable/*/*.lyx
do
	#strip the prefix stable/ 
    convert ${i#stable/}
done
echo "Number of files:" $files
echo "Number of errors:" $errors
[ $errors ]
