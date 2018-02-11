#!/bin/bash
## Make sure that all lyx files in ./stable/ come from a lyx executable
## the lyx executable should be lyx stable.
#
##debugging mode:
#set -x
#trap read debug
lyx=`which $1`
if [ ! -x "$lyx" ]
then
	echo "Usage: " $0 " path/to/lyx"
	exit 1
fi
echo "Using " $lyx
files=0
convert () {
	local ret
	((++files))
	echo "setup stable/$1"
	$lyx -batch -E lyx stable/$1 stable/$1
	ret=$?
	if [ ! $ret ]
	then
		# LyX returns an error
		echo "LyX error with file" $1
	fi
}
shopt -s nullglob #do not act weird if *.lyx expands to nothing
for i in stable/*.lyx stable/*/*.lyx
do
	#strip the prefix stable/ 
    convert ${i#stable/}
done
echo "Number of files:" $files
