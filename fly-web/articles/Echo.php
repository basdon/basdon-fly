<p>
	The echo service is the bridge between in-game chat and IRC chat. The IRC side
	of echo is implemented as an
	<a href=https://github.com/yugecin/anna>Anna^</a><img src=/s/moin-www.png alt="globe icon" title="external link">
	mod.
<p>
	Echo runs on ports <code>7767</code> (IRC) and <code>7768</code> (In-game).
<p>
	<strong>Note</strong>: When a range of bytes is mentioned (<code>X until Y</code>),
	the end of the range (<code>Y</code>) is meant to be exclusive.

<h3>Packet spec</h3>

<pre>byte 0: F
byte 1: L
byte 2: Y
byte 3: packet id
</pre>

<h4 id=p2>Packet 2: Hello</h4>

<p>
	A <code>Hello</code> packet is sent to the other side when the echo service comes
	online. Upon receiving, the other side should send a <a href=#p3>I'm there</a>
	packet back.

<pre>byte 4-7: random value (values 0 and 1 are forbidden)</pre>

<p>
	<strong>Note</strong>: IRC should also send a <a href=#p8>Status</a> packet when
	sending this packet. Game doesn't need to do this since nobody will be online when
	the game echo bridge starts.

<h4 id=p3>Packet 3: I'm there</h4>

<p>
	An <code>I'm there</code> packet is sent as a response to a
	<a href=#p2>Hello packet</a>.

<pre>byte 4-7: same as bytes 4-7 as received from the Hello packet</pre>

<p>
	<strong>Note</strong>: A <a href=#p8>Status</a> packet should also be sent when
	sending this packet.

<h4 id=p4>Packet 4: Bye</h4>

<p>
	Sent when an echo service is shutting down (either side).

<h4 id=p5>Packet 5: Ping</h4>

<p>
	Simple ping packet. Upon receiving, the other side should send a
	<a href=#p6>Pong</a> packet back with the same random value.

<pre>byte 4-7: random value (values 0 and 1 are forbidden)</pre>

<p>
	Currently, pings are only sent from IRC to game at a specified interval.
	IRC knows if game is down if they don't receive pongs, and game will know
	when IRC is down when it hasn't received a ping packet in the designated
	interval.

<h4 id=p6>Packet 6: Pong</h4>

<p>
	Response to a <a href=#p5>Ping</a> packet.

<pre>byte 4-7: same as bytes 4-7 as received from the Ping packet</pre>

<h4 id=p7>Packet 7: Status request</h4>

<p>
	Request a <a href=#p8>Status</a> packet. Usually this request is made when
	a user connects. The payload of the response is then sent to the user,
	so they can be informed about the Echo connection.

<h4 id=p8>Packet 8: Status</h4>

<p>
	Sent to respond to a <a href=#p7>Status request</a> packet, or accompanying
	either a <a href=#p2>Hello</a> or <a href=#p3>I'm there</a> packet.

<pre>byte 4: 1 if this is sent as a response to a status request packet
byte 5: lo byte of length of the status message
byte 6: hi byte of length of the status message
byte 7 until 7+[byte5/6]: status message

total len: 7+[byte5/6]
</pre>

<p>
	The status message usually contains the connected IRC channel/network (if
	sent from IRC to game), the amount of users, and the names of some of the
	users.

<h4 id=p10>Packet 10: Chat</h4>

<p>
	A simple packet saying someone sent a message.
	This packet can be sent from either side.

<pre>byte 4: lo byte of player id as WORD (0 when IRC to game)
byte 5: hi byte of player id as WORD (0 when IRC to game)
byte 6: length of the nickname (must be less than 50)
byte 7: lo byte of length of the message as WORD (must be less than 512)
byte 8: hi byte of length of the message as WORD (must be less than 512)
byte 9 until 9+[byte6]: nickname
byte 9+[byte6] until 9+[byte6]+[byte7/8]: message

total len: 9+[byte6]+[byte7/8]
</pre>

<h4 id=p11>Packet 11: Action</h4>

<p>
	Basically a copy of the <a href=#p10>Chat</a> packet, but for an
	action (<code>/me</code> command).

<h4 id=p12>Packet 12: Generic message</h4>

<p>
	Packet with a type and a message. See types below.

<pre>byte 4: message type
byte 5: lo byte of length of the message (max 450)
byte 6: hi byte of length of the message (max 450)
byte 7 until 7+[byte5/6]: message

total len: 7+[byte5/6]
</pre>

<p>
	Types:

<pre>0: flight message, sent from game to irc when a player finishes a flight
1: trac, sent from web to game and irc when a trac ticket/comment was made
2: login message, sent from game to irc
3: IRC mode change message, sent from irc to game
4: IRC topic change message, sent from irc to game
5: IRC nick change message, sent from irc to game
6: protip, sent from game to irc
7: metar weather report, sent from game to irc
8: radio message, sent from game to irc
</pre>

<h4 id=p30>Packet 30: Player connection</h4>

<p>
	Packet notifying a player has either connected or disconnected.

<pre>byte 4: lo byte of player id as WORD (0 when IRC to game)
byte 5: hi byte of player id as WORD (0 when IRC to game)
byte 6: reason (see below)
byte 7: length of the nickname (must be less than 25)
byte 8 until 8+[byte6]: nickname

total len: 8+[byte6]
</pre>

<p>
	Reasons:

<pre>Game:
0: timeout
1: quit
2: kick
3: connected

IRC:
6: quit
7: part
8: kick
9: join
</pre>

<p>
	Note: connection messages coming from the game are just that: connection
	messages. The player may not be the person registered to the account under the
	name they currently have. A login message will be sent from game to irc when the
	player actually logs in (see <a href=#p12>generic message</a> type 2).
