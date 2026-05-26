
# diag:
- seems to have completely different shape as uniform => caused by input?
  - sizes: 100, 1000, 2000, 3000, 5000, 10'000, 15'000, 20'000
  - 100 duplicates, maximum value is 100
  - FDB paper does similar things: 3 relations of 3 attributes each data with Zipf distribution over $[1 .. 100]$
    - from size 100 to 100'000
    - **todo** check if query is similar
    - 20k tuples we have 10ms vs FDB has 100ms => speedup by factor 10 **todo validate**
    - is my approach really so much better than SQLite? => todo check
  - nur steigung zwischen 100 und 1k ist größer, danach ziemlich ähnlich zu uniform
- interesting things seem to happen between 100 and 1.000

# scale
- upperbound growing from $\frac{\vert \text{relation} \vert}{0.0001=10^{-4}}=10^4\times \vert \text{relation} \vert $ to $\frac{\vert \text{relation} \vert}{0.01=10^{-2}} = 10^2 \vert \text{relation}\vert$
- size $10'000=10^4$:
  - Einordnung: $10^{2}$ should be same as 10'000 in diag => not plotted but data shows same results = good
 
 
 
- Wenn mehr Elemente in Relation wird obvious Laufzeit größer
- Wenn mehr Duplikate in Relationen,
  - wird zwischenergebnis größer => Laufzeit steigt
  - wird Endergebnis größer
  - => kleinerer Max-Value in zipf distribution
    - i.e. 0.01 hat kleineren max als 0.0001
  - wird Factorization besser im Vergleich
- break even point verschiebt sich bei kleineren Relationsgrößen
  - mehr Duplikate sind notwendig für BI to be better than flat
    - 1000 bei 0.6 (max val=1666) vs 10.000 bei 0.001 (max $10.000.000=10^7$)
    - 1k wahrscheinlich fast zu klein, weil sehr schnell (<=1ms)
    - 10k ~ 10ms
 
 
 
# todos 
- was sind akutell maximum & avg number of duplicates in den verschiedenen Benchmarks
  => wahrscheinlich im Case scale gibt es trzd noch recht viele Duplikate, weil Factorization noch sinnvoll / gut ist
- Aber warum ist dann BI am anfang noch relativ schlecht im Vergleich? ist dass nur wegen der zweiten Loop? 
  => try to get more insights about the different parts where we "loose" runtime
- Können wir insgesamt eig besser werden, wenn wir am Anfang nicht die ganzen Relation einmal kopieren? Falls ja, wie viel?
  - Wahrscheinlich aber eher weniger, weil man muss es ja sowieso einmal für den HT build machen. Oder reduiert sich das um round about 50%, weil ein Materialisierungsstep wegfällt? => todo check

# Wie schauen eig die DBLP tabellen aus?
  - max & avg number of duplicates
  - welche verteilungen? sind das uniforme verteilungen, normal, zipf, etc?
  - zipf similar to normal, oder? erzeugt beides somehow skew...
