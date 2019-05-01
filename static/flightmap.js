function e(a)
{
	return document.getElementById(a);
}
function flightmap(staticpath, id)
{
	var loadmap = function(ab, errcb)
	{
		var dv = new DataView(req.response);
		if (dv.getInt8(0) != 1 || dv.getInt32(1, 1) != id) {
			return errcb();
		}
		var len = req.response.byteLength;
		if (len < 69) {
			return errcb();
		}
		var mc = e('crmm').getContext('2d');
		mc.fillStyle = "#6486A4";
		mc.fillRect(0, 0, 1000, 1000);
		var map = new Image();
		map.onerror = function()
		{
			var dp = (len - 30);
			if (dp % 19) {
				return errcb();
			}
			dp /= 19;
			var mintim = 0, maxtim = 0, minspd, maxspd, minalt, maxalt, minsat, maxsat, minhp, maxhp;
			var tim = [], spd = [], alt = [], sat = [], hp = [], pos = [];
			dv = new DataView(req.response, 30);
			for (var i = 0; i < dp; i++) {
				var t, o = i * 19;
				tim.push(maxtim = dv.getInt32(o, 1));
				if (!mintim) {
					mintim = maxtim;
				}
				spd.push(t = dv.getInt16(4 + o, 1));
				if (!minspd || t < minspd) minspd = t;
				if (!maxspd || t > maxspd) maxspd = t;
				alt.push(t = dv.getInt16(6 + o, 1));
				if (!minalt || t < minalt) minalt = t;
				if (!maxalt || t > maxalt) maxalt = t;
				sat.push(t = dv.getInt8(8 + o, 1));
				if (!minsat || t < minsat) minsat = t;
				if (!maxsat || t > maxsat) maxsat = t;
				hp.push(t = dv.getInt16(9 + o, 1));
				if (!minhp || t < minhp) minhp = t;
				if (!maxhp || t > maxhp) maxhp = t;
				pos.push({x: dv.getFloat32(11 + o, 1) / 6.66 + 500, y: -dv.getFloat32(15 + o, 1) / 6.66 + 500});
			}
			var t = [];
			for (var i = 0; i < tim.length; i++) {
				t[i] = (tim[i] - mintim) / (maxtim - mintim);
			}
			e('rm').style.display = 'block';
			rmmsg.innerHTML = '';
			mc.lineCap = mc.lineJoin = 'round';
			mc.lineWidth = 7.5;
			for (var i = 1; i < tim.length; i++) {
				var v = (alt[i] - minalt) / (maxalt - minalt);
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
			loadmap(req.response, function() {
				rmmsg.innerHTML = '<p>Route info for this flight is corrupted, try again later.</p>';
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
