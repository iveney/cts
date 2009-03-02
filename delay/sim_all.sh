#!/bin/bash
# run eval script to get delay under different capacitance

NAME=s1000
INPUT=$NAME
OUTPUT=$NAME
declare -a OUT
rm -rf table

for i in `seq 0 10001`
do
	INPUT="$NAME.$i"
	OUTPUT="${INPUT}.out"
	echo "evaluating $i..."
	# generate input file
	sed -e "s/\(2 1500001 0\) 0/\1 $i/" s1000 > "$INPUT"
	# evaluate and output to file
	./eval2009v04.pl -s -v1 $INPUT s1000s tuned_45nm_HP.pm > "$OUTPUT"
	# get result
	OUT=(`grep "sink" "$OUTPUT" | cut -d " " -f 9`)
	# compute skew
	VL=`echo "scale=6; (${OUT[3]} + ${OUT[1]} - ${OUT[2]} - ${OUT[0]})/4.0" | bc`
	VH=`echo "scale=6; (${OUT[7]} + ${OUT[5]} - ${OUT[6]} - ${OUT[4]})/4.0" | bc`
	DELTA=`echo "$VL - $VH" | bc`
	# output data
	echo -e "$i\t$VL\t$VH\t$DELTA" >> table
	rm -rf $INPUT $OUTPUT
done

rm *.spice
rm *.wave
