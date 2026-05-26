COPY customer  from 'bench/data/ssb/sf10/customer.tbl'   DELIMITER '|';
COPY "date"    from 'bench/data/ssb/sf10/date.tbl'       DELIMITER '|';
COPY part      from 'bench/data/ssb/sf10/part.tbl'       DELIMITER '|';
COPY supplier  from 'bench/data/ssb/sf10/supplier.tbl'   DELIMITER '|';
COPY lineorder from 'bench/data/ssb/sf10/lineorder.tbl'  DELIMITER '|';
