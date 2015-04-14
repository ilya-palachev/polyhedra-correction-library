set terminal jpeg size 900,500
set xlabel 'Количество контуров' font ", 20"
set ylabel 'Время построения ограничений, с' font ", 20"
set output "Problem-preparation-time-graph.jpeg"
set xrange [0:205]
set xtics 10
set ytics 1
set key top left
set grid;
show grid

plot "performance_report.1_GIA.ipopt.linf.txt" u 1:3 w p pt 7 ps 2 linecolor rgb "black" notitle, \
"performance_report.1_GIA.ipopt.linf.txt" u 1:3 with lines notitle lt -1
