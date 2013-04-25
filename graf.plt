# GNU PLOT SCRIPT / uloha 6 / OBRAZCE
set encoding utf8
set decimalsign ','
set grid x y
titulek = 'Měření #'.fn
set key inside top left box
set xlabel "t"
set ylabel "f [rpm]"
set terminal pngcairo enhanced font ',11'
set output './dumpy_img/'.fn.'.png'
plot './dumpy/'.fn.'.txt' using 1 with lines lw 2 title titulek