INSERT INTO apt(i,c,e,n,x,y,z,flags) VALUES (9,'CATA',1,'Catalina City','-34028.80','-1443.34','8.32',1);

INSERT INTO map(ap,filename) VALUES
	(9,'cata_detail'),
	(9,'cata_lod'),
	(9,'cata_minimap'),
	(9,'cata_rnw');

INSERT INTO svp(apt,x,y,z) VALUES
	(9,-34284.5586,-1255.1853,8.3909),
	(9,-34305.0469,-1314.6583,8.4179),
	(9,-34289.7148,-1544.3190,8.4641);

INSERT INTO msp(a,x,y,z,t,name) VALUES
	(9,-33849.0195,-1524.8126,8.4110,8|16,'cargo A'),
	(9,-33683.6680,-1507.3109,8.4110,32,'cargo B'),
	(9,-33824.0938,-1456.2094,8.4110,32,'cargo C'),
	(9,-33705.2500,-1576.8508,10.8216,128,'cargo H'),
	(9,-34225.9844,-1400.4791,10.8216,64,'H2'),
	(9,-34190.9492,-1413.2706,10.8216,64,'H1'),
	(9,-34088.0000,-1356.2286,8.4110,4,'gate 1'),
	(9,-34042.7266,-1373.7186,8.4110,4,'gate 2'),
	(9,-33963.1133,-1403.4291,8.4110,4,'gate 3'),
	(9,-33918.5742,-1417.0493,8.4110,4,'gate 4'),
	(9,-34191.5508,-1359.9093,8.4110,2,'gate P3'),
	(9,-34162.1094,-1370.3408,8.4110,2,'gate P2'),
	(9,-34220.9727,-1349.4296,8.4110,2,'gate P1');

INSERT INTO rnw(a,i,s,h,x,y,z,w,n,type,surf) VALUES
	(9,0,'',290,-33586.88,-1342.56,8.74,40,6,1,1),
	(9,0,'',110,-34291.14,-1086.56,8.74,40,6,1,1),
	(9,1,'H',0,-33705.2500,-1576.8508,10.8216,25,0,2,3),
	(9,2,'H',0,-34225.9844,-1400.4791,10.8216,25,0,2,3),
	(9,3,'H',0,-34190.9492,-1413.2706,10.8216,25,0,2,3);

INSERT INTO art(cat,name,title) VALUES (0,'Catalina_City','Catalina City');

INSERT INTO artalt(art,alt) VALUES ((SELECT id FROM art WHERE name='Catalina_City'),'CATA');
