for %%G in (dumpy/*.txt) do "C:\Program Files (x86)\gnuplot\bin\gnuplot.exe" -e "fn='%%~nG'" graf.plt