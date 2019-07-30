function e(a)
{
	return document.getElementById(a);
}
function flightmap(staticpath, id)
{
	var loadmap = function(ab, errcb)
	{
		var dv = new DataView(req.response);
		if (dv.getInt32(0, 1) != 0x594C4603 || dv.getInt32(4, 1) != id) {
			return errcb('incorrect version or wrong flight id');
		}
		var len = req.response.byteLength;
		if (len < 96) {
			return errcb('no data');
		}
		var fuelcap = dv.getFloat32(8, 1);
		var mc = e('crmm').getContext('2d');
		mc.fillStyle = "#6486A4";
		mc.fillRect(0, 0, 1000, 1000);
		var map = new Image();
		map.onerror = function()
		{
			var dp = (len - 40);
			if (dp % 24) {
				return errcb('extra bytes: ' + (dp % 24));
			}
			dp /= 24;
			var events = '';
			var engine = 2, paused = 0, lastfuel, lasthp;
			var mintim = 0, maxtim = 0, minspd, maxspd, minalt, maxalt, minsat, maxsat, minhp, maxhp, maxfuel;
			var tim = [], spd = [], alt = [], sat = [], hp = [], pos = [], fuel = [], flags = [];
			for (var i = 0; i < dp; i++) {
				var t, dv = new DataView(req.response, 40 + i * 24);
				tim.push(maxtim = dv.getInt32(0, 1));
				if (!mintim) {
					mintim = maxtim;
				}
				var dd = new Date((maxtim + UTCoffset) * 1000);
				dd = '<strong>' + ('0'+dd.getUTCHours()).slice(-2)
					+ ':' + ('0'+dd.getUTCMinutes()).slice(-2)
					+ ':' + ('0'+dd.getUTCSeconds()).slice(-2) + '</strong> ';
				flags.push(t = dv.getInt8(4));
				var p = t & 1, en = t & 2;
				if (p != paused) {
					events += '<li>'+dd+'player '+(p?'':'un')+'paused</li>';
					paused = p;
				}
				if (en != engine) {
					events += '<li>'+dd+'engine '+(en?'started':'stopped')+'</li>';
					engine = en;
				}
				sat.push(t = dv.getInt8(5));
				if (!minsat || t < minsat) minsat = t;
				if (!maxsat || t > maxsat) maxsat = t;
				spd.push(t = dv.getInt16(6, 1));
				if (!minspd || t < minspd) minspd = t;
				if (!maxspd || t > maxspd) maxspd = t;
				alt.push(t = dv.getInt16(8, 1));
				if (!minalt || t < minalt) minalt = t;
				if (!maxalt || t > maxalt) maxalt = t;
				hp.push(t = dv.getInt16(10, 1));
				if (!minhp || t < minhp) minhp = t;
				if (!maxhp || t > maxhp) maxhp = t;
				if (lasthp && t > lasthp) {
					events += '<li>'+dd+'repaired '+(t-lasthp)+' damage</li>';
				}
				lasthp = t;
				fuel.push(t = dv.getFloat32(12, 1));
				if (!maxfuel || t > maxfuel) maxfuel = t;
				if (lastfuel && t > lastfuel) {
					events += '<li>'+dd+'refueled '+Math.round(t-lastfuel)+'L</li>';
				}
				lastfuel = t;
				pos.push({x: dv.getFloat32(16, 1) / 6.66 + 500, y: -dv.getFloat32(20, 1) / 6.66 + 500});
			}
			e('rmt').outerHTML = events;
			var t = [];
			for (var i = 0; i < tim.length; i++) {
				t[i] = (tim[i] - mintim) / (maxtim - mintim);
			}
			e('rm').style.display = 'block';
			rmmsg.innerHTML = '';
			mc.lineCap = mc.lineJoin = 'round';
			mc.lineWidth = 7.5;
			for (var i = 1; i < tim.length; i++) {
				var dalt = (maxalt - minalt);
				var v = dalt == 0 ? 0 : (alt[i] - minalt) / dalt;
				var h = (1 - v) * 240 / 360;
				mc.strokeStyle = "rgb("+hue(h + 1/3)+","+hue(h)+","+hue(h - 1/3)+")";
				mc.beginPath();
				mc.moveTo(pos[i - 1].x, pos[i - 1].y);
				mc.lineTo(pos[i].x, pos[i].y);
				mc.stroke();
			}
			var g = function(fill, id, vals, max)
			{
				var mc = e(id).getContext('2d');
				mc.fillStyle = fill;
				mc.beginPath();
				mc.moveTo(0, 200);
				for (var i = 0; i < t.length; i++) {
					mc.lineTo(500 * t[i], 200 * (1 - vals[i] / max));
				}
				mc.lineTo(500, 200);
				mc.closePath();
				mc.fill();
			};
			g('#99f', 'crms', spd, 145);
			g('#9f9', 'crma', alt, maxalt);
			g('#f99', 'crmh', hp, 1000);
			g('#f9f', 'crmp', sat, 100);
			g('#ff9', 'crmf', fuel, maxfuel);
		}
		map.onload = function()
		{
			mc.drawImage(map, 0, 0);
			map.onerror();
		};
		map.src = staticpath + '/map.svg';
	}
	var rmmsg = e('rmmsg');
	var req = new XMLHttpRequest();
	req.responseType = "arraybuffer";
	req.open('GET', staticpath + '/fdr/' + id + '.flight', 1);
	req.onload = function()
	{
		if (this.status >= 200 && this.status < 300) {
			loadmap(req.response, function(what) {
				rmmsg.innerHTML = '<p>Route info for this flight is corrupted ('+what+'), try again later.</p>';
			});
		}
		else rmmsg.innerHTML = '<p>There is no route info for this flight.</p>';
	};
	req.onerror = function()
	{
		rmmsg.innerHTML = '<p style="color:red">Could not retrieve mission route info.</p>';
	};
	req.send();
	function hue(t)
	{
		if(t < 0) t += 1;
		if(t > 1) t -= 1;
		if(t < 1/6) return Math.round(255 * 6 * t);
		if(t < 1/2) return 255;
		if(t < 2/3) return Math.round(255 * (4 - 6 * t));
		return 0;
	};
}
