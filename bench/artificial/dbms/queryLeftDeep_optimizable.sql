SELECT count(*)
FROM artificial1 artificial1, artificial2 artificial2, artificial3 artificial3
WHERE artificial2.b = artificial3.a AND artificial1.b = artificial2.a;
