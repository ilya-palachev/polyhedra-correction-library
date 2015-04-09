set terminal jpeg size 1000,500
set xlabel 'Количество контуров' font ", 20"
set ylabel 'Количество ограничений' font ", 20"
set output "Reduced-constraints-count-graph.jpeg"
set xrange [0:205]
set xtics 10 font ", 16"
set ytics 500000 font ", 16"
set format y "%.0f"
#set key font ", 20" spacing 2 outside
set title font ", 20"
set nokey
set grid;
show grid

plot "performance_report.1_GIA.cplex.linf.txt" using 1:11 with points pointtype 4 lw 2 pointsize 2 linecolor rgb "black" title "Общее число ограничений", \
     "performance_report.1_GIA.cplex.linf.txt" using 1:11 with lines linetype -1 lw 2 notitle, \
     "performance_report.1_GIA.cplex.linf.txt" using 1:12 with points pointtype 4 pointsize 2 linecolor rgb "black" title "Число неизбыточных ограничений", \
     "performance_report.1_GIA.cplex.linf.txt" using 1:12 with lines linetype -1 notitle
