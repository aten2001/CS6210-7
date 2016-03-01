#/bin/bash

# $1: command

for i in {1..1000}
do
	com=`./$1` 
	result0=`echo "$com"|tail -l`
	echo $result0 $i
done