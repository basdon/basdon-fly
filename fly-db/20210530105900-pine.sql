INSERT INTO apt(i,c,e,n,x,y,z,flags) VALUES (13,'PINE',1,'Angel Pine Airstrip',-2605.7908,-2302.7644,14.3184,1);

INSERT INTO map(ap,filename) VALUES
	(13,'pine_minimap'),
	(13,'pine_lod');

INSERT INTO msp(a,x,y,z,t,name) VALUES
	(13,-2648.2097,-2321.5491,13.4546,1|2,'gate 1'),
	(13,-2668.0027,-2301.0564,13.4546,8|16,'cargo'),
	(13,-2605.7908,-2302.7644,14.3184,64,'helipad');

INSERT INTO rnw(a,i,s,h,x,y,z,w,n,type,surf) VALUES
	(13,0,'',135,-2755.1384,-2163.4912,13.6832,35,6,1,1),
	(13,0,'',315,-2627.4670,-2295.6575,13.7052,35,6,1,1),
	(13,1,'H',0,-2605.7908,-2302.7644,14.3184,25,0,2,3);

INSERT INTO svp(apt,x,y,z) VALUES
	(13,-2644.5605,-2301.1353,13.4546);

INSERT INTO art(cat,name,title) VALUES (0,'Angel_Pine_Airstrip','Angel Pine Airstrip');

INSERT INTO artalt(art,alt) VALUES ((SELECT id FROM art WHERE name='Angel_Pine_Airstrip'),'PINE');
