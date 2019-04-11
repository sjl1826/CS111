# Total number of operations per second for each synchronization method.
# Mutex synchronized list operations, 1000 iterations at 1,2,4,8,12,16,24 threads
./lab2_list --threads=1 --iterations=1000 --sync=m >> lab2b_list.csv
./lab2_list --threads=2 --iterations=1000 --sync=m >> lab2b_list.csv
./lab2_list --threads=4 --iterations=1000 --sync=m >> lab2b_list.csv
./lab2_list --threads=8 --iterations=1000 --sync=m >> lab2b_list.csv
./lab2_list --threads=12 --iterations=1000 --sync=m >> lab2b_list.csv
./lab2_list --threads=16 --iterations=1000 --sync=m >> lab2b_list.csv
./lab2_list --threads=24 --iterations=1000 --sync=m >> lab2b_list.csv
# Spin-lock synchronized list operations. 1000 iterations at 1,2,4,8,12,16,24 threads
./lab2_list --threads=1 --iterations=1000 --sync=s >> lab2b_list.csv
./lab2_list --threads=2 --iterations=1000 --sync=s >> lab2b_list.csv
./lab2_list --threads=4 --iterations=1000 --sync=s >> lab2b_list.csv
./lab2_list --threads=8 --iterations=1000 --sync=s >> lab2b_list.csv
./lab2_list --threads=12 --iterations=1000 --sync=s >> lab2b_list.csv
./lab2_list --threads=16 --iterations=1000 --sync=s >> lab2b_list.csv
./lab2_list --threads=24 --iterations=1000 --sync=s >> lab2b_list.csv

#3
./lab2_list --yield=id --lists=4 --threads=1 --iterations=1 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=1 --iterations=2 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=1 --iterations=4 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=1 --iterations=8 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=1 --iterations=16 >> lab2b_list.csv

./lab2_list --yield=id --lists=4 --threads=4 --iterations=1 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=4 --iterations=2 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=4 --iterations=4 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=4 --iterations=8 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=4 --iterations=16 >> lab2b_list.csv

./lab2_list --yield=id --lists=4 --threads=8 --iterations=1 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=8 --iterations=2 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=8 --iterations=4 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=8 --iterations=8 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=8 --iterations=16 >> lab2b_list.csv

./lab2_list --yield=id --lists=4 --threads=12 --iterations=1 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=12 --iterations=2 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=12 --iterations=4 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=12 --iterations=8 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=12 --iterations=16 >> lab2b_list.csv

./lab2_list --yield=id --lists=4 --threads=16 --iterations=1 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=16 --iterations=2 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=16 --iterations=4 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=16 --iterations=8 >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=16 --iterations=16 >> lab2b_list.csv

./lab2_list --yield=id --lists=4 --threads=1 --iterations=10 --sync=s >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=1 --iterations=20 --sync=s >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=1 --iterations=40 --sync=s >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=1 --iterations=80 --sync=s >> lab2b_list.csv

./lab2_list --yield=id --lists=4 --threads=4 --iterations=10 --sync=s >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=4 --iterations=20 --sync=s >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=4 --iterations=40 --sync=s >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=4 --iterations=80 --sync=s >> lab2b_list.csv

./lab2_list --yield=id --lists=4 --threads=8 --iterations=10 --sync=s >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=8 --iterations=20 --sync=s >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=8 --iterations=40 --sync=s >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=8 --iterations=80 --sync=s >> lab2b_list.csv

./lab2_list --yield=id --lists=4 --threads=12 --iterations=10 --sync=s >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=12 --iterations=20 --sync=s >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=12 --iterations=40 --sync=s >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=12 --iterations=80 --sync=s >> lab2b_list.csv

./lab2_list --yield=id --lists=4 --threads=16 --iterations=10 --sync=s >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=16 --iterations=20 --sync=s >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=16 --iterations=40 --sync=s >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=16 --iterations=80 --sync=s >> lab2b_list.csv

./lab2_list --yield=id --lists=4 --threads=1 --iterations=10 --sync=m >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=1 --iterations=20 --sync=m >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=1 --iterations=40 --sync=m >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=1 --iterations=80 --sync=m >> lab2b_list.csv

