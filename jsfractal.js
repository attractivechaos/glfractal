function mb_draw_mbrot(canvasId)
{
	var palette = [[
		[  0,  0,  0],[  0,  0,  0],[  8,  0,  0],[ 16,  4,  4],[ 24,  4,  8],[ 32,  8, 12],[ 40, 12, 16],[ 48, 12, 20],
		[ 56, 16, 24],[ 64, 20, 28],[ 72, 20, 32],[ 80, 24, 36],[ 88, 28, 40],[ 96, 28, 44],[104, 32, 48],[112, 36, 52],
		[120, 36, 56],[128, 40, 60],[136, 40, 64],[144, 44, 68],[152, 48, 72],[160, 48, 76],[168, 52, 80],[176, 56, 84],
		[184, 56, 88],[192, 60, 92],[200, 64, 96],[208, 64,100],[216, 68,104],[224, 72,108],[232, 72,112],[240, 76,116],
		[252, 80,120],[248, 80,120],[240, 76,116],[232, 76,112],[224, 72,108],[216, 68,104],[208, 68,100],[200, 64, 96],
		[192, 60, 92],[184, 60, 88],[176, 56, 84],[168, 56, 80],[160, 52, 76],[152, 48, 72],[144, 48, 68],[136, 44, 64],
		[128, 40, 60],[120, 40, 60],[112, 36, 56],[104, 36, 52],[ 96, 32, 48],[ 88, 28, 44],[ 80, 28, 40],[ 72, 24, 36],
		[ 64, 20, 32],[ 56, 20, 28],[ 48, 16, 24],[ 40, 16, 20],[ 32, 12, 16],[ 24,  8, 12],[ 16,  8,  8],[  8,  4,  4],
		[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],
		[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],
		[  0,  0,  0],[  0,  4,  0],[  0, 12,  0],[  0, 20,  0],[  0, 28,  0],[  0, 36,  0],[  0, 44,  0],[  0, 52,  0],
		[  0, 60,  0],[  0, 68,  0],[  0, 76,  0],[  0, 84,  0],[  0, 92,  0],[  0,100,  0],[  0,108,  0],[  0,116,  0],
		[  0,124,  0],[  0,132,  0],[  0,140,  0],[  0,148,  0],[  0,156,  0],[  0,164,  0],[  0,172,  0],[  0,180,  0],
		[  0,188,  0],[  0,196,  0],[  0,204,  0],[  0,212,  0],[  0,220,  0],[  0,228,  0],[  0,236,  0],[  0,244,  0],
		[  0,252,  0],[  0,248,  0],[  0,240,  0],[  0,232,  0],[  0,224,  0],[  0,216,  0],[  0,208,  0],[  0,200,  0],
		[  0,192,  0],[  0,184,  0],[  0,176,  0],[  0,168,  0],[  0,160,  0],[  0,152,  0],[  0,144,  0],[  0,136,  0],
		[  0,128,  0],[  0,120,  0],[  0,112,  0],[  0,104,  0],[  0, 96,  0],[  0, 88,  0],[  0, 80,  0],[  0, 72,  0],
		[  0, 64,  0],[  0, 56,  0],[  0, 48,  0],[  0, 40,  0],[  0, 32,  0],[  0, 24,  0],[  0, 16,  0],[  0,  8,  0],
		[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],
		[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],
		[  0,  0,  0],[  4,  4,  0],[ 12, 12,  0],[ 20, 20,  0],[ 28, 28,  0],[ 36, 36,  0],[ 44, 44,  0],[ 52, 52,  0],
		[ 60, 60,  0],[ 68, 68,  0],[ 76, 76,  0],[ 84, 84,  0],[ 92, 92,  0],[100,100,  0],[108,108,  0],[116,116,  0],
		[124,124,  0],[132,132,  0],[140,140,  0],[148,148,  0],[156,156,  0],[164,164,  0],[172,172,  0],[180,180,  0],
		[188,188,  0],[196,196,  0],[204,204,  0],[212,212,  0],[220,220,  0],[228,228,  0],[236,236,  0],[244,244,  0],
		[252,252,  0],[248,248,  0],[240,240,  0],[232,232,  0],[224,224,  0],[216,216,  0],[208,208,  0],[200,200,  0],
		[192,192,  0],[184,184,  0],[176,176,  0],[168,168,  0],[160,160,  0],[152,152,  0],[144,144,  0],[136,136,  0],
		[128,128,  0],[120,120,  0],[112,112,  0],[104,104,  0],[ 96, 96,  0],[ 88, 88,  0],[ 80, 80,  0],[ 72, 72,  0],
		[ 64, 64,  0],[ 56, 56,  0],[ 48, 48,  0],[ 40, 40,  0],[ 32, 32,  0],[ 24, 24,  0],[ 16, 16,  0],[  8,  8,  0],
		[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],
		[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],
		[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],
		[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0],[  0,  0,  0]
	],
	[
		[  0,  0,  0],[  0,  0,  0],[  0,  0,  4],[  0,  0, 12],[  0,  0, 16],[  0,  0, 24],[  0,  0, 32],[  0,  0, 36],
		[  0,  0, 44],[  0,  0, 48],[  0,  0, 56],[  0,  0, 64],[  0,  0, 68],[  0,  0, 76],[  0,  0, 80],[  0,  0, 88],
		[  0,  0, 96],[  0,  0,100],[  0,  0,108],[  0,  0,116],[  0,  0,120],[  0,  0,128],[  0,  0,132],[  0,  0,140],
		[  0,  0,148],[  0,  0,152],[  0,  0,160],[  0,  0,164],[  0,  0,172],[  0,  0,180],[  0,  0,184],[  0,  0,192],
		[  0,  0,200],[  0,  4,200],[  0, 12,200],[  0, 16,204],[  0, 24,204],[  0, 28,208],[  0, 36,208],[  0, 40,208],
		[  0, 48,212],[  0, 56,212],[  0, 60,216],[  0, 68,216],[  0, 72,216],[  0, 80,220],[  0, 84,220],[  0, 92,224],
		[  0,100,224],[  0,104,224],[  0,112,228],[  0,116,228],[  0,124,232],[  0,128,232],[  0,136,232],[  0,140,236],
		[  0,148,236],[  0,156,240],[  0,160,240],[  0,168,240],[  0,172,244],[  0,180,244],[  0,184,248],[  0,192,248],
		[  0,200,252],[  4,200,252],[ 12,200,252],[ 20,204,252],[ 28,204,252],[ 36,208,252],[ 44,208,252],[ 52,208,252],
		[ 60,212,252],[ 68,212,252],[ 76,216,252],[ 84,216,252],[ 92,216,252],[100,220,252],[108,220,252],[116,224,252],
		[124,224,252],[132,224,252],[140,228,252],[148,228,252],[156,232,252],[164,232,252],[172,232,252],[180,236,252],
		[188,236,252],[196,240,252],[204,240,252],[212,240,252],[220,244,252],[228,244,252],[236,248,252],[244,248,252],
		[252,252,252],[248,252,252],[244,252,252],[240,252,252],[232,252,252],[228,252,252],[224,252,252],[216,252,252],
		[212,252,252],[208,252,252],[200,252,252],[196,252,252],[192,252,252],[184,252,252],[180,252,252],[176,252,252],
		[168,252,252],[164,252,252],[160,252,252],[156,252,252],[148,252,252],[144,252,252],[140,252,252],[132,252,252],
		[128,252,252],[124,252,252],[116,252,252],[112,252,252],[108,252,252],[100,252,252],[ 96,252,252],[ 92,252,252],
		[ 84,252,252],[ 80,252,252],[ 76,252,252],[ 72,252,252],[ 64,252,252],[ 60,252,252],[ 56,252,252],[ 48,252,252],
		[ 44,252,252],[ 40,252,252],[ 32,252,252],[ 28,252,252],[ 24,252,252],[ 16,252,252],[ 12,252,252],[  8,252,252],
		[  0,252,252],[  0,248,252],[  0,244,252],[  0,240,252],[  0,232,252],[  0,228,252],[  0,224,252],[  0,216,252],
		[  0,212,252],[  0,208,252],[  0,200,252],[  0,196,252],[  0,192,252],[  0,184,252],[  0,180,252],[  0,176,252],
		[  0,168,252],[  0,164,252],[  0,160,252],[  0,156,252],[  0,148,252],[  0,144,252],[  0,140,252],[  0,132,252],
		[  0,128,252],[  0,124,252],[  0,116,252],[  0,112,252],[  0,108,252],[  0,100,252],[  0, 96,252],[  0, 92,252],
		[  0, 84,252],[  0, 80,252],[  0, 76,252],[  0, 72,252],[  0, 64,252],[  0, 60,252],[  0, 56,252],[  0, 48,252],
		[  0, 44,252],[  0, 40,252],[  0, 32,252],[  0, 28,252],[  0, 24,252],[  0, 16,252],[  0, 12,252],[  0,  8,252],
		[  0,  0,252],[  0,  0,248],[  0,  0,244],[  0,  0,240],[  0,  0,236],[  0,  0,232],[  0,  0,228],[  0,  0,224],
		[  0,  0,220],[  0,  0,216],[  0,  0,212],[  0,  0,208],[  0,  0,204],[  0,  0,200],[  0,  0,196],[  0,  0,192],
		[  0,  0,188],[  0,  0,184],[  0,  0,180],[  0,  0,176],[  0,  0,172],[  0,  0,168],[  0,  0,164],[  0,  0,160],
		[  0,  0,156],[  0,  0,152],[  0,  0,148],[  0,  0,144],[  0,  0,140],[  0,  0,136],[  0,  0,132],[  0,  0,128],
		[  0,  0,124],[  0,  0,120],[  0,  0,116],[  0,  0,112],[  0,  0,108],[  0,  0,104],[  0,  0,100],[  0,  0, 96],
		[  0,  0, 92],[  0,  0, 88],[  0,  0, 84],[  0,  0, 80],[  0,  0, 76],[  0,  0, 72],[  0,  0, 68],[  0,  0, 64],
		[  0,  0, 60],[  0,  0, 56],[  0,  0, 52],[  0,  0, 48],[  0,  0, 44],[  0,  0, 40],[  0,  0, 36],[  0,  0, 32],
		[  0,  0, 28],[  0,  0, 24],[  0,  0, 20],[  0,  0, 16],[  0,  0, 12],[  0,  0,  8],[  0,  0,  0],[  0,  0,  0]
	]]

	var max_iter = 256, xmin = -2., xmax = 1.2, ymin = -1.2, ymax = 1.2, which_palette = 0

	var canvas = document.getElementById(canvasId)
	var ctx = canvas.getContext("2d")
	var img = ctx.getImageData(0, 0, ctx.canvas.width, ctx.canvas.height)
	var pix = img.data

	var inp = document.URL
	if (inp.indexOf('?') >= 0) {
		var match = /[?&]d=([0-9a-fA-F]+)/.exec(inp)
		if (match.length == 2 && match[1].length >= 72) {
			var buf = new ArrayBuffer(4)
			var b = new Uint8Array(buf)
			var a = new Uint32Array(b)
			for (var i = 0; i < 4; ++i) b[i] = parseInt(match[1].substr(i*2,2), 16)
			max_iter = a[0]
			buf = new ArrayBuffer(32)
			b = new Uint8Array(buf)
			a = new Float64Array(b)
			for (var i = 0; i < 32; ++i) b[i] = parseInt(match[1].substr(i*2+8,2), 16)
			xmin = a[0]; xmax = a[1]; ymin = a[2]; ymax = a[3]
		}
	}

	function cal_mbrot(palette)
	{
		var w = ctx.canvas.width, h = ctx.canvas.height
		var x0, y0, w1 = 1. / w, h1 = 1. / h
		for (var j = 0, p = 0; j < h; ++j) {
			y0 = ymin + (ymax - ymin) * j * h1
			for (var i = 0; i < w; ++i) {
				var k, x = x0 = xmin + (xmax - xmin) * i * w1, y = y0
				for (k = 0; k < max_iter; ++k) {
					var z = x * y
					x *= x; y *= y
					if (x + y >= 4.) break
					x = x - y + x0
					y = z + z + y0 
				}
				k &= 0xff
				pix[p++] = palette[k][0]; pix[p++] = palette[k][1]; pix[p++] = palette[k][2]; pix[p++] = 255
			}
		}
	}

	function ev_mouse(ev)
	{
		// get coordinate
		var x, y
		if (ev.layerX || ev.layerX == 0) { // Firefox
			x = ev.layerX;
			y = ev.layerY;
		} else if (ev.offsetX || ev.offsetX == 0) { // Opera
			x = ev.offsetX;
			y = ev.offsetY;
		} // not working in IE8, but the whole script does not work (at least not well) anyway

		var is_shift = ev.shiftKey? 1 : 0
		var xc = xmin + (xmax - xmin) * x / ctx.canvas.width;
		var yc = ymin + (ymax - ymin) * y / ctx.canvas.height;
		var xstep = (xmax - xmin) * (is_shift == 0? .5 : 2.) * .5;
		var ystep = (ymax - ymin) * (is_shift == 0? .5 : 2.) * .5;
		xmin = xc - xstep; xmax = xc + xstep;
		ymin = yc - ystep; ymax = yc + ystep;
		cal_mbrot(palette[which_palette])
		ctx.putImageData(img, 0, 0)
	}

	function ev_keyboard(ev)
	{
		var replot = 0
		if (ev.keyCode >= 48 && ev.keyCode <= 57) { // 0-9
			var x = ev.keyCode == 48? 10 : ev.keyCode - 48
			max_iter = x<<8
			document.getElementById("message").innerHTML='Max iteration: '+max_iter
			replot = 1
		} else if (ev.keyCode == 112 || ev.keyCode == 80) {
			which_palette ^= 1
			replot = 1
		} else if (ev.keyCode == 100 || ev.keyCode == 68) {
			var b, a = new Uint32Array(1), str = ''
			a[0] = max_iter
			b = new Uint8Array(a)
			for (var i = 0; i < 4; ++i) str += b[i] < 16? '0'+b[i].toString(16) : b[i].toString(16)
			a = new Float64Array(4)
			a[0] = xmin; a[1] = xmax; a[2] = ymin; a[3] = ymax;
			b = new Uint8Array(a)
			for (var i = 0; i < 32; ++i) str += b[i] < 16? '0'+b[i].toString(16) : b[i].toString(16)
			alert(str)
		}
		if (replot) {
			cal_mbrot(palette[which_palette])
			ctx.putImageData(img, 0, 0)
		}
	}

	document.getElementById("message").innerHTML='Max iteration: '+max_iter
	if (canvas.addEventListener) canvas.addEventListener('click', ev_mouse, false);
	else canvas.attachEvent('onclick', ev_mouse);
	window.addEventListener('keydown', ev_keyboard, false); // I do not know if IE implements it

	cal_mbrot(palette[which_palette])
	ctx.putImageData(img, 0, 0)
}
