COPY customer  from 'bench/data/ssb/sf1/customer.tbl'   DELIMITER '|';
COPY "date"    from 'bench/data/ssb/sf1/date.tbl'       DELIMITER '|';
COPY part      from 'bench/data/ssb/sf1/part.tbl'       DELIMITER '|';
COPY supplier  from 'bench/data/ssb/sf1/supplier.tbl'   DELIMITER '|';
COPY lineorder from 'bench/data/ssb/sf1/lineorder.tbl'  DELIMITER '|';
