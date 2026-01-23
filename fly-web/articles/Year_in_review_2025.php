<style>
/*copied so if it ever changes in the future, it will keep looking like it did in 2025*/
.flight-state-2025 {
	padding: .1em .4em;
	border-radius: 3px;
	font-family: monospace;
	white-space: nowrap;
}
.airportlist-2025 div {
	display: inline-block;
	padding: .5em;
	margin: .5em;
	font-size: 0.8em;
	text-align: center;
	border: 1px solid #161A21;
	background: #FAEBD7;
}
.airportlist-2025 img {
	width: 100px;
	border: 1px solid #000;
}
</style>

<p>
	Basdon launched on December 25th, 2019. This means we have completed Basdon's
	5th year in 2025. Let's review this year.

<h3>Flights</h3>

<!--ughh yes I realize I'm missing a day with these << comparisons but I don't want to redo all this okay :) (i wrote this when i only had three stats yet but shh)-->
<!--SELECT COUNT(*) FROM flg WHERE UNIX_TIMESTAMP('2024-12-25 00:00:00') < tload AND tload < UNIX_TIMESTAMP('2025-12-25 00:00:00')-->
<p>
	<strong>3460 flights</strong> started this year, which is <strong>24.5% of all flights</strong> recorded.
<!--SELECT AVG(adistance), MAX(adistance), AVG(distance), MAX(distance) FROM flg WHERE UNIX_TIMESTAMP('2024-12-25 00:00:00') < tload AND tload < UNIX_TIMESTAMP('2025-12-25 00:00:00')-->
<!--SELECT SUM(adistance) FROM flg WHERE UNIX_TIMESTAMP('2024-12-25 00:00:00') < tload AND tload < UNIX_TIMESTAMP('2025-12-25 00:00:00') -->
<p>
	Average point-to-point distance was <strong>12.9Km</strong> while the average actual distance flown was <strong>15.6Km</strong>.
	Together, you all flew for a distance of <strong>54,019Km</strong> during missions.
<p>
	The longest current route is <strong>CATA to STCL</strong> with a whopping distance of <strong>59Km</strong>.
<p>
	The longest flight performed this year was <a href="flight.php?id=13768">flight #13768</a> with <strong>78.6Km</strong>.

<h3>Players</h3>

<!--SELECT COUNT(*) FROM usr WHERE UNIX_TIMESTAMP('2024-12-25 00:00:00') < registertime AND registertime < UNIX_TIMESTAMP('2025-12-25 00:00:00') GROUP BY pw != ''-->
<p>
	<strong>311 new accounts</strong> were registered this year, and an additional <strong>247 guest sessions</strong> were played.
<!--SELECT COUNT(*) FROM usr WHERE registertime < UNIX_TIMESTAMP('2024-12-25 00:00:00') AND lastseengame > UNIX_TIMESTAMP('2024-12-25 00:00:00')-->
<p>
	<strong>38 existing players</strong> registered before this year but came back in 2025 to fly with us.
<!--SELECT COUNT(*) FROM ses WHERE UNIX_TIMESTAMP('2024-12-25 00:00:00') < s AND s < UNIX_TIMESTAMP('2025-12-25 00:00:00')-->
<!--SELECT SUM(e-s)/3600 FROM ses WHERE UNIX_TIMESTAMP('2024-12-25 00:00:00') < s AND s < UNIX_TIMESTAMP('2025-12-25 00:00:00')-->
<p>
	All together, you played during <strong>2825 sessions</strong> for a total of <strong>1149 hours</strong>.

<!--SELECT usr.name, SUM(ses.e-ses.s)/3600 c FROM ses JOIN usr ON ses.u = usr.i WHERE UNIX_TIMESTAMP('2024-12-25 00:00:00') < ses.s AND ses.s < UNIX_TIMESTAMP('2025-12-25 00:00:00') GROUP BY usr.name ORDER BY c DESC-->
<p>
	Top pilots by online time (this includes idle/afk time) this year were:
<ul>
	<li><strong>Christian_Hernandez</strong> (479 hours),
	<li><strong>KWUNMUEANG_XD</strong> (162 hours),
	<li><strong>DrLJY</strong> (57 hours),
	<li><strong>juasemoc</strong> (29 hours),
	<li><strong>HeldrickCL</strong> (29 hours).
