set terminal pngcairo size 1200,700
set output "output/chart.png"

set datafile separator ","
set grid
set multiplot layout 2,1

# Candles
plot "output/candles.csv" using 1:3:4:2:5 with candlesticks

# Volume
plot "output/candles.csv" using 1:6 with boxes

unset multiplot
