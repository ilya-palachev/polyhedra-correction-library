set terminal jpeg size 1000,500
set xlabel 'Количество контуров' font ", 20"
set output "Reduced-constraints-ratio-graph.jpeg"
set xrange [0:205]
set yrange [0:0.4]
set xtics 10 font ", 16"
set ytics 0.1 font ", 16"
#set key font ", 20" spacing 2 outside
set title font ", 20"
set nokey
set grid;
show grid

plot "reduced-constraints-ratio.txt" using 1:2 with points pointtype 4 lw 2 pointsize 2 linecolor rgb "black", \
     "reduced-constraints-ratio.txt" using 1:2 with lines linetype -1 lw 2 notitle
