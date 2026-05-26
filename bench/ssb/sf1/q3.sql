-- SSB Query 31
\set queryname ssb_sf1_query_31
\show queryname
-- Result size: 247638
select count(*)
from customer, lineorder, supplier, "date"
where lo_custkey = c_custkey
  and lo_suppkey = s_suppkey
  and lo_orderdate = d_datekey
  and c_region = 'ASIA'
  and s_region = 'ASIA'
  and d_year >= 1992 and d_year <= 1997
;

-- SSB Query 32
\set queryname ssb_sf1_query_32
\show queryname
-- Result size: 8541
select count(*)
from customer, lineorder, supplier, "date"
where lo_custkey = c_custkey
  and lo_suppkey = s_suppkey
  and lo_orderdate = d_datekey
  and c_nation = 'UNITED STATES'
  and s_nation = 'UNITED STATES'
  and d_year >= 1992 and d_year <= 1997
;

-- SSB Query 33
\set queryname ssb_sf1_query_33
\show queryname
-- Result size: 272
select count(*)
from customer, lineorder, supplier, "date"
where lo_custkey = c_custkey
  and lo_suppkey = s_suppkey
  and lo_orderdate = d_datekey
  and (c_city='UNITED KI1'
    or c_city='UNITED KI5')
  and (s_city='UNITED KI1'
    or s_city='UNITED KI5')
  and d_year >= 1992 and d_year <= 1997
;

-- SSB Query 34
\set queryname ssb_sf1_query_34
\show queryname
-- Result size: 5
select count(*)
from customer, lineorder, supplier, "date"
where lo_custkey = c_custkey
  and lo_suppkey = s_suppkey
  and lo_orderdate = d_datekey
  and (c_city='UNITED KI1'
    or c_city='UNITED KI5')
  and (s_city='UNITED KI1'
    or s_city='UNITED KI5')
  and d_yearmonth = 'Dec1997'
;
