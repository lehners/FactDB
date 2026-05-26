CREATE TABLE KRelationTest
(
    a integer,
    b integer
);
CREATE TABLE KRelationTestThree
(
    a integer,
    b integer,
    c integer
);

CREATE TABLE KPersonKnowsPerson
(
    creationDate timestamp,
    person1      uint64,
    person2      uint64
);
CREATE TABLE KPerson
(
    creationDate Timestamp,
    idx          UInt64,
    last_name    Varchar(1024),
    first_name   Varchar(1024),
    gender       Varchar(1024),
    birthday     Date,
    location_ip  Varchar(1024),
    browser_used Varchar(1024),
    city_id      Integer,
    speaks       Varchar(1024),
    email        Varchar(1024)
);