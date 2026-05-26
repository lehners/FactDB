
create node table artificial_keys(k int32, PRIMARY KEY(k));

create rel table artificial1 (FROM artificial_keys TO artificial_keys, idVal int32);
create rel table artificial2 (FROM artificial_keys TO artificial_keys, idVal int32);
create rel table artificial3 (FROM artificial_keys TO artificial_keys, idVal int32);

