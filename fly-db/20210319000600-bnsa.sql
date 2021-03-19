INSERT INTO apt(i,c,e,n,x,y,z,flags) VALUES (11,'BNSA',1,'BNS Atlantic',4990.1753,-11051.1572,16.0,1);

INSERT INTO map(ap,filename) VALUES
	(11,'bnsa_minimap'),
	(11,'bnsa_lod');

INSERT INTO msp(a,x,y,z,t,name) VALUES
	(11,5021.3174,-11046.4424,15.6761,512,'M1'),
	(11,4878.7256,-11021.9063,15.6539,256,'MH1');

INSERT INTO rnw(a,i,s,h,x,y,z,w,n,type,surf) VALUES
	(11,0,'',80,4854.8013,-11050.7012,16.1277,14.5,2,1,1),
	(11,0,'',260,5008.0718,-11023.7148,16.0618,14.5,0,1,1),
	(11,1,'H',0,4878.7256,-11021.9063,15.6539,8,0,2,1);

INSERT INTO art(cat,name,title) VALUES (0,'BNS_Atlantic','BNS Atlantic');

INSERT INTO artalt(art,alt) VALUES ((SELECT id FROM art WHERE name='BNS_Atlantic'),'BNSA');
