MATCH
    (a1:artificial_keys)<-[n1:artificial1]-(a2:artificial_keys),
    (a2:artificial_keys)-[n2:artificial2]->(a3:artificial_keys),
    (a3:artificial_keys)-[n3:artificial3]->(a4:artificial_keys)
RETURN COUNT(*);

