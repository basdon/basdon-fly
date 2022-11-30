INSERT INTO apt(i,c,e,n,x,y,z,flags) VALUES (16,'MORR',1,'Morrison Airport',-15635.2480,11895.0273,15.6505,1);

INSERT INTO map(ap,filename) VALUES
	(16,'morr_minimap'),
	(16,'morr_lod'),
	(16,'morr_detail'),
	(16,'morr_airportdetail'),
	(16,'morr_towerdetail');

INSERT INTO msp(a,x,y,z,t,name) VALUES
	(16,-15450.0000,12148.7764,3.8,4,'gate 1'),
	(16,-15450.0000,12084.2627,3.8,4,'gate 2'),
	(16,-15450.0000,12019.0313,3.8,4,'gate 3'),
	(16,-15405.1934,12254.3164,6.4,1|2,'gate P1'),
	(16,-15450.0000,11920.2480,3.8000,8|16|32,'cargo A'),
	(16,-15527.3691,12334.0645,8.3,128,'cargo H'),
	(16,-15508.3838,12366.9775,8.3,64,'H1');

INSERT INTO rnw(a,i,s,h,x,y,z,w,n,type,surf) VALUES
	(16,0,'',30,-15814.7646,12104.0713,6.68,40,6,1,1),
	(16,0,'',210,-15346.7480,12914.8066,6.68,40,6,1,1),
	(16,1,'H',0,-15527.3691,12334.0645,8.3,25,0,2,3),
	(16,2,'H',0,-15508.3838,12366.9775,8.3,25,0,2,3);

INSERT INTO svp(apt,x,y,z) VALUES
	(16,-15664.4395,12055.1426,6.5000), -- 1
	(16,-15290.0625,11934.2559,3.2027); -- gasstation

INSERT INTO art(cat,name,title) VALUES (0,'Morrison_Airport','Morrison Airport');

INSERT INTO artalt(art,alt) VALUES ((SELECT id FROM art WHERE name='Morrison_Airport'),'MORR');
