
import SimData

def create(app):
	v = SimData.Vector3
	vehicle = app.createVehicle
	vehicle("sim:vehicles.aircraft.m2k", v(483010, 499010, 190.2), v(0, 100.0, 0), v(2.0, 2.0, 140.0))
	vehicle("sim:vehicles.aircraft.m2k", v(483025, 499015, 190.2), v(0, 100.0, 0), v(5.0, 0.0, 240.0))
	vehicle("sim:vehicles.aircraft.m2k", v(483040, 499020, 190.2), v(0, 100.0, 0), v(8.0, 1.0, 240.0))
	vehicle("sim:vehicles.aircraft.m2k", v(483055, 499025, 190.2), v(0, 100.0, 0), v(12.0, 3.0, 240.0))