</ul>

<!--SELECT usr.name, COUNT(*) c FROM flg JOIN usr ON usr.i = flg.player WHERE UNIX_TIMESTAMP('2024-12-25 00:00:00') < tload AND tload < UNIX_TIMESTAMP('2025-12-25 00:00:00') GROUP BY usr.name ORDER BY c DESC-->
<p>
	Top pilots by flights this year were:
<ul>
	<li><strong>KWUNMUEANG_XD</strong> (878 flights),
	<li><strong>Christian_Hernandez</strong> (423 flights),
	<li><strong>juasemoc</strong> (156 flights),
	<li><strong>DrLJY</strong> (128 flights),
	<li><strong>_]rVF[.Ejik_</strong> (127 flights).
</ul>

<h3>Server</h3>

<!--SELECT (
	(SELECT COUNT(*) FROM ses WHERE UNIX_TIMESTAMP('2024-12-25 00:00:00') < s AND s < UNIX_TIMESTAMP('2025-12-25 00:00:00') AND versionstring = '0.3.DL-R1')
	/
	(SELECT COUNT(*) FROM ses WHERE UNIX_TIMESTAMP('2024-12-25 00:00:00') < s AND s < UNIX_TIMESTAMP('2025-12-25 00:00:00') AND versionstring IS NOT NULL)
)-->
<p>
	In August 2025 we added support for 0.3.DL clients. Since then, <strong>8.7%</strong> of game sessions were played with <strong>0.3.DL</strong>.

<!--SELECT COUNT(*) FROM heartbeat WHERE UNIX_TIMESTAMP('2024-12-25 00:00:00') < tlast AND tlast < UNIX_TIMESTAMP('2025-12-25 00:00:00') AND cleanexit=0-->
<p>
	The server <strong>crashed 57 times</strong>. I will look into this soon :)

<!--SELECT SUM(LEAST(UNIX_TIMESTAMP('2025-12-25 00:00:00'),tlast)-GREATEST(UNIX_TIMESTAMP('2024-12-25 00:00:00'),tstart)) FROM heartbeat WHERE UNIX_TIMESTAMP('2024-12-25 00:00:00') < tlast AND tlast < UNIX_TIMESTAMP('2025-12-25 00:00:00')-->
<!--31167952-->
<!--SELECT UNIX_TIMESTAMP('2025-12-25 00:00:00')-UNIX_TIMESTAMP('2024-12-25 00:00:00')-->
<!--31536000-->
<p>
	The server had an uptime of <strong>98.83%</strong>, which could be better.
<p>
	2025 saw more updates than 2024, and we aim to continue this trend. <strong>Three new locations</strong> were added in 2025 (compared to 0 in 2024+2023):
<div class=airportlist-2025>
	<div>
		<a href="article.php?title=Palomino_Creek_Airport"><img src=/s/articles/palo.jpg alt="" title=PALO></a><br>
		<a href="article.php?title=Palomino_Creek_Airport">Palomino Creek Airport</a>
	</div>
	<div>
		<a href="article.php?title=Santiago_de_Chile_City"><img src=/s/articles/stcl.jpg alt="" title=STCL></a><br>
		<a href="article.php?title=Santiago_de_Chile_City">Santiago de Chile City</a>
	</div>
	<div>
		<a href="article.php?title=Robinson_Airport"><img src=/s/articles/rbsn.jpg alt="" title=RBSN></a><br>
		<a href="article.php?title=Robinson_Airport">Robinson Airport</a>
	</div>
</div>


<h3>More detailed stats</h3>

<h4>Flight status</h4>

