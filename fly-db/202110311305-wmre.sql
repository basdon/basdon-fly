START TRANSACTION;

INSERT INTO apt(i,c,e,n,x,y,z,flags) VALUES (15,'WMRE',1,'Windmill Resort',-7430.4717,3147.4673,52.0822,1);

INSERT INTO map(ap,filename) VALUES
	(15,'wmre_minimap'),
	(15,'wmre_lod'),
	(15,'wmre_detail');

INSERT INTO msp(a,x,y,z,t,name) VALUES
	(15,-7480.5259,3122.3989,49.2800,1|2|8|16,'gate 1'),
	(15,-7287.0181,3183.5181,49.2800,0x40|0x80,'H1');

INSERT INTO rnw(a,i,s,h,x,y,z,w,n,type,surf) VALUES
	(15,0,'',250,-7255.5562,3128.2813,49.2592,40,6,1,1),
	(15,0,'',70, -7561.2114,3017.0366,49.2592,40,0,1,1),
	(15,1,'H',0,-7287.0181,3183.5181,49.2800,10,0,2,3);

INSERT INTO svp(apt,x,y,z) VALUES
	(15,-7375.1973,3156.4360,49.5428);

INSERT INTO art(cat,name,title) VALUES (0,'Windmill_Resort','Windmill Resort');

INSERT INTO artalt(art,alt) VALUES ((SELECT id FROM art WHERE name='Windmill_Resort'),'WMRE');

-- maverick
INSERT INTO veh(ap,m,x,y,z,r,col1,col2,inusedate) VALUES(15,487,-7311.7544,3179.3447,49.6528,18.9634,26,57,1635689181);
-- caddy
INSERT INTO veh(ap,m,x,y,z,r,col1,col2,inusedate) VALUES(15,457,-7554.8716,2882.8276,5.1764,-124.3712,2,1,1635689181);
-- quadbike
INSERT INTO veh(ap,m,x,y,z,r,col1,col2,inusedate) VALUES(15,471,-7548.5425,2889.7786,4.9926,-137.4662,120,117,1635689181);

COMMIT;
