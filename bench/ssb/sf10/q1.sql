-- SSB Query 11
\set queryname ssb_sf10_query_11
\show queryname
-- Result size: 1193001
select count(*)
from lineorder, "date"
where lo_orderdate = d_datekey
  and d_year = 1993
  and lo_discount between 1 and 3
  and lo_quantity < 25;

-- SSB Query 12
\set queryname ssb_sf10_query_12
\show queryname
-- Result size: 42209
select count(*)
from lineorder, "date"
where lo_orderdate = d_datekey
  and d_yearmonthnum = 199401
  and lo_discount between 4 and 6
  and lo_quantity between 26 and 35;

-- SSB Query 13
\set queryname ssb_sf10_query_13
\show queryname
-- Result size: 9488
select count(*)
from lineorder, "date"
where lo_orderdate = d_datekey
  and d_weeknuminyear = 6
  and d_year = 1994
  and lo_discount between 5 and 7
  and lo_quantity between 26 and 35;

