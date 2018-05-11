#include <bits/stdc++.h>
#include "gnuplot.h"
using namespace std;
int main() {
	Gnuplot g;
	g.cmd("set terminal pngcairo");
	g.cmd("set output 'graph.png'");
	g.cmd("set style line 10 lc rgb 'black' lt 1 lw 1");
	g.cmd("set grid xtics ytics mxtics mytics ls 10");
	g.cmd("set mytics 1");
	g.cmd("set mxtics 1");
	g.cmd("set xr [100:1000]");
	g.cmd("set yr [0:140]");
	g.cmd("set xlabel 'Número de Etiquetas'");
	g.cmd("set ylabel 'Erro Abs. Médio de Estimação'");
	g.cmd("plot 'data.dat' u 1:2 w lp lw 2 pt 1 ps 2 t 'Lower Bound', 'data.dat' u 1:3 w lp lw 2 pt 5 ps 2 t 'Eom Lee'");
}
