import sys

class Data: pass

class Table:
	def __init__(self, xbreaks, ybreaks, table):
		self.x = xbreaks
		self.y = ybreaks
		self.table = table

class hnum:
	def __init__(self, id, scale=1.0):
		self.id = id
		self.scale = scale
	def process(self, data, line):
		setattr(data, self.id, float(line)*self.scale);
		return 1

class hsub:
	def __init__(self, id, opts):
		self.id = id
		self.opts = opts
	def process(self, data, line):
		d = Data()
		line = line.split()
		for idx in range(len(line)):
			self.opts[idx].process(d, line[idx])
		if not hasattr(data, self.id):
			setattr(data, self.id, d);
		return 1

class hnumlist:
	def __init__(self, id, scale=1.0):
		self.id = id
		self.state = 0
		self.scale = scale
	def process(self, data, line):
		if self.state == 0:
			self.state = 1
			self.count = int(line)
			self.list = []
			return 0
		self.list += map(float, line.split())
		if len(self.list) < self.count: return 0
		self.list = map(lambda x, s=self.scale: x*s, self.list)
		setattr(data, self.id, self.list)
		return 1 

class htable:
	def __init__(self, id, xbreaks, ybreaks, scale=1.0):
		self.id = id
		self.current = []
		self.table = []
		self.xbreaks = xbreaks
		self.ybreaks = ybreaks
		self.scale = scale
	def process(self, data, line):
		xbreaks = getattr(data, self.xbreaks)
		ybreaks = getattr(data, self.ybreaks)
		self.current += map(float, line.split())
		if len(self.current) > len(xbreaks): raise "fubar table"
		if len(self.current) == len(xbreaks):
			self.current = map(lambda x, s=self.scale: x*s, self.current)
			self.table.append(self.current)
			if len(self.table) == len(ybreaks):
				table = Table(xbreaks, ybreaks, self.table)
				setattr(data, self.id, table)
				return 1
			self.current = []
		return 0


class ACData:

	handlers = [
		hnum("empty_weight"),
		hnum("reference_area"),
		hnum("internal_fuel"),
		hnum("aoa_max"),
		hnum("aoa_min"),
		hnum("beta_max"),
		hnum("beta_min"),
		hnum("max_g"),
		hnum("max_roll"),
		hnum("min_vcas"),
		hnum("max_vcas"),
		hnum("corner_vcas"),
		hnum("theta_max"),
		hnum("num_gear"),
		hsub("gear1", (hnum("x"), hnum("y"), hnum("z"), hnum("rng"))),
		hsub("gear2", (hnum("x"), hnum("y"), hnum("z"), hnum("rng"))),
		hsub("gear3", (hnum("x"), hnum("y"), hnum("z"), hnum("rng"))),
		hnum("cg_loc"),
		hnum("length"),
		hnum("span"),
		hnum("fuselage_radius"),
		hnum("tail_height"),
		hnumlist("aero_mach_breaks"),
		hnumlist("aero_alpha_breaks"),
		hnum("c_l_multiplier"),
		htable("c_l", "aero_alpha_breaks", "aero_mach_breaks"),
		hnum("c_d_multiplier"),
		htable("c_d", "aero_alpha_breaks", "aero_mach_breaks"),
		hnum("c_y_multiplier"),
		htable("c_y", "aero_alpha_breaks", "aero_mach_breaks"),
		hnum("thrust_multiplier"),
		hnum("fuel_flow_multiplier"),
		hnumlist("thrust_mach_breaks"),
		hnumlist("thrust_altitude_breaks", 0.3048),
		htable("idle_thrust", "thrust_mach_breaks", "thrust_altitude_breaks", 4.545),
		htable("mil_thrust", "thrust_mach_breaks", "thrust_altitude_breaks", 4.545),
		htable("ab_thrust", "thrust_mach_breaks", "thrust_altitude_breaks", 4.545),
	]



	def __init__(self):
		self.data = Data()

	def parse(self, f):
		idx = 0
		line_no = 0
		for line in f.xreadlines():
			line_no = line_no + 1
			comment = line.find('#')
			if comment >= 0:
				line = line[:comment]
			line = line.strip()
			if line == "": continue
			try:
				done = self.handlers[idx].process(self.data, line)
			except:
				print "Error parsing input, line %d" % line_no
				raise
			if done:
				idx = idx + 1
				if idx >= len(self.handlers): break

	class Tabpr:
		tabs = "\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
		def __init__(self, indent):
			indent = self.tabs[:indent]	
			self._indent = indent
		def __call__(self, *args):
			print "%s%s" % (self._indent, " ".join(map(str,args)))
		def indent(self, n=1):
			if n < 0: self._indent = self._indent[:n]
			if n > 0: self._indent = self._indent + self.tabs[:n]

	def table_xml(self, id, table, xspacing=1, yspacing=1, indent=0):
		tabpr = self.Tabpr(indent)
		tabpr("<Table name='%s'>" % id)
		tabpr.indent()
		tabpr("<XBreaks spacing='%s'>" % str(xspacing))
		tabpr.indent()
		tabpr(*table.x)
		tabpr.indent(-1)
		tabpr("</XBreaks>")
		tabpr("<YBreaks spacing='%s'>" % str(yspacing))
		tabpr.indent()
		tabpr(*table.y)
		tabpr.indent(-1)
		tabpr("</YBreaks>")
		tabpr("<Values>")
		tabpr.indent()
		for curve in table.table:
			tabpr(*curve)
		tabpr.indent(-1)
		tabpr("</Values>")
		tabpr.indent(-1)
		tabpr("</Table>")

	def engine_xml(self):
		self.table_xml("idle_thrust", self.data.idle_thrust, 0.1, 1000.0, 1)
		self.table_xml("mil_thrust", self.data.mil_thrust, 0.1, 1000.0, 1)
		self.table_xml("ab_thrust", self.data.ab_thrust, 0.1, 1000.0, 1)


f = sys.stdin

ac = ACData()
ac.parse(f)
ac.engine_xml()