<!--SELECT COUNT(*), state FROM flg WHERE UNIX_TIMESTAMP('2024-12-25 00:00:00') < tload AND tload < UNIX_TIMESTAMP('2025-12-25 00:00:00') GROUP BY state-->
<table class=new style=text-align:right>
	<thead>
		<tr><th>Status<th>Count<th>Share
	<tbody>
		<tr><td><span class=flight-state-2025 style=background:#dfd>LANDED</span><td>2946<td>85.1%
		<tr><td><span class=flight-state-2025 style=background:#fdb>ABORTED</span><td>191<td>5.5%
		<tr><td><span class=flight-state-2025 style=background:#fdd>CRASHED</span><td>186<td>5.3%
		<tr><td><span class=flight-state-2025 style=background:#fdd>VEHICLE DESTROYED</span><td>40<td>1.1%
		<tr><td><span class=flight-state-2025 style=background:#ffd>PAUSED</span><td>33<td>&lt;1%
		<tr><td><span class=flight-state-2025 style=background:#fdd>DIED</span><td>32<td>&lt;1%
		<tr><td><span class=flight-state-2025 style=background:#edf>SERVER ERROR</span><td>13<td>&lt;1%
		<tr><td><span class=flight-state-2025 style=background:#fdb>CONNECTION LOST</span><td>11<td>&lt;1%
		<tr><td><span class=flight-state-2025 style=background:#fdd>PLAYER QUIT</span><td>8<td>&lt;1%
</table>

<h4>Flight destinations</h4>

<!--SELECT apt.n, COUNT(*) c, COUNT(*)/3460 FROM flg JOIN msp ON flg.tmsp = msp.i JOIN apt ON apt.i = msp.a WHERE UNIX_TIMESTAMP('2024-12-25 00:00:00') < tload AND tload < UNIX_TIMESTAMP('2025-12-25 00:00:00') GROUP BY apt.n ORDER BY c DESC-->
<table class=new style=text-align:right>
	<thead>
		<tr><th>Destination<th>Count<th>Share
	<tbody>
		<tr><td>Los Santos Airport<td>533<td>15%
		<tr><td>San Fierro Airport<td>454<td>13%
		<tr><td>Las Venturas Airport<td>385<td>11%
		<tr><td>Catalina City<td>256<td>7%
		<tr><td>Angel Pine Airstrip<td>228<td>7%
		<tr><td>Octavia Island<td>208<td>6%
		<tr><td>Sandy Dunes<td>194<td>6%
		<tr><td>Iguazu City<td>165<td>5%
		<tr><td>BNS Atlantic<td>140<td>4%
		<tr><td>Morrison Airport<td>136<td>4%
		<tr><td>Santa Rosa Airport<td>135<td>4%
		<tr><td>Palomino Creek Airport<td>130<td>4%
		<tr><td>San Fierro Carrier<td>99<td>3%
		<tr><td>Santiago de Chile City<td>96<td>3%
		<tr><td>Windmill Resort<td>92<td>3%
		<tr><td>Area 69<td>87<td>3%
		<tr><td>Caligula's Palace<td>62<td>2%
		<tr><td>Los Santos Inlet Field<td>38<td>1%
		<tr><td>Robinson Airport<td>22<td>&lt;1%
</table>

<p>
	I am not including flight origin stats because currently spawn points are limited to LSA/SFA/LVA only, so this would be very skewed.

<h4>Flight type</h4>

<!--SELECT missiontype, COUNT(*) c, COUNT(*)/3460 FROM flg WHERE UNIX_TIMESTAMP('2024-12-25 00:00:00') < tload AND tload < UNIX_TIMESTAMP('2025-12-25 00:00:00') GROUP BY missiontype ORDER BY c DESC-->
<table class=new style=text-align:right>
	<thead>
		<tr><th>Type<th>Count<th>Share
	<tbody>
		<tr><td>Passengers (large quantity)<td>1743<td>50%
		<tr><td>Passengers (medium quantity)<td>859<td>25%
		<tr><td>Military<td>295<td>9%
		<tr><td>Helicopter (passengers)<td>145<td>4%
		<tr><td>Passengers (small quantity)<td>134<td>4%
		<tr><td>Cargo (large quantity)<td>114<td>3%
		<tr><td>Cargo (medium quantity)<td>95<td>3%
		<tr><td>Helicopter (military)<td>31<td>&lt;1%
		<tr><td>Cargo (small quantity)<td>26<td>&lt;1%
		<tr><td>Helicopter (cargo)<td>18<td>&lt;1%
</table>

<h4>Passenger satisfaction</h4>

<p>
	There were <strong>2881 passenger flights</strong>, accounting <strong>83% of all flights</strong> this year.

