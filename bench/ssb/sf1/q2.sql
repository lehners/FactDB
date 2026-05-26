-- SSB Query 21
\set queryname ssb_sf1_query_21
\show queryname
-- Result size: 44532
select count(*)
from lineorder, "date", part, supplier
where lo_orderdate = d_datekey
  and lo_partkey = p_partkey
  and lo_suppkey = s_suppkey
  and p_category = 'MFGR#12'
  and s_region = 'AMERICA';
-- group by d_year, p_brand1
-- order by d_year, p_brand1

-- SSB Query 22
\set queryname ssb_sf1_query_22
\show queryname
-- Result size: 10513
select count(*)
from lineorder, "date", part, supplier
where lo_orderdate = d_datekey
  and lo_partkey = p_partkey
  and lo_suppkey = s_suppkey
  and p_brand1 between 'MFGR#2221'
    and 'MFGR#2228'
  and s_region = 'ASIA';

-- SSB Query 23
\set queryname ssb_sf1_query_23
\show queryname
-- Result size: 1277
select count(*)
from lineorder, "date", part, supplier
where lo_orderdate = d_datekey
  and lo_partkey = p_partkey
  and lo_suppkey = s_suppkey
  and p_brand1= 'MFGR#2239'
  and s_region = 'EUROPE';