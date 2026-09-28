set datafile separator ","
set terminal pngcairo size 1200,800
set output "mean_delay.png"
set title "Mean Packet Delay vs Arrival Rate"
set xlabel "Arrival rate (packets/second)"
set ylabel "Mean delay (ms)"
set grid
set key outside

plot "results.csv" using (($2 == 333333) ? $1 : 1/0):3 with linespoints title "seed 333333", \
     "results.csv" using (($2 == 444444) ? $1 : 1/0):3 with linespoints title "seed 444444", \
     "results.csv" using (($2 == 400470318) ? $1 : 1/0):3 with linespoints title "seed 400470318"

unset output
