<p>
	Everything on this page is about SA-MP 0.3.7-R4

<ul>
	<li><a href=#client_console_commands>Client console commands</a>
	<li><a href=#vehicle_categories_(client)>Vehicle categories (client)</a>
	<li><a href=#vehicle_damage_status>Vehicle damage status</a>
</ul>

<h3 id=client_console_commands>Client console commands</h3>

<p>
	See <code>samp.dll+690D0</code>.

<ul>
	<li><code>/audiomsg</code> - toggles whether messages are printed when audio streams are played
	<li><code>/cmpstat</code> - does absolutely nothing
	<li><code>/ctd</code> - toggles camera target debug (what is the effect?)
	<li><code>/dl</code> - toggles debug labels on vehicles
	<li><code>/fontsize &lt;size&gt;</code> - changes chat window's font size
	<li><code>/fpslimit &lt;limit&gt;</code> - set fps limit of the game. only has effect if "Frame limiter" in game settings is enabled.
	<li><code>/headmove</code> - toggles whether other players' character head will move to the direction they're looking at
	<li><code>/hudscalefix</code> - toggles a fix that ensures the minimap radar is a circle on widescreen resolutions
	<li><code>/interior</code> - prints the current game interior
	<li><code>/logurls</code> - toggles whether messages are printend when files are downloaded. file downloading is leftover code that is not usable.
	<li><code>/mem</code> - prints the game's streaming memory limit
	<li><code>/nametagstatus</code> - toggles whether the "hourglass" icon is shown next to players' nametags when they are paused
	<li><code>/pagesize &lt;size&gt;</code> - sets amount of lines that the chat window shows on screen
	<li><code>/q</code> - alias of <code>/quit</code>
	<li><code>/quit</code> - quit the game
	<li><code>/rcon &lt;command&gt;</code> - sends rcon commands to the server
	<li>
		<code>/rs [comment]</code> - saves current position, depending on your state:
		<ul>
			<li>when on foot, saves <code>x,y,z,facingAngle ; comment</code> in <code>Documents\GTA San Andreas User Files\SAMP\rawpositions.txt</code>
			<li>when in car, saves <code>model,x,y,z,heading,col1,col2 ; comment</code> in <code>rawvehicles.txt</code> <strong>in your game folder</strong> (this seems like an oversight and was probably supposed to be in User Files too)
		</ul>
	<li>
		<code>/save [comment]</code> - saves current position in <code>Documents\GTA San Andreas User Files\SAMP\savedpositions.txt</code> formatted as a
		<a href=https://basdon.github.io/documented-samp-pawn-api/main.xml#AddPlayerClass><code>AddPlayerClass</code></a><img src=/s/moin-www.png alt="globe icon" title="external link"> or
		<a href=https://basdon.github.io/documented-samp-pawn-api/main.xml#AddStaticVehicle><code>AddStaticVehicle</code></a><img src=/s/moin-www.png alt="globe icon" title="external link"> function call, depending on your state.
	<li><code>/testdw</code> - send test values to the death message list (death message list may be hidden with <kbd>F9</kbd>)
	<li><code>/timestamp</code> - toggles whether to show timestamp before chat messages
	<li><code>/togobjlight</code> - seems unused
</ul>

When launching SA-MP Debug (<code>samp_debug.exe</code>), following commands are also available:

<ul>
	<li><code>/player_skin &lt;skinid&gt;</code>
	<li><code>/set_weather &lt;weatherid&gt;</code>
	<li><code>/set_time &lt;hour&gt; &lt;minute&gt;</code>
	<li><code>/v &lt;model&gt;</code> - alias of <code>/vehicle</code>
	<li><code>/vehicle &lt;modelid&gt;</code> - spawns a vehicle next to you. <code>model</code> value is unchecked, so this will crash the client if it's invalid
	<li><code>/vsel</code> - opens vehicle selection UI. Use the <code>JUMP</code>/<code>SPRINT</code> keys to select the model and <code>ENTER VEHICLE</code> key to spawn
</ul>
<h3 id=vehicle_categories_(client)>Vehicle Categories (client)</h3>

