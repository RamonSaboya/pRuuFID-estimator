#include <bits/stdc++.h>
#include "gnuplot.h"
using namespace std;
Gnuplot g;
void init() {
	g.set_title("pRuuFID");
	g.set_grid();
	g.cmd("set terminal pngcairo");
	g.cmd("set output 'out.png'");
}
int main() {
	init();
	g.cmd("plot x");
	return 0;
}
