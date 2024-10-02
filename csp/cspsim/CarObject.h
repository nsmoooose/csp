#pragma once

#include <csp/cspsim/DynamicObject.h>
#include <csp/cspsim/input/HID.h>

namespace csp {

	class CarObject: public DynamicObject
	{
	public:
		CSP_DECLARE_OBJECT(CarObject)

		DECLARE_INPUT_INTERFACE(CarObject, DynamicObject)
		END_INPUT_INTERFACE

	public:
		CarObject();
		virtual ~CarObject();
	};

} /* namespace csp */
