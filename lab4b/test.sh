#!/bin/sh

touch errors.txt
./lab4b --bogus 2> /dev/null
if [ $? -ne 1 ]
then
	echo "Error: bogus argument not detected\n" >> errors.txt
fi

./lab4b --scale=C --period=5 --log=testing.txt <<EOF
START
START
START
STOP
PERIOD=2
SCALE=F
OFF
EOF

if [ ! -s testing.txt ]
then
	echo "Error: testing.txt not created\n" >> errors.txt
fi

if [ $? -ne 0 ]
then
	echo "Error: incorrect return value\n" >> errors.txt
fi

for c in START START STOP START PERIOD=2 SCALE=F OFF
do
	grep $c testing.txt > /dev/null
	if [ $? -ne 0 ]
	then
		echo "Error: did not log $c command" >> errors.txt
	fi
done

if [ -s err.txt ]
then
	echo "TESTS FAILED"
else
	echo "Tests Passed"
fi ;

rm -f testing.txt errors.txt
