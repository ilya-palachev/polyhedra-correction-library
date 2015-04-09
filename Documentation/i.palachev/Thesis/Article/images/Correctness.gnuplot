set terminal jpeg size 1000,1300
set xlabel 'Количество контуров' font ", 20"
set ylabel 'Величина функционала' font ", 20"
set output "Correctness.jpeg"
set xrange [0:105]
set xtics 10 font ", 16"
#set key font ", 20" spacing 2 outside
set title font ", 20"
set nokey
set grid;
show grid

plot "performance_report.1_GIA.ipopt.linf.txt" using 1:10 with points pointtype 6 pointsize 2 linecolor rgb "black" title "Ipopt L-infinity", \
     "performance_report.1_GIA.ipopt.linf.txt" using 1:10 with lines linetype -1 notitle, \
     "performance_report.1_GIA.ipopt.l1.txt" using 1:6 with points pointtype 7 pointsize 2 linecolor rgb "black" title "Ipopt L-1", \
     "performance_report.1_GIA.ipopt.l1.txt" using 1:6 with lines linetype -1 notitle, \
     "performance_report.1_GIA.ipopt.l2.txt" using 1:9 with points pointtype 1 pointsize 2 linecolor rgb "black" title "Ipopt L-2", \
     "performance_report.1_GIA.ipopt.l2.txt" using 1:9 with lines linetype -1 notitle, \
     "performance_report.1_GIA.cplex.linf.txt" using 1:10 with points pointtype 4 pointsize 2 linecolor rgb "black" title "CPLEX L-infinity", \
     "performance_report.1_GIA.cplex.linf.txt" using 1:10 with lines linetype -1 notitle, \
     "performance_report.1_GIA.cplex.l1.txt" using 1:6 with points pointtype 5 pointsize 2 linecolor rgb "black" title "CPLEX L-1", \
     "performance_report.1_GIA.cplex.l1.txt" using 1:6 with lines linetype -1 notitle, \
     "performance_report.1_GIA.ipopt.linf.unreduced.txt" using 1:10 with points pointtype 6 pointsize 2 linecolor rgb "black" title "Ipopt L-infinity", \
     "performance_report.1_GIA.ipopt.linf.unreduced.txt" using 1:10 with lines linetype 2 lc rgb "black" lw 2 notitle, \
     "performance_report.1_GIA.ipopt.l1.unreduced.txt" using 1:6 with points pointtype 7 pointsize 2 linecolor rgb "black" title "Ipopt L-1", \
     "performance_report.1_GIA.ipopt.l1.unreduced.txt" using 1:6 with lines linetype 2 lc rgb "black" lw 2 notitle, \
     "performance_report.1_GIA.ipopt.l2.unreduced.txt" using 1:9 with points pointtype 1 pointsize 2 linecolor rgb "black" title "Ipopt L-2", \
     "performance_report.1_GIA.ipopt.l2.unreduced.txt" using 1:9 with lines linetype 2 lc rgb "black" lw 2 notitle, \
     "performance_report.1_GIA.cplex.linf.unreduced.txt" using 1:10 with points pointtype 4 pointsize 2 linecolor rgb "black" title "CPLEX L-infinity", \
     "performance_report.1_GIA.cplex.linf.unreduced.txt" using 1:10 with lines linetype 2 lc rgb "black" lw 2 notitle, \
     "performance_report.1_GIA.cplex.l1.unreduced.txt" using 1:6 with points pointtype 5 pointsize 2 linecolor rgb "black" title "CPLEX L-1", \
     "performance_report.1_GIA.cplex.l1.unreduced.txt" using 1:6 with lines linetype 2 lc rgb "black" lw 2 notitle \


