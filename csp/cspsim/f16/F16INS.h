#ifndef __CSPSIM_F16_F16_INS_H__
#define __CSPSIM_F16_F16_INS_H__

#include <csp/csplib/data/Quat.h>
#include <csp/cspsim/System.h>

namespace csp {

class F16INS: public System {
public:
	CSP_DECLARE_OBJECT(F16INS)

	F16INS();
	virtual ~F16INS();

	virtual void registerChannels(Bus* bus);
	virtual void importChannels(Bus* bus);

protected:
	virtual double onUpdate(double dt);

	DataChannel<Quat>::RefT b_INSAttitude;
	DataChannel<Quat>::CRefT b_Attitude;
	DataChannel<Vector3>::CRefT b_Position;
	DataChannel<Vector3>::CRefT b_Velocity;

	double m_ResponseTime;
};

} // namespace csp

#endif // __CSPSIM_F16_F16_INS_H__
