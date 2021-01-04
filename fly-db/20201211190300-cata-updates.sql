UPDATE apt SET x=-33563.703125,y=-1231.510498 WHERE i=9;

DELETE FROM msp WHERE
	(a = 9 AND name='cargo A') OR
	(a = 9 AND name='cargo B') OR
	(a = 9 AND name='cargo C') OR
	(a = 9 AND name='cargo H') OR
	(a = 9 AND name='H1') OR
	(a = 9 AND name='H2');

INSERT INTO msp(a,x,y,z,t,name) VALUES
	(9,-34162.1094,-1370.3408,8.4110,2,'gate P1'),
	(9,-34191.5508,-1359.9093,8.4110,2,'gate P2'),
	(9,-34220.9727,-1349.4296,8.4110,2,'gate P3'),
	(9,-34225.9844,-1400.4791,10.821,128,'cargo H'),
	(9,-34190.9492,-1413.2706,10.821,64,'H1'),
	(9,-33824.0938,-1456.2094,8.4110,8|16|32,'cargo B'),
	(9,-33683.6680,-1507.3109,8.4110,8|16|32,'cargo A');