<p>
	Some vehicle things are handled differently by the client depending on the category of the vehicle.
	The category of a vehicle is determined at runtime by its vtable. SAMP defines following categories:
<ul>
	<li>0: other, which effectively is: trailer/monstertruck(+dune/dumper)/quadbike<strong><sup>(1)</sup></strong>
	<li>1: automobile (vtable <code>0x871120</code>)
	<li>2: motorcycle (vtable <code>0x871360</code>)
	<li>3: heli (vtable <code>0x871680</code>)
	<li>4: boat (vtable <code>0x8721A0</code>)
	<li>5: plane (vtable <code>0x871948</code>)
	<li>6: bike (vtable <code>0x871528</code>)
	<li>7: train (vtable <code>0x872370</code>)
</ul>
<details>
	<summary>Full vehicle list</summary>
	<table class="new">
		<thead>
			<tr><th>model<th>actual vtable<th>assigned category<th>vehicle name
		</thead>
		<tbody>
			<tr><td>400<td>0x871120<td>1<td>Landstalker
			<tr><td>401<td>0x871120<td>1<td>Bravura
			<tr><td>402<td>0x871120<td>1<td>Buffalo
			<tr><td>403<td>0x871120<td>1<td>Linerunner
			<tr><td>404<td>0x871120<td>1<td>Perennial
			<tr><td>405<td>0x871120<td>1<td>Sentinel
			<tr><td>406<td>0x8717D8<td>0<strong><sup>(1)</sup><td>Dumper
			<tr><td>407<td>0x871120<td>1<td>Fire Truck
			<tr><td>408<td>0x871120<td>1<td>Trashmaster
			<tr><td>409<td>0x871120<td>1<td>Stretch
			<tr><td>410<td>0x871120<td>1<td>Manana
			<tr><td>411<td>0x871120<td>1<td>Infernus
			<tr><td>412<td>0x871120<td>1<td>Voodoo
			<tr><td>413<td>0x871120<td>1<td>Pony
			<tr><td>414<td>0x871120<td>1<td>Mule
			<tr><td>415<td>0x871120<td>1<td>Cheetah
			<tr><td>416<td>0x871120<td>1<td>Ambulance
			<tr><td>417<td>0x871680<td>3<td>Leviathan
			<tr><td>418<td>0x871120<td>1<td>Moonbeam
			<tr><td>419<td>0x871120<td>1<td>Esperanto
			<tr><td>420<td>0x871120<td>1<td>Taxi
			<tr><td>421<td>0x871120<td>1<td>Washington
			<tr><td>422<td>0x871120<td>1<td>Bobcat
			<tr><td>423<td>0x871120<td>1<td>Mr Whoopee
			<tr><td>424<td>0x871120<td>1<td>BF Injection
			<tr><td>425<td>0x871680<td>3<td>Hunter
			<tr><td>426<td>0x871120<td>1<td>Premier
			<tr><td>427<td>0x871120<td>1<td>Enforcer
			<tr><td>428<td>0x871120<td>1<td>Securicar
			<tr><td>429<td>0x871120<td>1<td>Banshee
			<tr><td>430<td>0x8721A0<td>4<td>Predator
			<tr><td>431<td>0x871120<td>1<td>Bus
			<tr><td>432<td>0x871120<td>1<td>Rhino
			<tr><td>433<td>0x871120<td>1<td>Barracks
			<tr><td>434<td>0x871120<td>1<td>Hotknife
			<tr><td>435<td>0x871C28<td>0<td>Artic Trailer 1
			<tr><td>436<td>0x871120<td>1<td>Previon
			<tr><td>437<td>0x871120<td>1<td>Coach
			<tr><td>438<td>0x871120<td>1<td>Cabbie
			<tr><td>439<td>0x871120<td>1<td>Stallion
			<tr><td>440<td>0x871120<td>1<td>Rumpo
			<tr><td>441<td>0x871120<td>1<td>RC Bandit
			<tr><td>442<td>0x871120<td>1<td>Romero
			<tr><td>443<td>0x871120<td>1<td>Packer
			<tr><td>444<td>0x8717D8<td>0<strong><sup>(1)</sup><td>Monster
			<tr><td>445<td>0x871120<td>1<td>Admiral
			<tr><td>446<td>0x8721A0<td>4<td>Squalo
			<tr><td>447<td>0x871680<td>3<td>Seasparrow
			<tr><td>448<td>0x871360<td>2<td>Pizzaboy
			<tr><td>449<td>0x872370<td>7<td>Tram
			<tr><td>450<td>0x871C28<td>0<td>Artic Trailer 2
			<tr><td>451<td>0x871120<td>1<td>Turismo
			<tr><td>452<td>0x8721A0<td>4<td>Speeder
			<tr><td>453<td>0x8721A0<td>4<td>Reefer
			<tr><td>454<td>0x8721A0<td>4<td>Tropic
			<tr><td>455<td>0x871120<td>1<td>Flatbed
			<tr><td>456<td>0x871120<td>1<td>Yankee
			<tr><td>457<td>0x871120<td>1<td>Caddy
			<tr><td>458<td>0x871120<td>1<td>Solair
			<tr><td>459<td>0x871120<td>1<td>Berkley's RC Van
			<tr><td>460<td>0x871948<td>5<td>Skimmer
			<tr><td>461<td>0x871360<td>2<td>PCJ-600
			<tr><td>462<td>0x871360<td>2<td>Faggio
			<tr><td>463<td>0x871360<td>2<td>Freeway
			<tr><td>464<td>0x871948<td>5<td>RC Baron
			<tr><td>465<td>0x871680<td>3<td>RC Raider
			<tr><td>466<td>0x871120<td>1<td>Glendale
			<tr><td>467<td>0x871120<td>1<td>Oceanic
			<tr><td>468<td>0x871360<td>2<td>Sanchez
			<tr><td>469<td>0x871680<td>3<td>Sparrow
			<tr><td>470<td>0x871120<td>1<td>Patriot
			<tr><td>471<td>0x871AE8<td>0<strong><sup>(1)</sup><td>Quadbike
			<tr><td>472<td>0x8721A0<td>4<td>Coast Guard
			<tr><td>473<td>0x8721A0<td>4<td>Dinghy
			<tr><td>474<td>0x871120<td>1<td>Hermes
			<tr><td>475<td>0x871120<td>1<td>Sabre
			<tr><td>476<td>0x871948<td>5<td>Rustler
			<tr><td>477<td>0x871120<td>1<td>ZR-350
			<tr><td>478<td>0x871120<td>1<td>Walton
			<tr><td>479<td>0x871120<td>1<td>Regina
			<tr><td>480<td>0x871120<td>1<td>Comet
			<tr><td>481<td>0x871528<td>6<td>BMX
			<tr><td>482<td>0x871120<td>1<td>Burrito
			<tr><td>483<td>0x871120<td>1<td>Camper
			<tr><td>484<td>0x8721A0<td>4<td>Marquis
			<tr><td>485<td>0x871120<td>1<td>Baggage
			<tr><td>486<td>0x871120<td>1<td>Dozer
			<tr><td>487<td>0x871680<td>3<td>Maverick
			<tr><td>488<td>0x871680<td>3<td>News Chopper
			<tr><td>489<td>0x871120<td>1<td>Rancher
			<tr><td>490<td>0x871120<td>1<td>FBI Rancher
			<tr><td>491<td>0x871120<td>1<td>Virgo
			<tr><td>492<td>0x871120<td>1<td>Greenwood
			<tr><td>493<td>0x8721A0<td>4<td>Jetmax
			<tr><td>494<td>0x871120<td>1<td>Hotring Racer
			<tr><td>495<td>0x871120<td>1<td>Sandking
			<tr><td>496<td>0x871120<td>1<td>Blista Compact
			<tr><td>497<td>0x871680<td>3<td>Police Maverick
			<tr><td>498<td>0x871120<td>1<td>Boxville
			<tr><td>499<td>0x871120<td>1<td>Benson
			<tr><td>500<td>0x871120<td>1<td>Mesa
			<tr><td>501<td>0x871680<td>3<td>RC Goblin
			<tr><td>502<td>0x871120<td>1<td>Hotring Racer A
			<tr><td>503<td>0x871120<td>1<td>Hotring Racer B
			<tr><td>504<td>0x871120<td>1<td>Bloodring Banger
			<tr><td>505<td>0x871120<td>1<td>Lure Rancher
			<tr><td>506<td>0x871120<td>1<td>Super GT
			<tr><td>507<td>0x871120<td>1<td>Elegant
			<tr><td>508<td>0x871120<td>1<td>Journey
			<tr><td>509<td>0x871528<td>6<td>Bike
			<tr><td>510<td>0x871528<td>6<td>Mountain Bike
			<tr><td>511<td>0x871948<td>5<td>Beagle
			<tr><td>512<td>0x871948<td>5<td>Cropduster
			<tr><td>513<td>0x871948<td>5<td>Stuntplane
			<tr><td>514<td>0x871120<td>1<td>Tanker
			<tr><td>515<td>0x871120<td>1<td>Roadtrain
			<tr><td>516<td>0x871120<td>1<td>Nebula
			<tr><td>517<td>0x871120<td>1<td>Majestic
			<tr><td>518<td>0x871120<td>1<td>Buccaneer
			<tr><td>519<td>0x871948<td>5<td>Shamal
			<tr><td>520<td>0x871948<td>5<td>Hydra
			<tr><td>521<td>0x871360<td>2<td>FCR-900
			<tr><td>522<td>0x871360<td>2<td>NRG-500
			<tr><td>523<td>0x871360<td>2<td>HPV1000
			<tr><td>524<td>0x871120<td>1<td>Cement Truck
			<tr><td>525<td>0x871120<td>1<td>Towtruck
			<tr><td>526<td>0x871120<td>1<td>Fortune
			<tr><td>527<td>0x871120<td>1<td>Cadrona
			<tr><td>528<td>0x871120<td>1<td>FBI Truck
			<tr><td>529<td>0x871120<td>1<td>Willard
			<tr><td>530<td>0x871120<td>1<td>Forklift
			<tr><td>531<td>0x871120<td>1<td>Tractor
			<tr><td>532<td>0x871120<td>1<td>Combine Harvester
			<tr><td>533<td>0x871120<td>1<td>Feltzer
			<tr><td>534<td>0x871120<td>1<td>Remington
			<tr><td>535<td>0x871120<td>1<td>Slamvan
			<tr><td>536<td>0x871120<td>1<td>Blade
			<tr><td>539<td>0x871948<td>5<td>Vortex
			<tr><td>540<td>0x871120<td>1<td>Vincent
			<tr><td>541<td>0x871120<td>1<td>Bullet
			<tr><td>542<td>0x871120<td>1<td>Clover
			<tr><td>543<td>0x871120<td>1<td>Sadler
			<tr><td>544<td>0x871120<td>1<td>Fire Truck Ladder
			<tr><td>545<td>0x871120<td>1<td>Hustler
			<tr><td>546<td>0x871120<td>1<td>Intruder
			<tr><td>547<td>0x871120<td>1<td>Primo
			<tr><td>548<td>0x871680<td>3<td>Cargobob
			<tr><td>549<td>0x871120<td>1<td>Tampa
			<tr><td>550<td>0x871120<td>1<td>Sunrise
			<tr><td>551<td>0x871120<td>1<td>Merit
			<tr><td>552<td>0x871120<td>1<td>Utility Van
			<tr><td>553<td>0x871948<td>5<td>Nevada
			<tr><td>554<td>0x871120<td>1<td>Yosemite
			<tr><td>555<td>0x871120<td>1<td>Windsor
			<tr><td>556<td>0x8717D8<td>0<strong><sup>(1)</sup><td>Monster A
			<tr><td>557<td>0x8717D8<td>0<strong><sup>(1)</sup><td>Monster B
			<tr><td>558<td>0x871120<td>1<td>Uranus
			<tr><td>559<td>0x871120<td>1<td>Jester
			<tr><td>560<td>0x871120<td>1<td>Sultan
			<tr><td>561<td>0x871120<td>1<td>Stratum
			<tr><td>562<td>0x871120<td>1<td>Elegy
			<tr><td>563<td>0x871680<td>3<td>Raindance
			<tr><td>564<td>0x871120<td>1<td>RC Tiger
			<tr><td>565<td>0x871120<td>1<td>Flash
			<tr><td>566<td>0x871120<td>1<td>Tahoma
			<tr><td>567<td>0x871120<td>1<td>Savanna
			<tr><td>568<td>0x871120<td>1<td>Bandito
			<tr><td>569<td>0x871120<td>7<td>Freight flat
			<tr><td>570<td>0x871120<td>7<td>Brown Streak Carriage
			<tr><td>571<td>0x871120<td>1<td>Kart
			<tr><td>572<td>0x871120<td>1<td>Mower
			<tr><td>573<td>0x8717D8<td>0<strong><sup>(1)</sup><td>Dune
			<tr><td>574<td>0x871120<td>1<td>Sweeper
			<tr><td>575<td>0x871120<td>1<td>Broadway
			<tr><td>576<td>0x871120<td>1<td>Tornado
			<tr><td>577<td>0x871948<td>5<td>AT-400
			<tr><td>578<td>0x871120<td>1<td>DFT-30
			<tr><td>579<td>0x871120<td>1<td>Huntley
			<tr><td>580<td>0x871120<td>1<td>Stafford
			<tr><td>581<td>0x871360<td>2<td>BF-400
			<tr><td>582<td>0x871120<td>1<td>Newsvan
			<tr><td>583<td>0x871120<td>1<td>Tug
			<tr><td>584<td>0x871C28<td>0<td>Tanker Trailer
			<tr><td>585<td>0x871120<td>1<td>Emperor
			<tr><td>586<td>0x871360<td>2<td>Wayfarer
			<tr><td>587<td>0x871120<td>1<td>Euros
			<tr><td>588<td>0x871120<td>1<td>Hotdog
			<tr><td>589<td>0x871120<td>1<td>Club
			<tr><td>590<td>0x872370<td>7<td>Freight box
			<tr><td>591<td>0x871C28<td>0<td>Artic Trailer 3
			<tr><td>592<td>0x871948<td>5<td>Andromada
			<tr><td>593<td>0x871948<td>5<td>Dodo
			<tr><td>594<td>0x871120<td>1<td>RC Cam
			<tr><td>595<td>0x8721A0<td>4<td>Launch
			<tr><td>596<td>0x871120<td>1<td>Police LS
			<tr><td>597<td>0x871120<td>1<td>Police SF
			<tr><td>598<td>0x871120<td>1<td>Police LV
			<tr><td>599<td>0x871120<td>1<td>Ranger
			<tr><td>600<td>0x871120<td>1<td>Picador
			<tr><td>601<td>0x871120<td>1<td>S.W.A.T.
			<tr><td>602<td>0x871120<td>1<td>Alpha
			<tr><td>603<td>0x871120<td>1<td>Phoenix
			<tr><td>604<td>0x871120<td>1<td>Damaged Glendale
			<tr><td>605<td>0x871120<td>1<td>Damaged Sadler
			<tr><td>606<td>0x871C28<td>0<td>Baggage Box A
			<tr><td>607<td>0x871C28<td>0<td>Baggage Box B
			<tr><td>608<td>0x871C28<td>0<td>Tug Stairs
			<tr><td>609<td>0x871120<td>1<td>Black Boxville
			<tr><td>610<td>0x871C28<td>0<td>Farm Trailer
			<tr><td>611<td>0x871C28<td>0<td>Utility Trailer
		</tbody>
	</table>
