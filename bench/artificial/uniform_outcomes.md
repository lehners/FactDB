
# diag:
- growing input size, but range stays the same for all tuples with $[0,1000]$
- i.e. from 10 duplicates bis 200 duplicates
- mehr duplicates heißt mehr children in Factorized Tree (1:1 mapping)
- i.e. obvious, dass BI & TI für wachsende #duplicates im Vergleich besser werden muss
- Warum ist BI schlechter als TI?
  - TI kann pipelined werden und braucht nur in top-level node locks
  - BI kann nicht pipelined werden (Full Pipeline Breaker, da zuerst eingefügt werden muss und dann im nächsten Schritt HT erstellt werden muss)
  - BI braucht locks/parallelisierung der Listen => overhead
- offene Fragen:
  - wie ist der overhead aufgeteilt zw. BI und TI? ist der BI lookup + insert effektiv identisch zu TI lookup + insert?
  - Was passiert zwischen 0-25 duplicaten, wenn langsam Factorized besser als Flat wird? => s. scale Diagramm

# scale:
- all have size 100.000, duplicates goes from 1 to 30
  - < 8 dups flat wins
  - \> 8 fact is preferable
  - BI always a bit worse, but wins from 20 and more against flat
  - what happens at other relation sizes? how does this influence the break even point?
  - for < 1:1 joins obviously flat besser
  - Ergebnisse grundsätzlich wie erwartet
- all have size 10.000, duplicates goes from 1 to 30
  - similar to 100.000
  - break even at 10 for TI, 22 for BI. => bit smaller, but not much
  - nothing unexpected
- all have size 1.000, duplicates goes from 1 to 30
  - mostly same as before
  - TI still 10, BI about 25 => BI growing input size => shrinking break even point
- all have size 1.000.000, dups same:
  - break even TI similar
  - break even BI smaller (12)
- BI break even grows (for some reason) while TI stays almost the same (but also slowly growing)

# How does this compare to real world data? 
- i.e. ce benchmark (dblp for now)
- what are the #duplicates per relation in real world? => dblp10
  - duplicates calculation by plotTableStats.py 
  - total: 54 attributes, >1 dups: 36 cases, >10 dups in 12 cases, >100: 6
    - => few cases, where BI should be really superior(?)
    - BUT: still BI may be better, since there may be already accumulated effects from previous joins
  - duplicates alone seem not to be the only cause why factorization is better or worse
    - but already a good implicator
    - for Q210_15, the for all but 1 relation the median # of duplicates is 1, but the maximum is quite high
    - also median and min do not completely show, why some benchmarks perform better than others, e.g. for Q15
  - total selectivity (query output / cross product size) is more or less random
  - more detailed insights if we look not at total runtime but division into different operators:
    - obviously is first copy of table very expensive => solvable by checking for this special case
    - joins where i do not profit from factorization only incur overhead => cannot be equalized later by cheaper joins
      - future work: do flat and factorized execution interleaved
        - only start factorization as soon as we know we benefit from it
        - decide at compile time what to do for which join
      - large overhead in repeated computation of count(*)
        - should be significantly reducable by caching the sizes of the tree rooted at some iterator
        - alternatively: only compute the result once and return in the next steps the cached result
        - 

- how do more joins influence the output?

# cyclic case:
- how does this affect the results?

# compare to FDB:
- need data for size up to 100 => more duplicates, even better for factorization

# TLS Dequeu is slow?
- push_back in deque seems to be expensive
- needed properties:
  - parallel inserts => thread_local
  - pointer stability, i.e. after insert & resize pointers must not change
  - (fast) iterator
  - size
