<p>
	This is the specification of the <code>.map</code> file format used to store/load
	maps (clusters of objects).

<pre>byte 0-3: spec version</pre>

<p>
	The rest is dependent on the spec version:
<ul>
	<li>
		<a href=#v3>Version 0x0350414D (MAP3)</a>
	<li>
		<a href=#old>Old versions</a>
		<ul>
			<li><a href=#v2>Version 0x0250414D (MAP2)</a>
			<li><a href=#v1>Version 0x1</a>
		</ul>
</ul>

<h3 id=v3>Version 0x0350414D (MAP3)</h3>

<p>
	v3 added material replacements (only textures for now).
<p>
	The size of an object is now variable, so the zones are now positioned before
	the objects. Total object size is placed in the header. Object data is now the
	full data that would be used in the RPC packet.

<pre>byte 4-7: number of removed buildings [A]
byte 8-11: number of objects [B]
byte 12-15: total size of object data [D]
byte 16-19: number of gangzones [C]
byte 20-23: stream in radius (FLOAT)
byte 24-27: stream out radius (FLOAT)
byte 28-31: object drawdistance (FLOAT)
-----------
size 32 bytes

&gt; removed building, section repeated [A] times:
byte 0-3: model (DWORD)
byte 4-7: x (FLOAT)
byte 8-11: y (FLOAT)
byte 12-15: z (FLOAT)
byte 16-19: radius (FLOAT)
-----------
size 20 bytes

&gt; gangzones, section repeated [C] times:
byte 0-3: west/minx (FLOAT)
byte 4-7: south/miny (FLOAT)
byte 8-11: east/maxx (FLOAT)
byte 12-15: north/maxy (FLOAT)
byte 16-19: colorABGR (DWORD)
-----------
size 20 bytes

&gt; objects, section repeated [B] times:
byte 0-1: size of object data (WORD) (can be reused for objectid in RPC)
byte 2-5: model (DWORD)
byte 6-9: x (FLOAT)
byte 10-13: y (FLOAT)
byte 14-17: z (FLOAT)
byte 18-21: rx (FLOAT)
byte 22-25: ry (FLOAT)
byte 26-29: rz (FLOAT)
byte 30-33: draw distance (FLOAT) (should be same as the one in header)
byte 34: no camera col (BYTE) (should always be 0)
byte 35-36: attached object id (WORD) (should always be -1)
byte 37-38: attached vehicle id (WORD) (should always be -1)
byte 39: number of materials (BYTE)
(repeated [byte39] times):
	byte 0: material type (BYTE) (1 = texture, 2 = font)
	byte 1: material index (BYTE) (0-15)
	(when material type is 1 aka texture):
		byte 2-3: model id (WORD) (of the object that has the texture)
		byte 4: length of txdname string (BYTE)
		byte 5-x: txdname string (non zero term)
		byte x: length of texture name string (BYTE)
		byte x-x: texture name string (non zero term)
		byte x-x: material color ARGB (DWORD)
	(when material type is 2 aka text):
		(unsupported)
-----------
size VARIABLE bytes

total file size:
32 + 20 * num_removed_buildings[A] + 20 * num_gangzones[C] + objectdata_size[D]
</pre>

<h3 id=old>Old versions</h3>

<h4 id=v2>Version 0x0250414D (MAP2)</h4>

<p>
	v2 changed spec number from just a number to a proper 4-byte magic sequence because
	we're cool kids now. The fourth byte (index 3) is the actual version number.
<p>
	Per-object draw distance in v1 was never used, so now it's global.
	Stream in/out radius was added, because it should be stored together with the
	map instead of in a db. No more "negative model number equals a removed building
	annoyances".
<p>
	Gang zones were added. Their data is the same data as what the ShowGangZone RPC needs.

<pre>byte 4-7: number of removed buildings [A]
byte 8-11: number of objects [B]
byte 12-15: number of gangzones [C]
byte 16-19: stream in radius (FLOAT)
byte 20-23: stream out radius (FLOAT)
byte 24-27: object drawdistance (FLOAT)
-----------
size 28 bytes

&gt; removed building, section repeated [A] times:
byte 0-3: model (DWORD)
byte 4-7: x (FLOAT)
byte 8-11: y (FLOAT)
byte 12-15: z (FLOAT)
byte 16-19: radius (FLOAT)
-----------
size 20 bytes

&gt; objects, section repeated [B] times:
byte 0-3: model (DWORD)
byte 4-7: x (FLOAT)
byte 8-11: y (FLOAT)
byte 12-15: z (FLOAT)
byte 16-19: rx (FLOAT)
byte 20-23: ry (FLOAT)
byte 24-27: rz (FLOAT)
-----------
size 28 bytes

&gt; gangzones, section repeated [C] times:
byte 0-3: west/minx (FLOAT)
byte 4-7: south/miny (FLOAT)
byte 8-11: east/maxx (FLOAT)
byte 12-15: north/maxy (FLOAT)
byte 16-19: colorABGR (DWORD)
-----------
size 20 bytes

total file size:
28 + 20 * num_removed_buildings + 28 * num_objects + 20 * num_gangzones
</pre>

<h4 id=v1>Version 0x1</h4>

<ul>
	<li>remove buildings
	<li>per-object drawdistance
	<li>the object data is a subset of the data needed for the CreateObject RPC
	<li>the remove building data is the same as the data needed for the RemoveBuilding RPC
</ul>

<pre>byte 4-7: number of removed buildings
byte 8-11: number of objects
(repeated x times from byte 12):
	byte 0-3: model (DWORD)
	(when model is a negative number: remove building):
		byte 4-7: x (FLOAT)
		byte 8-11: y (FLOAT)
		byte 12-15: z (FLOAT)
		byte 16-19: radius (FLOAT)
		-----------
		size 16 (+4 model)
	(when model is a positive number: object):
		byte 4-7: x (FLOAT)
		byte 8-11: y (FLOAT)
		byte 12-15: z (FLOAT)
		byte 16-19: rx (FLOAT)
		byte 20-23: ry (FLOAT)
		byte 24-27: rz (FLOAT)
		byte 28-31: drawdistance (FLOAT)
		-----------
		size 28 (+4 model)
</pre>