</details>

<h4>Notes</h4>

<p>
	<strong><sup>(1)</sup></strong> vehicles with a vtable that isn't one of the explicitely checked values
	are put in category 0. Besides all trailers, this also (maybe unexpectedly) includes
	<strong>Dumper, Monster, Monster A, Monster B, Quadbike, Dune</strong>. This is because the Dumper, Dune
	and all Monster vehicles are instances of <code>CMonsterTruck</code> and the Quadbike has its own unique
	<code>CQuadBike</code> class.

<h3 id=vehicle_damage_status>Vehicle damage status</h3>

<p>
	See
	<a href=https://basdon.github.io/documented-samp-pawn-api/main.xml#Appendix_Vehicle_Damage_Status>
		https://basdon.github.io/documented-samp-pawn-api/main.xml#Appendix_Vehicle_Damage_Status
	</a>
	for an overview of vehicle support, behavior, and some quirks.

<h4>Server receiving damage from client</h4>

<p>
	Server ignores damage status updates if it's not coming from the driver of the vehicle,
	otherwise it's applied and synced unconditionally and <code>OnVehicleDamageStatusUpdate</code>
	is invoked.

<h4>Client receiving damage from server</h4>

<p>
	Behavior depends on the vehicle model it's being applied to, see also <a href=#vehicle_categories_(client)>Vehicle categories</a>.
	Most import to take into account: <strong>Dumper, Monster, Monster A, Monster B, Quadbike, Dune</strong> are categorized as
	"unknown", meaning none of their damage gets synced. The bumper may get damaged in the driver's game (or tires of a quad bike),
	but none of the other players will see any damage to that vehicle.
