INSERT INTO apt(i,c,e,n,x,y,z,flags) VALUES (17,'PALO',1,'Palomino Creek Airport',2370.26,-196.79,25.35,1);

INSERT INTO map(ap,filename) VALUES
	(17,'palo_minimap'),
	(17,'palo_lod'),
	(17,'palo_detail');

INSERT INTO msp(a,x,y,z,t,name) VALUES
	(17,2369.5266,-118.2473,28.7425,1|2|8|16,'gate 1'),
	(17,2333.9604,-164.9910,27.4549,64|128,'H1');

INSERT INTO rnw(a,i,s,h,x,y,z,w,n,type,surf) VALUES
	(17,0,'',90,2294.19,-196.83,25.35,20,0,1,3),
	(17,0,'',270,2447.41,-196.83,25.35,20,6,1,3),
	(17,1,'H',0,2333.9604,-164.9910,27.4549,20,0,2,3);

INSERT INTO svp(apt,x,y,z) VALUES
	(17,2392.7673,-100.2670,28.7440);

INSERT INTO art(cat,name,title) VALUES (0,'Palomino_Creek_Airport','Palomino Creek Airport');

INSERT INTO artalt(art,alt) VALUES ((SELECT id FROM art WHERE name='Palomino_Creek_Airport'),'PALO');
