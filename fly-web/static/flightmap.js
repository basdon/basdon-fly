function e(a)
{
	return document.getElementById(a);
}
function flightmap(staticpath, id)
{
	var formatevent = function(time, description)
	{
		var date = new Date((time + UTCoffset) * 1000);
		var h = ('0' + date.getUTCHours()).slice(-2);
		var m = ('0' + date.getUTCMinutes()).slice(-2);
		var s = ('0' + date.getUTCSeconds()).slice(-2);
		return '<li><strong>' + h + ':' + m + ':' + s + '</strong> ' + description + '</li>';
	};
	var loadmap = function(ab, errcb)
	{
		var dv = new DataView(req.response);
		var version = dv.getInt32(0, 1);
		if ((version & 0xFFFFFF00) != 0x594C4600 || (version = (version & 0xFF)) < 3 || 5 < version) {
			return errcb('incorrect version');
		}
		if (dv.getInt32(4, 1) != id) {
			return errcb('incorrect version or wrong flight id');
		}
		var chunksize = 24;
		if (version > 3) {
			chunksize = 28;
		}
		var len = req.response.byteLength;
		if (len < 40 + chunksize * 2) {
			return errcb('no data');
		}
		var fuelcap = dv.getFloat32(8, 1);
		var num_frames = (len - 40);
		if (num_frames % chunksize) {
			return errcb('extra bytes: ' + (num_frames % chunksize));
		}
		num_frames /= chunksize;
		var events = '';
		var lastengine = 2, lastpaused = 0, lastfuel, lasthp;
		var mintim = 0, maxtim;
		var minspd = -9e9, maxspd = 9e9, minalt = 9e9, maxalt = -9e9, minsat = -9e9, maxsat = 9e9;
		var minhp = -9e9, maxhp = 9e9, maxfuel = -9e9;
		var minx = 9e9, miny = 9e9, maxx = -9e9, maxy = -9e9;
		var tim = [], spd = [], alt = [], sat = [], hp = [], pos = [], fuel = [], flags = [], pitch10 = [], roll10 = [];
		var pausetime, totalpausetime = 0;
		for (var i = 0; i < num_frames; i++) {
			var value, dv = new DataView(req.response, 40 + i * chunksize);
			tim.push((time = dv.getInt32(0, 1)) - totalpausetime);
			maxtim = time;
			if (!mintim) {
				mintim = time;
			}
			flags.push(value = dv.getInt8(4));
			var paused = value & 1, engine = value & 2;
			if (paused != lastpaused) {
				events += formatevent(time, 'player ' + (paused ? 'paused' : 'unpaused'));
				if (paused) {
					if (!pausetime) {
						pausetime = time;
					}
				} else if (pausetime) {
					var d =  time - pausetime - 1;
					totalpausetime += d;
					tim[tim.length - 1] -= d;
					pausetime = 0;
				}
				lastpaused = paused;
			}
			if (engine != lastengine) {
				events += formatevent(time, 'engine ' + (engine ? 'started' : 'stopped'));
				lastengine = engine;
			}
			sat.push(value = dv.getInt8(5));
			if (value < minsat) minsat = value;
			if (value > maxsat) maxsat = value;
			spd.push(value = dv.getInt16(6, 1));
			if (value < minspd) minspd = value;
			if (value > maxspd) maxspd = value;
			/*Flight map version 4 has bogus data for altitude (it's same as pitch10).*/
			alt.push(value = (version == 4 ? 0 : dv.getInt16(8, 1)));
			if (value < minalt) minalt = value;
			if (value > maxalt) maxalt = value;
			hp.push(value = dv.getInt16(10, 1));
			if (value < minhp) minhp = value;
			if (value > maxhp) maxhp = value;
			if (lasthp && value > lasthp) {
				events += formatevent('repaired ' + (value - lasthp) + ' damage');
			}
			lasthp = value;
			fuel.push(value = dv.getFloat32(12, 1));
			if (value > maxfuel) maxfuel = value;
			if (lastfuel && value > lastfuel) {
				events += formatevent('refueled ' + Math.round(value - lastfuel) + 'L');
			}
			lastfuel = value;
			var x = dv.getFloat32(16, 1);
			if (x > maxx) maxx = x;
			if (x < minx) minx = x;
			var y = -dv.getFloat32(20, 1);
			if (y > maxy) maxy = y;
			if (y < miny) miny = y;
			pos.push({x: x, y: y});
			if (version >= 4) {
				pitch10.push(dv.getInt16(24, 1));
				roll10.push(dv.getInt16(26, 1));
			}
		}
		e('fm_flightevents').outerHTML = events;
		e('fm_mapcanvas').style.display = 'block';
		/*TODO: uncomment this if we want to zoom out to show all islands completely.*/
		/*
		for (var i = 1 + fm_islands[0] * 6; i < fm_islands.length; i++) {
			var count = fm_islands[i];
			var mx = fm_islands[i + 1], my = fm_islands[i + 2];
			if (mx < minx) minx = mx;
			if (my < miny) miny = my;
			i += 2;
			while (count--) {
				var w = fm_islands[i + 3], h = fm_islands[i + 4];
				i += 5;
				if (mx + w > maxx) maxx = mx + w;
				if (my + h > maxy) maxy = my + h;
			}
		}
		*/
		var map_vsize = maxx - minx;
		var map_vsize_y = maxy - miny;
		if (map_vsize > map_vsize_y) {
			var dif = (map_vsize - map_vsize_y) * 4 / 6;
			miny -= dif / 2;
			maxy += dif / 2;
			map_vsize_y = map_vsize;
		} else {
			var dif = (map_vsize_y - map_vsize) * 6 / 4;
			minx -= dif / 2;
			maxx += dif / 2;
			map_vsize = map_vsize_y;
		}
		var map_pad = 20, map_pad_y = map_pad, sidebar_width = 300;
		var map_pad_x = sidebar_width + map_pad;
		var map_size = 900 - map_pad_x - map_pad;
		var map_scale = map_size / map_vsize;
		for (var i = 0; i < num_frames; i++) {
			pos[i].x = map_pad_x + (pos[i].x - minx) * map_scale;
			pos[i].y = map_pad_y + (pos[i].y - miny) * map_scale;
		}
		var basecanvas = document.createElement("canvas"), canvas = e('fm_canvas');
		basecanvas.width = 900; basecanvas.height = 600;
		var mc = basecanvas.getContext('2d');
		mc.fillStyle = "#6486A4";
		mc.fillRect(sidebar_width, 0, 900, 600);
		/*Draw islands.*/
		mc.fillStyle = '#ccc';
		for (var i = 1 + fm_islands[0] * 6; i < fm_islands.length;) {
			var count = fm_islands[i];
			var mx = fm_islands[i + 1], my = fm_islands[i + 2];
			i += 3;
			while (count--) {
				var fx = fm_islands[i], fy = fm_islands[i + 1];
				var tx = fm_islands[i + 2], ty = fm_islands[i + 3];
				var w = (tx - fx) * map_scale, h = (ty - fy) * map_scale;
				mc.fillRect(map_pad_x + (mx + fx - minx) * map_scale, map_pad_y + (my + fy - miny) * map_scale, w, h);
				i += 4;
			}
		}
		/*Draw runways.*/
		var count = fm_islands[0];
		i = 1;
		while (count--) {
			var x = fm_islands[i + 1], y = -fm_islands[i + 2], w = fm_islands[i + 4] * map_scale;
			x = map_pad_x + (x - minx) * map_scale;
			y = map_pad_y + (y - miny) * map_scale;
			mc.save();
			mc.fillStyle = (fm_islands[i] & 1) ? '#0C88C0' : '#9C6B9F';
			mc.translate(x, y);
			mc.rotate(fm_islands[i + 3] / 180 * Math.PI);
			mc.fillRect(-w / 2, 0, w, -fm_islands[i + 5] * map_scale);
			mc.restore();
			i += 6;
		}
		mc.fillStyle = '#333';
		mc.fillRect(0, 0, sidebar_width, 900);
		/*Scale.*/
		for (var len of [5000, 2500, 2000, 1500, 500, 200, 100]) {
			var width = Math.floor(len * map_scale);
			if (width < map_size / 3) {
				mc.font = '10px serif';
				var size = mc.measureText('' + len);
				mc.fillStyle = '#fff';
				size.height = Math.ceil(size.actualBoundingBoxAscent + size.actualBoundingBoxDescent);
				size.width = Math.ceil(size.width);
				mc.save();
				mc.translate(900 - width - size.width - 15, 600 - 11 - size.height);
				mc.fillRect(0, 0, width + size.width + 11, size.height + 6);
				/*Not using stroke because for an unknown reason it fucking anti-aliases?*/
				mc.fillStyle = '#000';
				mc.fillRect(0, 0, width + size.width + 11, 1);
				mc.fillRect(0, size.height + 6, width + size.width + 11, 1);
				mc.fillRect(-1, 1, 1, size.height + 5);
				mc.fillRect(width + size.width + 11, 1, 1, size.height + 5);
				mc.fillText('' + len, 4 + width + 4, 3 + size.height);
				mc.fillRect(4, 5, 1, 6);
				mc.fillRect(4, 10, width, 1);
				mc.fillRect(3 + width, 5, 1, 6);
				mc.restore();
				break;
			}
		}
		mc.fillStyle = "#fff";
		mc.font = '16px serif';
		var t = "basdon.net SA-MP aviation server";
		mc.fillText(t, 5 + (sidebar_width - 10 - mc.measureText(t).width) / 2, 600 - 5);
		var totaltime = maxtim - mintim - totalpausetime;
		var frame_t = [];
		for (var i = 0; i < num_frames; i++) {
			frame_t[i] = (tim[i] - mintim) / totaltime;
		}
		mc.lineCap = mc.lineJoin = 'round';
		mc.lineWidth = 4;
		var altitude_range = (maxalt - minalt);
		for (var i = 1; i < tim.length; i++) {
			var color_v = altitude_range == 0 ? 0 : (alt[i] - minalt) / altitude_range;
			var degrees = (1 - color_v) * 240 / 360;
			var r = hue(degrees + 1/3), g = hue(degrees), b = hue(degrees - 1/3);
			mc.strokeStyle = "rgb(" + r + "," + g + "," + b + ")";
			mc.beginPath();
			mc.moveTo(pos[i - 1].x, pos[i - 1].y);
			mc.lineTo(pos[i].x, pos[i].y);
			mc.stroke();
		}
		var drawgraph = function(title, strokecol, data, min, max, y, show_pct, extra_lines)
		{
			var top = y, left = 0, width = sidebar_width - 1, height = 95;
			mc.save();
			mc.translate(0, y);
			mc.fillStyle = '#000';
			mc.fillRect(0, 0, width + 1, height + 1);
			mc.fillStyle = '#fff';
			mc.fillRect(0, 0, width, height);
			mc.font = '12px serif';
			mc.fillStyle = '#000';
			mc.translate(31, 0); left += 31; width -= 54;
			mc.fillText(title, (width - mc.measureText(title).width) / 2, 2);
			mc.translate(0, 15); top += 15; height -= 25;
			mc.fillRect(-1, -1, width + 2, height + 2);
			mc.fillStyle = '#fff';
			mc.fillRect(0, 0, width, height);
			mc.fillStyle = '#000';
			mc.font = '10px serif';
			mc.textBaseline = 'middle';
			var t = show_pct ? '100%' : ('' + max);
			mc.fillText(t, -3 - Math.ceil(mc.measureText(t).width), 0);
			var t = show_pct ? '0%' : ('' + min);
			mc.fillText(t, -3 - Math.ceil(mc.measureText(t).width), height);
			/*The actual data.*/
			var scale = (max - min) / height;
			mc.save();
			mc.beginPath();
			mc.rect(0, 0, width, height);
			mc.clip();
			mc.strokeStyle = '#' + strokecol;
			for (var i = 1; i < tim.length; i++) {
				mc.beginPath();
				mc.moveTo(width * frame_t[i - 1], (1 - (data[i - 1] - min) / (max - min)) * height);
				mc.lineTo(width * frame_t[i], (1 - (data[i] - min) / (max - min)) * height);
				mc.stroke();
			}
			mc.restore();
			/*Extra dottes axes.*/
			for (var j = 0; j < extra_lines.length; j++) {
				if (max > extra_lines[j] && min < extra_lines[j]) {
					var y = Math.floor((1 - (extra_lines[j] - min) / (max - min)) * height) + 1;
					mc.fillText('' + extra_lines[j], width + 3, y);
					for (var i = 0; i < width; i += 10) {
						mc.fillRect(i, y, 5, 1);
					}
				}
			}
			mc.restore();
			return {
				top: top, left: left, width: width, height: height, right: left + width, bot: top + height,
				data: data, min: min, max: max, show_pct: show_pct
			};
		};
		mc.textBaseline = 'top';
		mc.lineWidth = 3;
		var graphy = 0, yinc = 96;
		var graph_regions = [
			drawgraph('Altitude', '99f', alt, Math.min(0, minalt), Math.max(800, maxalt), graphy, 0, [0,minalt,maxalt]),
			drawgraph('Speed', '8d8', spd, 0, 145, graphy += yinc, 0, [/*Dodo max speed.*/78,maxspd]),
			drawgraph('Aircraft health', 'f99', hp, 0, 1000, graphy += yinc, 0, [250]),
			drawgraph('Passenger satisfaction', 'f9f', sat, 0, 100, graphy += yinc, 1, [75,50,25]),
			drawgraph('Fuel', 'dd9', fuel, 0, maxfuel, graphy += yinc, 1, []),
		];
		var last_t = -2; /*-2 to force update on first call.*/
		mc = canvas.getContext('2d')
		var drawupdate = function(mm) {
			var x = mm.offsetX, y = mm.offsetY;
			var t = -1;
			for (graph of graph_regions) {
				if (graph.left < x && x < graph.right && graph.top < y && y < graph.bot) {
					t = (x - graph.left) / (graph.width);
					t = t * 1.01 - 0.005;
					if (t < 0) {
						t = 0;
					} else if (t > 1) {
						t = 1;
					}
					break;
				}
			}
			var dataindex = 0;
			if (t == -1) {
				if (x > sidebar_width) {
					var shortest_index = -1;
					var shortest_dist = 9e9;
					for (var i = 0; i < num_frames; i++) {
						var dx = pos[i].x - x, dy = pos[i].y - y, dist = dx * dx + dy * dy;
						if (dist < shortest_dist) {
							shortest_dist = dist;
							shortest_index = i;
						}
					}
					if (shortest_index != -1) {
						dataindex = shortest_index;
						t = frame_t[dataindex];
					}
				}
			} else {
				for (var i = 0; i < num_frames; i++) {
					var actual_t = 0;
					if (frame_t[i] > t) {
						actual_t = frame_t[i];
						dataindex = i;
						if (i != 0) {
							var difup = frame_t[i] - t;
							var difdown = t - frame_t[i - 1];
							if (difdown < difup) {
								dataindex--;
								actual_t = frame_t[i - 1];
							}
						}
						t = actual_t;
						break;
					}
				}
			}
			if (t == last_t) {
				return;
			}
			last_t = t;
			mc.font = '10px serif';
			mc.textBaseline = 'top';
			mc.drawImage(basecanvas, 0, 0);
			/*Graph lines, numbers.*/
			for (graph of graph_regions) {
				mc.save();
				mc.translate(graph.left, graph.top);
				mc.fillStyle = '#000';
				var x = Math.floor(graph.width * t);
				mc.fillRect(x, 0, 1, graph.height);
				var pct = (graph.data[dataindex] - graph.min) / (graph.max - graph.min);
				var y = Math.floor((1 - pct) * graph.height);
				mc.fillRect(x - 2, y - 2, 5, 5);
				var txt = '' + graph.data[dataindex];
				if (graph.show_pct) {
					txt = Math.round(pct * 100) + '%';
				}
				var size = mc.measureText(txt);
				var w = Math.ceil(size.width);
				var h = Math.ceil(size.actualBoundingBoxAscent + size.actualBoundingBoxDescent);
				mc.translate(x, y);
				if (t < .5) {
					mc.translate(5, 0);
				} else {
					mc.translate(-5 - w - 4, 0);
				}
				if (pct < .5) {
					mc.translate(0, -5 - h - 4);
				} else {
					mc.translate(0, 5);
				}
				mc.fillStyle = '#000';
				mc.fillRect(-1, 0, w + 6, h + 4);
				mc.fillRect(0, -1, w + 4, h + 6);
				mc.fillStyle = '#fff';
				mc.fillRect(0, 0, w + 4, h + 4);
				mc.fillStyle = '#000';
				mc.fillText(txt, 2, 2);
				mc.restore();
			}
			/*Plane on the path.*/
			if (t != -1) {
				mc.fillStyle = '#000';
				var dx, dy;
				if (dataindex == 0) {
					dx = pos[dataindex + 1].x - pos[dataindex].x;
					dy = pos[dataindex + 1].y - pos[dataindex].y;
				} else {
					dx = pos[dataindex].x - pos[dataindex - 1].x;
					dy = pos[dataindex].y - pos[dataindex - 1].y;
				}
				mc.save();
				mc.translate(pos[dataindex].x, pos[dataindex].y);
				mc.rotate(Math.atan2(dy, dx) + Math.PI / 2);
				mc.fillRect(-1, -8, 2, 16);
				mc.fillRect(-2, -7, 4, 15);
				mc.fillRect(-6, -4, 12, 4);
				mc.fillRect(-7, -3, 14, 2);
				mc.fillRect(-4, 4, 8, 2);
				mc.fillRect(-3, 3, 6, 1);
				mc.restore();
			}
			/*Attitude indicator.*/
			var roll = 0, pitch = 0;
			if (roll10.length && t != -1) {
				roll = -roll10[dataindex] / 10;
				pitch = pitch10[dataindex] / 10;
			}
			mc.save();
			mc.translate((sidebar_width - 100) / 2, 480);
			mc.translate(50, 50);
			mc.save();
			mc.fillStyle = '#444';
			mc.fillRect(-50, -50, 100, 100);
			mc.beginPath();
			mc.arc(0, 0, 45, 0, Math.PI * 2);
			mc.clip();
			mc.rotate(roll / 180 * Math.PI);
			mc.fillStyle = '#4BA4D9';
			mc.fillRect(-50, -50, 100, 48);
			mc.fillStyle = '#fff';
			mc.fillRect(-50, -2, 100, 4);
			mc.fillStyle = '#786240';
			mc.fillRect(-50, 2, 100, 48);
			/*Pointer.*/
			mc.fillStyle = '#fff';
			mc.beginPath();
			mc.moveTo(-4, -44);
			mc.lineTo(0, -34);
			mc.lineTo(4, -44);
			mc.closePath();
			mc.fill();
			/*Angles.*/
			mc.lineWidth = 1;
			mc.strokeStyle = '#fff';
			for (var i = -1; i < 3; i += 2) {
				mc.save();
				/*10*/
				mc.rotate(i * 10 / 180 * Math.PI);
				mc.beginPath();
				mc.moveTo(0, -40);
				mc.lineTo(0, -33);
				mc.stroke();
				/*20*/
				mc.rotate(i * 10 / 180 * Math.PI);
				mc.beginPath();
				mc.moveTo(0, -40);
				mc.lineTo(0, -33);
				mc.stroke();
				/*30*/
				mc.rotate(i * 10 / 180 * Math.PI);
				mc.lineWidth = 2;
				mc.beginPath();
				mc.moveTo(0, -44);
				mc.lineTo(0, -33);
				mc.stroke();
				/*45*/
				mc.rotate(i * 15 / 180 * Math.PI);
				mc.lineWidth = 2;
				mc.beginPath();
				mc.moveTo(-2, -40);
				mc.lineTo(0, -33);
				mc.lineTo(2, -40);
				mc.closePath();
				mc.fill();
				/*60*/
				mc.rotate(i * 15 / 180 * Math.PI);
				mc.lineWidth = 2;
				mc.beginPath();
				mc.moveTo(0, -44);
				mc.lineTo(0, -33);
				mc.stroke();
				mc.restore();
			}
			mc.restore();
			mc.save();
			mc.beginPath();
			mc.arc(0, 0, 34, 0, Math.PI * 2);
			mc.clip();
			mc.rotate(roll / 180 * Math.PI);
			mc.translate(0, pitch);
			mc.fillStyle = '#0085CB';
			mc.fillRect(-50, -150, 100, 149);
			mc.fillStyle = '#fff';
			mc.fillRect(-50, -1, 100, 2);
			mc.fillStyle = '#6B5530';
			mc.fillRect(-50, 1, 100, 149);
			mc.fillStyle = '#fff';
			mc.font = '8px serif';
			mc.textBaseline = 'middle';
			for (var i = -4; i < 5; i++) {
				if (i == 0) {
					continue;
				}
				var k = Math.abs(i);
				var w = 12;
				if (k % 2) {
					w = 6;
				} else {
					mc.fillText('' + (k * 5), w, i * 5);
					mc.fillText('' + (k * 5), -w - 9, i * 5);
				}
				mc.fillRect(-w, i * 5, w * 2, 1);
			}
			mc.restore();
			/*Orange thing.*/
			mc.fillStyle = mc.strokeStyle = '#FF9E28';
			mc.fillRect(-22, -1, 17, 2);
			mc.fillRect(-6, -1, 2, 6);
			mc.fillRect(22, -1, -17, 2);
			mc.fillRect(6, -1, -2, 6);
			mc.fillRect(-1, -1, 2, 2);
			/*Orange pointer.*/
			mc.lineWidth = 1;
			mc.beginPath();
			mc.moveTo(0, -33);
			mc.lineTo(-3, -25);
			mc.lineTo(3, -25);
			mc.closePath();
			mc.stroke();
			mc.restore();
		};
		drawupdate({offsetX: -100, offsetY: -100});
		canvas.addEventListener('mousemove', drawupdate);
	};
	var msgdiv = e('fm_mapmsg');
	var req = new XMLHttpRequest();
	req.responseType = "arraybuffer";
	req.open('GET', staticpath + '/fdr/' + id + '.flight', 1);
	req.onload = function()
	{
		if (this.status >= 200 && this.status < 300) {
			msgdiv.innerHTML = '';
			loadmap(req.response, function(what) {
				msgdiv.innerHTML = '<p>Route info for this flight is corrupted ('+what+'), try again later.</p>';
			});
		} else {
			msgdiv.innerHTML = '<p>There is no route info for this flight.</p>';
		}
	};
	req.onerror = function()
	{
		msgdiv.innerHTML = '<p style="color:#c22">Could not retrieve mission route info.</p>';
	};
	req.send();
	function hue(t)
	{
		if (t < 0) t += 1;
		if (t > 1) t -= 1;
		if (t < 1/6) return Math.round(255 * 6 * t);
		if (t < 1/2) return 255;
		if (t < 2/3) return Math.round(255 * (4 - 6 * t));
		return 0;
	};
};
