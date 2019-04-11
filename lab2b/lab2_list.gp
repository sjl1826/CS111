#! /usr/local/cs/bin/gnuplot
#
# purpose:
#	 generate data reduction graphs for the multi-threaded list project
#
# input: lab2b_list.csv
#	1. test name
#	2. # threads
#	3. # iterations per thread
#	4. # lists
#	5. # operations performed (threads x iterations x (ins + lookup + delete))
#	6. run time (ns)
#	7. run time per operation (ns)
##

# general plot parameters
set terminal png
set datafile separator ","

#NUMBER 1
# how many threads/iterations we can run without failure (w/o yielding)
set title "Throughput of each Synchronization Method"
set xlabel "Threads"
set logscale x 2
unset xrange
set xrange [1:32]
set ylabel "Number of Operations per second"
set logscale y 10
set output 'lab2b_1.png'

plot \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'Mutex synchronized list operations' with linespoints lc rgb 'red', \
     "< grep 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'Spin-lock synchronized list operations' with linespoints lc rgb 'green'

#NUMBER 2
set title "Average time per operation and Wait for lock time against number of competing threads"
set xlabel "Threads"
set logscale x 2
set xrange [1:32]
set ylabel "Time"
set logscale y 10
set output 'lab2b_2.png'
# note that unsuccessful runs should have produced no output
plot \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($7) \
	title 'Average Time per Operation' with linespoints lc rgb 'green', \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($8) \
	title 'Wait for Lock Time' with linespoints lc rgb 'red'


#NUMBER 3
set title "Protected Iterations that run without failure"
set xlabel "Threads"
set logscale x 2
unset xrange
set xrange [1:16]
set ylabel "Iterations"
set logscale y 10
set output 'lab2b_3.png'
plot \
    "< grep 'list-id-none,[0-9]*,[0-9]*,4,' lab2b_list.csv" using ($2):($3) \
       title 'only yield' with points lc rgb 'green', \
    "< grep 'list-id-m,[0-9]*,[0-9]*,4,' lab2b_list.csv" using ($2):($3) \
       title 'yield with mutex' with points lc rgb 'red', \
    "< grep 'list-id-s,[0-9]*,[0-9]*,4,' lab2b_list.csv" using ($2):($3) \
       title 'yield with spin-lock' with points lc rgb 'blue'


#NUMBER 4
set title "Throughput of Mutex with Multiple Lists"
set xlabel "Threads"
set logscale x 2
unset xrange
set xrange [1:16]
set ylabel "Number of Operations per second"
set logscale y 10
set output 'lab2b_4.png'
plot \
    "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
       title 'Throughput with 1 list' with linespoints lc rgb 'green', \
    "< grep 'list-none-m,[0-9]*,1000,4,' lab2b_list.csv" using ($2):(1000000000/($7)) \
       title 'Throughput with 4 lists' with linespoints lc rgb 'red', \
    "< grep 'list-none-m,[0-9]*,1000,8,' lab2b_list.csv" using ($2):(1000000000/($7)) \
       title 'Throughput with 8 lists' with linespoints lc rgb 'purple', \
    "< grep 'list-none-m,[0-9]*,1000,16,' lab2b_list.csv" using ($2):(1000000000/($7)) \
       title 'Throughput with 16 lists' with linespoints lc rgb 'blue', \
       

#NUMBER 5
set title "Throughput of Spin-Lock with Multiple Lists"
set xlabel "Threads"
set logscale x 2
unset xrange
set xrange [1:16]
set ylabel "Number of Operations per second"
set logscale y 10
set output 'lab2b_5.png'
plot \
    "< grep 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
       title 'Throughput with 1 list' with linespoints lc rgb 'green', \
    "< grep 'list-none-s,[0-9]*,1000,4,' lab2b_list.csv" using ($2):(1000000000/($7)) \
       title 'Throughput with 4 lists' with linespoints lc rgb 'red', \
    "< grep 'list-none-s,[0-9]*,1000,8,' lab2b_list.csv" using ($2):(1000000000/($7)) \
       title 'Throughput with 8 lists' with linespoints lc rgb 'purple', \
    "< grep 'list-none-s,[0-9]*,1000,16,' lab2b_list.csv" using ($2):(1000000000/($7)) \
       title 'Throughput with 16 lists' with linespoints lc rgb 'blue'