./lab2_list --yield=id --lists=4 --threads=4 --iterations=10 --sync=m >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=4 --iterations=20 --sync=m >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=4 --iterations=40 --sync=m >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=4 --iterations=80 --sync=m >> lab2b_list.csv

./lab2_list --yield=id --lists=4 --threads=8 --iterations=10 --sync=m >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=8 --iterations=20 --sync=m >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=8 --iterations=40 --sync=m >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=8 --iterations=80 --sync=m >> lab2b_list.csv

./lab2_list --yield=id --lists=4 --threads=12 --iterations=10 --sync=m >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=12 --iterations=20 --sync=m >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=12 --iterations=40 --sync=m >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=12 --iterations=80 --sync=m >> lab2b_list.csv

./lab2_list --yield=id --lists=4 --threads=16 --iterations=10 --sync=m >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=16 --iterations=20 --sync=m >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=16 --iterations=40 --sync=m >> lab2b_list.csv
./lab2_list --yield=id --lists=4 --threads=16 --iterations=80 --sync=m >> lab2b_list.csv


#4
./lab2_list --lists=4 --threads=1 --iterations=1000 --sync=s >> lab2b_list.csv
./lab2_list --lists=8 --threads=1 --iterations=1000 --sync=s >> lab2b_list.csv
./lab2_list --lists=16 --threads=1 --iterations=1000 --sync=s >> lab2b_list.csv

./lab2_list --lists=4 --threads=2 --iterations=1000 --sync=s >> lab2b_list.csv
./lab2_list --lists=8 --threads=2 --iterations=1000 --sync=s >> lab2b_list.csv
./lab2_list --lists=16 --threads=2 --iterations=1000 --sync=s >> lab2b_list.csv

./lab2_list --lists=4 --threads=4 --iterations=1000 --sync=s >> lab2b_list.csv
./lab2_list --lists=8 --threads=4 --iterations=1000 --sync=s >> lab2b_list.csv
./lab2_list --lists=16 --threads=4 --iterations=1000 --sync=s >> lab2b_list.csv

./lab2_list --lists=4 --threads=8 --iterations=1000 --sync=s >> lab2b_list.csv
./lab2_list --lists=8 --threads=8 --iterations=1000 --sync=s >> lab2b_list.csv
./lab2_list --lists=16 --threads=8 --iterations=1000 --sync=s >> lab2b_list.csv

./lab2_list --lists=4 --threads=12 --iterations=1000 --sync=s >> lab2b_list.csv
./lab2_list --lists=8 --threads=12 --iterations=1000 --sync=s >> lab2b_list.csv
./lab2_list --lists=16 --threads=12 --iterations=1000 --sync=s >> lab2b_list.csv

#5
./lab2_list --lists=4 --threads=1 --iterations=1000 --sync=m >> lab2b_list.csv
./lab2_list --lists=8 --threads=1 --iterations=1000 --sync=m >> lab2b_list.csv
./lab2_list --lists=16 --threads=1 --iterations=1000 --sync=m >> lab2b_list.csv

./lab2_list --lists=4 --threads=2 --iterations=1000 --sync=m >> lab2b_list.csv
./lab2_list --lists=8 --threads=2 --iterations=1000 --sync=m >> lab2b_list.csv
./lab2_list --lists=16 --threads=2 --iterations=1000 --sync=m >> lab2b_list.csv

./lab2_list --lists=4 --threads=4 --iterations=1000 --sync=m >> lab2b_list.csv
./lab2_list --lists=8 --threads=4 --iterations=1000 --sync=m >> lab2b_list.csv
./lab2_list --lists=16 --threads=4 --iterations=1000 --sync=m >> lab2b_list.csv

./lab2_list --lists=4 --threads=8 --iterations=1000 --sync=m >> lab2b_list.csv
./lab2_list --lists=8 --threads=8 --iterations=1000 --sync=m >> lab2b_list.csv
./lab2_list --lists=16 --threads=8 --iterations=1000 --sync=m >> lab2b_list.csv

./lab2_list --lists=4 --threads=12 --iterations=1000 --sync=m >> lab2b_list.csv
./lab2_list --lists=8 --threads=12 --iterations=1000 --sync=m >> lab2b_list.csv
./lab2_list --lists=16 --threads=12 --iterations=1000 --sync=m >> lab2b_list.csv