<p>
	Client applies tire damage at <code>CSampVehicle::ApplyVehicleDamageTires(char)</code></summary> with
	following behavior:
<ul>
	<li>for <strong>CHeli/CPlane/CBmx/CBoat/CTrain/Unknown</strong>: nop
	<li>for <strong>CAutomobile</strong>: apply to <code>CAutomobile.m_damageManager.wheel[]</code>
	<li>for <strong>CBike</strong>: apply to <code>CBike.m_acWheelDamageState[]</code>
</ul>

<p>
	Client applies other damage at <code>CSampVehicle::ApplyVehicleDamagePanelsDoorsLights(int,int,char)</code>
	with following behavior:
<ul>
	<li>
		for <strong>CBike/CHeli/CPlane/CBmx/CBoat/CTrain/Unknown</strong>: nop
	<li>
		for <strong>CAutomobile/CPlane</strong>:
		<ul>
			<li>
				if not all values are <code>0</code>:
				<ul>
					<li>apply panels doors lights to <code>CAutomobile::m_damageManager</code>
					<li>call <code>CAutomobile::mCC()</code> (seems to apply damage status things, spawn flying components if things are detached, ...)
				</ul>
			<li>
				if all values are <code>0</code>: call <code>CAutomobile::Fix</code><strong><sup>(1)</sup><sup>(3)</sup></strong>
		</ul>
