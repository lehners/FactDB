
-- SSB Query 41
\set queryname ssb_sf1_query_41
\show queryname
-- Result size: 89953
select count(*)
from "date", customer, supplier, part, lineorder
where lo_custkey = c_custkey
  and lo_suppkey = s_suppkey
  and lo_partkey = p_partkey
  and lo_orderdate = d_datekey
  and c_region = 'AMERICA'
  and s_region = 'AMERICA'
  and (p_mfgr = 'MFGR#1'
    or p_mfgr = 'MFGR#2');

-- SSB Query 42
\set queryname ssb_sf1_query_42
\show queryname
-- Result size: 21755
select count(*)
from "date", customer, supplier, part, lineorder
where lo_custkey = c_custkey
  and lo_suppkey = s_suppkey
  and lo_partkey = p_partkey
  and lo_orderdate = d_datekey
  and c_region = 'AMERICA'
  and s_region = 'AMERICA'
  and (d_year = 1997 or d_year = 1998)
  and (p_mfgr = 'MFGR#1'
    or p_mfgr = 'MFGR#2');

-- SSB Query 43
\set queryname ssb_sf1_query_43
\show queryname
-- Result size: 2194
select count(*)
from "date", customer, supplier, part, lineorder
where lo_custkey = c_custkey
  and lo_suppkey = s_suppkey
  and lo_partkey = p_partkey
  and lo_orderdate = d_datekey
  and s_nation = 'UNITED STATES'
  and (d_year = 1997 or d_year = 1998)
  and p_category = 'MFGR#14';
