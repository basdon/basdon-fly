-- two rustlers at a69, in the hangars
INSERT INTO veh(m,e,ap,x,y,z,r,col1,col2,inusedate) VALUES
	(476,1,(SELECT i FROM apt WHERE c='BASE'),278.4334,1989.3662,18.3485,270.0,6,7,UNIX_TIMESTAMP()),
	(476,1,(SELECT i FROM apt WHERE c='BASE'),278.4334,1955.7488,18.3533,270.0,77,87,UNIX_TIMESTAMP());