</ul>

<h4>Notes</h4>

<p>
	<strong><sup>(1)</sup></strong> while <code>CPlane</code> inherits from <code>CAutomobile</code>,
	it overrides the <code>Fix</code> method yet SAMP still calls the base <code>CAutomobile::Fix</code> for
	<code>CPlane</code> instances. This is what causes the 'ghost door' effect<strong><sup>(6)</sup></strong> on some planes after doing
	<code>RepairVehicle()</code><strong><sup>(2)</sup></strong> or updating damage status to 0 on planes.
<p>
	For comparison, <abbr title="Multi Theft Auto, another multiplayed mod">MTA</abbr> dispatches to specific overridden methods based on the vehicle's model info:
<pre>if (pModelInfo->IsCar() || pModelInfo->IsMonsterTruck() || pModelInfo->IsTrailer())
    dwFunc = FUNC_CAutomobile__Fix<strong><sup>(3)</sup></strong>;
else if (pModelInfo->IsPlane())
    dwFunc = FUNC_CPlane__Fix<strong><sup>(3)</sup></strong>;
else if (pModelInfo->IsHeli())
    dwFunc = FUNC_CHeli__Fix<strong><sup>(3)</sup></strong>;
else if (pModelInfo->IsBike())
    dwFunc = FUNC_CBike_Fix<strong><sup>(3)</sup></strong>;
