INSERT INTO apt(i,c,e,n,x,y,z,flags) VALUES (10,'IGZU',1,'Iguazu City','-3733.0','27709.0','4.43',1);

INSERT INTO map(ap,filename) VALUES
	(10,'igzu_minimap'),
	(10,'igzu_lod'),
	(10,'igzu_detail');

INSERT INTO svp(apt,x,y,z) VALUES
	(10,-3812.4841,27788.9160,5.1915),
	(10,-3893.3926,28117.5234,5.1915);

INSERT INTO msp(a,x,y,z,t,name) VALUES
	(10,-3859.5920,27892.9102,5.2000,8|16|32,'cargo C'),
	(10,-3990.3057,28071.2871,5.6923,64,'H1'),
	(10,-3907.7483,27932.7422,5.2000,4,'gate D'),
	(10,-3956.0513,27972.9609,5.2000,4,'gate E'),
	(10,-3983.0767,28018.9746,5.2000,2,'gate P1'),
	(10,-4012.3054,28043.8770,5.2000,2,'gate P2');

INSERT INTO rnw(a,i,s,h,x,y,z,w,n,type,surf) VALUES
	(10,0,'',310,-3653.7251,27461.6133,5.4196,40,6,1,1),
	(10,0,'',130,-4304.9761,28007.8281,5.4196,40,0,1,1),
	(10,1,'H',0,-3990.3057,28071.2871,5.6923,35,0,2,3);

INSERT INTO art(cat,name,title) VALUES (0,'Iguazu_City','Iguazú City');

INSERT INTO artalt(art,alt) VALUES ((SELECT id FROM art WHERE name='Iguazu_City'),'IGZU');