<!--SELECT satisfaction, COUNT(*) c, COUNT(*)/(SELECT COUNT(*) FROM flg WHERE (missiontype&0x47)!=0 AND UNIX_TIMESTAMP('2024-12-25 00:00:00') < tload AND tload < UNIX_TIMESTAMP('2025-12-25 00:00:00')) FROM flg WHERE (missiontype&0x47)!=0 AND UNIX_TIMESTAMP('2024-12-25 00:00:00') < tload AND tload < UNIX_TIMESTAMP('2025-12-25 00:00:00') GROUP BY satisfaction ORDER BY c DESC-->
<!--SELECT COUNT(*) c, COUNT(*)/(SELECT COUNT(*) FROM flg WHERE (missiontype&0x47)!=0 AND UNIX_TIMESTAMP('2024-12-25 00:00:00') < tload AND tload < UNIX_TIMESTAMP('2025-12-25 00:00:00')) FROM flg WHERE (missiontype&0x47)!=0 AND UNIX_TIMESTAMP('2024-12-25 00:00:00') < tload AND tload < UNIX_TIMESTAMP('2025-12-25 00:00:00') AND satisfaction > 0 AND satisfaction < 95 ORDER BY c DESC-->
<table class=new style=text-align:right>
	<thead>
		<tr><th>Satisfaction<th>Count<th>Share
	<tbody>
		<tr><td>100%<td>2031<td>70%
		<tr><td>99%<td>60<td>2%
		<tr><td>98%<td>40<td>1%
		<tr><td>97%<td>35<td>1%
		<tr><td>96%<td>28<td>1%
		<tr><td>95%<td>19<td>&lt;1%
		<tr><td>1%-94%<td>465<td>16%
		<tr><td>0%<td>203<td>7%
</table>

<p>
	All of this together means an <strong>average satisfaction of 87%</strong>.

<h4>Aircraft models</h4>

<!--SELECT veh.m, COUNT(*) c, COUNT(*)/3460 FROM flg JOIN veh ON flg.vehicle = veh.i WHERE UNIX_TIMESTAMP('2024-12-25 00:00:00') < tload AND tload < UNIX_TIMESTAMP('2025-12-25 00:00:00') GROUP BY veh.m ORDER BY c DESC-->
<table class=new style=text-align:right>
	<thead>
		<tr><th>Model<th>Count<th>Share
	<tbody>
		<tr><td>AT-400<td>1589<td>46%
		<tr><td>Shamal<td>556<td>16%
		<tr><td>Andromada<td>268<td>8%
		<tr><td>Nevada<td>266<td>8%
		<tr><td>Hydra<td>226<td>6%
		<tr><td>Dodo<td>160<td>5%
		<tr><td>Beagle<td>132<td>4%
		<tr><td>Rustler<td>69<td>2%
		<tr><td>Maverick<td>50<td>1%
		<tr><td>Raindance<td>44<td>1%
		<tr><td>Leviathan<td>34<td>1%
		<tr><td>Cargobob<td>31<td>1%
		<tr><td>Sparrow<td>24<td>&lt;1%
		<tr><td>News Chopper<td>11<td>&lt;1%
</table>

<h4>Busiest days by number of flights (CE(S)T timezone)</h4>
<!--SELECT CAST(FROM_UNIXTIME(tload) AS date) d,COUNT(*) x FROM flg WHERE UNIX_TIMESTAMP('2024-12-25 00:00:00') < tload AND tload < UNIX_TIMESTAMP('2025-12-25 00:00:00') GROUP BY CAST(FROM_UNIXTIME(tload) AS DATE) ORDER BY x DESC-->
<table class=new style=text-align:right>
	<thead>
		<tr><th>Day<th>Flights
	<tbody>
		<tr><td>2025-07-19 (Sat)<td>55
		<tr><td>2025-09-02 (Tue)<td>52
		<tr><td>2025-06-19 (Thu)<td>51
		<tr><td>2025-06-18 (Wed)<td>46
		<tr><td>2025-01-21 (Tue)<td>44
		<tr><td>2025-09-06 (Sat)<td>44
		<tr><td>2025-11-02 (Sun)<td>42
		<tr><td>2025-07-12 (Sat)<td>40
		<tr><td>2025-08-18 (Mon)<td>39
		<tr><td>2025-07-13 (Sun)<td>37
		<tr><td>2025-06-14 (Sat)<td>37
		<tr><td>2025-08-19 (Tue)<td>36
</table>

<h2>Here's to 2026!</h2>

<p>
	Thank you for flying with Basdon :)<br>- Robin
