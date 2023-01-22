-- enriched flight list
CREATE VIEW flg_enriched AS
SELECT id,_u.name AS name,_u.i AS i,_a.c AS f,_b.c AS t,state,tload,tlastupdate,adistance,_v.m AS vehmodel
FROM flg _f
JOIN usr _u ON _f.player=_u.i
JOIN apt _a ON _a.i=_f.fapt
JOIN apt _b ON _b.i=_f.tapt
JOIN veh _v ON _v.i=_f.vehicle
ORDER BY id DESC
