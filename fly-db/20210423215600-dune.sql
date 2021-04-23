INSERT INTO apt(i,c,e,n,x,y,z,flags) VALUES (12,'DUNE',1,'Sandy Dunes',9596.1816,-3833.9109,15.4042,1);

INSERT INTO map(ap,filename) VALUES
	(12,'dune_minimap'),
	(12,'dune_lod'),
	(12,'dune_bushes'),
	(12,'dune_detail');

INSERT INTO svp(apt,x,y,z) VALUES
	(12,9659.5986,-3760.6890,11.4355);

INSERT INTO msp(a,x,y,z,t,name) VALUES
	(12,10083.3330,-3811.0000,11.4855,32,'cargo A'),
	(12,10145.2588,-3811.0000,11.4855,32,'cargo B'),
	(12,10209.3271,-3811.0000,11.4855,4,'gate C'),
	(12,10273.4766,-3811.0000,11.4855,4,'gate B'),
	(12,10337.1602,-3811.0000,11.4855,4,'gate A'),
	(12,10472.1074,-3798.7000,11.4855,1|2,'gate P1'),
	(12,10507.1973,-3798.7000,11.4855,1|2,'gate P2'),
	(12,10542.0391,-3798.7000,11.4855,8|16,'cargo C'),
	(12,10646.3359,-3794.3999,12.4300,64,'H1'),
	(12,10688.4023,-3794.3999,12.4300,64,'H2'),
	(12,10730.5293,-3794.3999,12.4300,128,'cargo H');

INSERT INTO rnw(a,i,s,h,x,y,z,w,n,type,surf) VALUES
	(12,0,'',90,9598.2949,-3964.0310,11.9314,37,6,1,1),
	(12,0,'',270,10745.0713,-3964.1069,11.9314,37,6,1,1),
	(12,1,'H',0,10646.3359,-3794.3999,12.4300,25,0,2,3),
	(12,2,'H',0,10688.4023,-3794.3999,12.4300,25,0,2,3),
	(12,3,'H',0,10730.5293,-3794.3999,12.4300,25,0,2,3);

INSERT INTO art(cat,name,title) VALUES (0,'Sandy_Dunes','Sandy Dunes');

INSERT INTO artalt(art,alt) VALUES ((SELECT id FROM art WHERE name='Sandy_Dunes'),'DUNE');
