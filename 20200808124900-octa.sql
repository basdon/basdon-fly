INSERT INTO map(ap,filename) VALUES
	(NULL,'octa_lod'),
	(NULL,'octa_vegasblock'),
	(NULL,'octa_lsblock'),
	(NULL,'octa_cargo'),
	(NULL,'octa_jetty'),
	(NULL,'octa_airportmarkings'),
	(NULL,'octa_airportbuilding'),
	(NULL,'octa_southbeach'),
	(NULL,'octa_minimap');

INSERT INTO apt(i,c,e,n,x,y,z,flags) VALUES (8,'OCTA',1,'Octavia Island','12908.988281','6961.109863','22.978376',1);

INSERT INTO svp(apt,x,y,z) VALUES
	(8,13317.2109,6512.1470,7.1800),
	(8,12860.4717,6600.4385,6.9235);

INSERT INTO msp(a,x,y,z,t,name) VALUES
	(8,13019.7363,6518.6304,6.2000,8|16|32,'cargo'),
	(8,13058.7334,6835.0693,6.2000,4,'gate B'),
	(8,13060.2881,6917.5283,6.2000,4,'gate A'),
	(8,13033.6758,6625.4536,6.2491,64|128,'H2'),
	(8,13033.8486,6660.3442,6.2490,64|128,'H1'),
	(8,13084.5049,7004.2681,6.2000,1,'hangar'),
	(8,13049.7500,6724.4097,6.2000,2,'gate P2'),
	(8,13050.3213,6757.9722,6.2000,2,'gate P1');

INSERT INTO veh(m,e,ap,x,y,z,r,col1,col2,inusedate) VALUES
	(495,1,8,12455.8984,6551.1514,4.7621,-178.9000,118,117,UNIX_TIMESTAMP()),
	(485,1,8,12985.0430,6613.6069,6.8300,-90.0000,1,73,UNIX_TIMESTAMP()),
	(431,1,8,12958.5234,6613.5913,7.4100,-180.0000,71,59,UNIX_TIMESTAMP()),
	(448,1,8,12759.4248,6745.8325,6.8071,-180.2471,1,1,UNIX_TIMESTAMP()),
	(431,1,8,12962.8418,6613.5928,7.4100,-180.0000,71,59,UNIX_TIMESTAMP()),
	(485,1,8,12985.0381,6617.2578,6.8300,-90.0000,1,73,UNIX_TIMESTAMP()),
	(608,1,8,12982.7871,6633.8228,7.6900,-180.0000,1,1,UNIX_TIMESTAMP()),
	(485,1,8,12985.0313,6620.5601,6.8300,-90.0000,1,73,UNIX_TIMESTAMP()),
	(606,1,8,12989.2197,6633.5762,7.3500,-180.0000,1,1,UNIX_TIMESTAMP()),
	(608,1,8,12979.7939,6633.8281,7.6900,-180.0000,1,1,UNIX_TIMESTAMP()),
	(487,1,8,13070.3135,6659.5088,7.3291,-90.0000,26,57,UNIX_TIMESTAMP()),
	(407,1,8,12915.4551,6997.6240,7.4900,0.0000,6,1,UNIX_TIMESTAMP()),
	(606,1,8,12986.3535,6633.5737,7.3500,-180.0000,1,1,UNIX_TIMESTAMP()),
	(593,1,8,13043.4180,7024.8638,7.7200,-180.0000,1,1,UNIX_TIMESTAMP());

INSERT INTO rnw(a,i,s,h,x,y,z,w,n,type,surf) VALUES
	(8,0,'',360,13214.9258,6339.9209,7.3037,40,6,1,1),
	(8,0,'',180,13214.5342,7001.1582,7.4658,40,6,1,1),
	(8,1,'',270,13315.5938,6319.9004,7.3110,40,6,1,1),
	(8,1,'',90,12644.4043,6319.7339,7.2981,40,6,1,1),
	(8,2,'H',0,13033.8037,6660.3354,7.2491,20,0,2,3),
	(8,3,'H',0,13033.6748,6625.6763,7.2491,20,0,2,3),
	(8,4,'H',0,13069.0713,6625.6050,7.2491,20,0,2,3),
	(8,5,'H',0,13068.9756,6659.8975,7.2491,20,0,2,3);

INSERT INTO art(cat,name,title) VALUES (0,'Octavia_Island','Octavia Island');

INSERT INTO artalt(art,alt) VALUES ((SELECT id FROM art WHERE name='Octavia_Island'),'OCTA');