</pre>
<p>
	Source: <a href=https://github.com/multitheftauto/mtasa-blue/blob/bb0b32cb90f3379853fb9eab0a651caabd837d6f/Client/game_sa/CVehicleSA.cpp#L1120>https://github.com/multitheftauto/mtasa-blue/blob/bb0b32cb90f3379853fb9eab0a651caabd837d6f/Client/game_sa/CVehicleSA.cpp#L1120</a>.
<p>
	It also defines <code>FUNC_CQuadBike__Fix</code><strong><sup>(3)</sup></strong>, yet that is not used in the above.
<p>
	<strong><sup>(2)</sup></strong> <code>RepairVehicle(vehicleid)</code> is implemented in SAMP by doing <code>SetVehicleHealth(vehicleid, 1000.0); UpdateVehicleDamageStatus(vehicleid, 0, 0, 0, 0);</code>.
<p>
	<strong><sup>(3)</sup></strong> Implementations of the <code>Fix</code> proc:
<ul>
	<li>
		<code>CAutomobile::Fix</code> does the following:
		<ul>
			<li><code>CAutomobile::m_damageManager.reset()</code>
			<li><code>if (!(CVehicle::pHandlingData->uiModelFlags & 0x100<strong><sup>(4)</sup></strong>)) m_damageManager.door[2] = door[3] = door[4] = door[5] = 4<strong><sup>(5)</sup></strong>;</code>
			<li>clump stuff
			<li><code>CVehicle::m_nVehicleFlags[1] &= 0xFE;</code> (clears <code>bIsDamaged</code>)
			<li>matrix stuff
			<li><code>m_damageManager.wheel[0] = wheel[1] = wheel[2] = wheel[3] = 0;</code> (seems unnecessary as <code>m_damageManager.reset()</code> already did this)
			<li>reset bouncing panels<sup>[citation needed]</sup>
		</ul>
	<li>
		<code>CPlane::Fix</code> does the following:
		<ul>
			<li><code>CAutomobile::m_damageManager.reset()</code>
			<li><code>if (!(CVehicle::pHandlingData->uiModelFlags & 0x100<strong><sup>(4)</sup></strong>)) m_damageManager.door[2] = door[3] = door[4] = door[5] = 4<strong><sup>(5)</sup></strong>;</code>
			<li><code>CVehicle::m_nVehicleFlags[1] &= 0xFE;</code> (clears <code>bIsDamaged</code>)
		</ul>
	<li>
		<code>CHeli::Fix</code> does the following:
		<ul>
			<li><code>CAutomobile::m_damageManager.reset()</code>
			<li><code>CVehicle::m_nVehicleFlags[1] &= 0xFE;</code> (clears <code>bIsDamaged</code>)
		</ul>
	<li>
		<code>CBike::Fix</code> does the following:
		<ul>
			<li><code>CVehicle::m_nVehicleFlags[1] &= 0xFE;</code> (clears <code>bIsDamaged</code>)
			<li><code>CBike::m_cDamageFlags &= 0xBF;</code> (unknown)
			<li><code>CBike::m_acWheelDamageState[0] = CBike::m_acWheelDamageState[1] = 0;</code>
		</ul>
	<li>
		<code>CQuadBike::Fix</code> does the following:
		<ul>
			<li><code>CAutomobile::m_damageManager.reset()</code>
			<li><code>m_damageManager.door[2] = door[3] = door[4] = door[5] = 4<strong><sup>(5)</sup></strong>;</code>
			<li>clump stuff
			<li><code>m_damageManager.wheel[0] = wheel[1] = wheel[2] = wheel[3] = 0;</code> (seems unnecessary as <code>m_damageManager.reset()</code> already did this)
			<li><code>CVehicle::m_nVehicleFlags[1] &= 0xFE;</code> (clears <code>bIsDamaged</code>)
		</ul>
</ul>
<p>
	<strong><sup>(4)</sup></strong> <code>uiModelFlags</code> mask <code>0x100</code> means <code>NO_DOORS</code>, as per <code>data/handling.cfg</code> comments (field <code>modelFlags (af)</code>).
<p>
	<strong><sup>(5)</sup></strong> a damagemanager door value of <code>4</code> means that the door is removed/detached.
<p>
	<strong><sup>(6)</sup></strong>
	<a href=https://basdon.github.io/documented-samp-pawn-api/main.xml#Appendix_Vehicle_Damage_Status_planeghostdoors>
		https://basdon.github.io/documented-samp-pawn-api/main.xml#Appendix_Vehicle_Damage_Status_planeghostdoors
	</a>
