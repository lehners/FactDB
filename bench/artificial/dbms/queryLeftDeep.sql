
SELECT count(*)
FROM artificial1 artificial1 join artificial2 artificial2 on forceorder (artificial1.b = artificial2.a) join artificial3 artificial3 on forceorder (artificial2.b = artificial3.a);
