#pragma once
// Combat Simulator Project
// Copyright (C) 2005 The Combat Simulator Project
// http://csp.sourceforge.net
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


/**
 * @file DataEntry.h
 *
 **/

#include <csp/cspsim/f16/AlphaNumericDisplay.h>
#include <csp/cspsim/f16/NavigationSystem.h>
#include <csp/cspsim/Bus.h>

#include <csp/csplib/util/StringTools.h>
#include <csp/csplib/util/Conversions.h>
#include <csp/csplib/util/Math.h>
#include <csp/csplib/util/Ref.h>
#include <csp/csplib/data/Enum.h>
#include <csp/csplib/data/Vector3.h>

#include <cmath>

#if !defined(__GNUC__) && !defined(snprintf)
#define snprintf _snprintf
#endif

namespace csp {

//////
// todo
//  - move callbacks/accessors to another header and document them
//  - change datachannel interface to allow uniform access to push, pull, and method channels.

class CycleCallback: public Referenced {
public:
	typedef Ref<CycleCallback> RefT;
	virtual void increment()=0;
	virtual void decrement()=0;
	virtual int get() const=0;
	virtual void importChannels(Bus*) {}
};

class SimpleCycle: public CycleCallback {
public:
	typedef Ref<SimpleCycle> RefT;
	SimpleCycle(int initial, int min_, int max_, bool wrap): m_Value(initial), m_Min(min_), m_Max(max_), m_Wrap(wrap) {
		assert(initial >= min_ && initial <= max_);
	}
	virtual void increment() { m_Value++; if (m_Value > m_Max) m_Value = m_Wrap ? m_Min : m_Max; }
	virtual void decrement() { m_Value--; if (m_Value < m_Min) m_Value = m_Wrap ? m_Max : m_Min; }
	virtual int get() const { return m_Value; }
	void set(int value) { m_Value = std::min(m_Max, std::max(m_Min, value)); }
private:
	int m_Value;
	int m_Min;
	int m_Max;
	bool m_Wrap;
};


template <typename T>
class Accessor: public Referenced {
public:
	virtual bool readonly() const=0;
	virtual void set(T const &)=0;
	virtual T get() const=0;
	virtual void importChannels(Bus*) {}
};

template <typename T>
class ChannelAccessor: public Accessor<T> {
public:
	ChannelAccessor(std::string const &channel): m_ChannelName(channel) { }
	ChannelAccessor(typename DataChannel<T>::RefT channel): m_Channel(channel) { }
	virtual bool readonly() const { return false; }
	virtual void set(T const &x) { if (m_Channel.valid()) m_Channel->value() = x; }
	virtual T get() const { return !m_Channel ? T() : m_Channel->value(); }
	virtual void importChannels(Bus* bus) {
		if (!m_Channel && !m_ChannelName.empty()) {
			m_Channel = bus->getSharedChannel(m_ChannelName, false);
		}
	}
private:
	typename DataChannel<T>::RefT m_Channel;
	std::string m_ChannelName;
};

template <typename T>
class ChannelReadOnlyAccessor: public Accessor<T> {
public:
	ChannelReadOnlyAccessor(typename DataChannel<T>::CRefT channel): m_Channel(channel) { }
	virtual bool readonly() const { return true; }
	virtual void set(T const &) { assert(0); }
	virtual T get() const { return !m_Channel ? T() : m_Channel->value(); }
	virtual void importChannels(Bus* bus) {
		if (!m_Channel && !m_ChannelName.empty()) {
			m_Channel = bus->getChannel(m_ChannelName, false);
		}
	}
private:
	typename DataChannel<T>::CRefT m_Channel;
	std::string m_ChannelName;
};

template <typename T>
class LocalAccessor: public Accessor<T> {
public:
	LocalAccessor(): m_Value() { }
	LocalAccessor(T const &value): m_Value(value) { }
	virtual bool readonly() const { return false; }
	virtual void set(T const &x) { m_Value = x; }
	virtual T get() const { return m_Value; }
private:
	T m_Value;
};


// notes on channel access.
//
// from a channel provider point of view, one would prefer to export coarse grained
// interfaces.  for example, exporting a "steerpoint_system" is easier than exporting
// individual channels for active steerpoint number and active steerpoint data.  not
// to mention that active steerpoint data could be broken down into many finer channels.
// moreover, exporting a larger object makes it easy to enforce constraints on the
// individual pieces and eliminates the need for complicated push/pull interactions.
//
// there are at least two downsides to this approach.  first, the exported interface
// must be available to all systems that may use it at compile time.  contrast this
// with primitive types where the user need know nothing other than the type and the
// channel name.  primitive types are flexible in that any class can provide them,
// transparently, hiding all these details from the user.  even for coarse grained
// channel types the tendancy should be to provide the most primitive interface
// possible and let subtypes to all the interesting work.  using primitive types for
// channels is the extreme limit of this tendency, with the advantage that there is
// still an object that provides these channels and that can coordinate between them.
// that coordination is typically achieved using push/pull channels to create dynamic
// links between channels.  for example read(A) returns Alist[B] via a pull callback,
// such that write(B) will change the output of the next call to read(A).
//
// the second drawback concerns data-driven programming.  csplib provides fairly
// powerful ways to connect objects together at runtime based on external data.
// for some systems it would be very nice to provide only very basic class types
// than can then be connected in non-trivial ways via xml.  for example, the f-16
// data entry display consists of a large number of data-entry screens.  the naive
// approach is to write a separate subclass for each that imports the necessary
// channels from the bus and connects them to generic input widgets.  each subclass
// hard codes the widget layout and static text it displays.  this works but requires
// code changes to modify the layout and add new pages.  an attractive alternative
// would be to write a single, generic interface page that gets its layout from an
// xml definition via csplib.  specifying static text this way is easy, but
// connecting input widgets to the bus is difficult unless all the channels of
// interest are primitive types.  for example, an elevation entry widget can take
// a single channel input of type double (or int), but that requires every system
// that exports a position vector (or more complex object containing position
// vectors, like a steerpoint) to also export a separate channel for its z-component
// if it is to be editable via this mechanism.  but ideally the channel exporter
// shouldn't have to know or care about how the exported value will be used.
// so one alternative is to provide adapters on the importing end.  add a layer
// of abstraction between the data entry widget and the channel.   the elevation
// widget just needs an accessor object that can read and set double values.
// the value of interest may correspond to a channel<double>, a channel<Vector3>,
// or something more complex like a channel<Steerpoint>.  create the appropriate
// adapter, e.g. Vector3ChannelAccessor which implements the accessor interface for
// type double, and binds to a channel of type Vector3.  the adapter needs an
// integer parameter to know which channel (x, y, or z) to access and a channel
// name, which are supplied through its xml interface.  Lastly the Accessor needs
// access to the bus.
//
// of course there are downsides to this last approach as well.  first, adapters
// must be Objects, which may make them harder to use programatically.
// i'm not sure if this is a real limitation though, for example one can provide
// a non-default constructor for supplying the paramaters that would otherwise
// come from xml.  also, what could in principle be very lightweight adapter
// objects acquire all the baggage of the Object interface.  on the other hand
// much of this baggage is class static, and i expect that the total number of
// adapter classes and instances will be reasonably small in practice.

class ActiveSteerpointCycle: public CycleCallback {
public:
	ActiveSteerpointCycle() { }
	ActiveSteerpointCycle(std::string const &channel): m_Channel(channel) { }
	virtual void increment() {
		if (b_Navigation.valid()) b_Navigation->value()->nextSteerpoint();
	}
	virtual void decrement() { if (b_Navigation.valid()) b_Navigation->value()->prevSteerpoint(); }
	virtual int get() const { return (activeSteerpoint().valid()) ? activeSteerpoint()->index() : 0; }
	virtual void importChannels(Bus *bus) {
		b_Navigation = bus->getSharedChannel(m_Channel, false);
	}
private:
	Ref<Steerpoint> activeSteerpoint() const { return b_Navigation.valid() ? b_Navigation->value()->activeSteerpoint() : 0; }
	DataChannel<NavigationSystem::RefT>::RefT b_Navigation;
	std::string m_Channel;
};

class SteerpointIndexAccessor: public Accessor<int> {
public:
	SteerpointIndexAccessor() { }
	SteerpointIndexAccessor(std::string const &channel): m_Channel(channel) { }
	virtual bool readonly() const { return false; }
	virtual void set(int const &num) {
		if (m_Navigation.valid()) {
			m_Navigation->setActiveSteerpoint(num);
		}
	}
	virtual int get() const {
		return activeSteerpoint().valid() ? activeSteerpoint()->index() : 1;
	}
	virtual void importChannels(Bus *bus) {
		DataChannel<NavigationSystem::RefT>::RefT nav = bus->getSharedChannel(m_Channel, false);
		m_Navigation = nav.valid() ? nav->value() : 0;
	}
private:
	Ref<Steerpoint> activeSteerpoint() const { return m_Navigation.valid() ? m_Navigation->activeSteerpoint() : 0; }
	Ref<NavigationSystem> m_Navigation;
	std::string m_Channel;
};


class SteerpointCoordinateAccessor: public Accessor<double> {
public:
	typedef enum { LATITUDE, LONGITUDE, ELEVATION } Coordinate;
	SteerpointCoordinateAccessor(): m_Coordinate(LATITUDE) { }
	SteerpointCoordinateAccessor(Coordinate coordinate, std::string const &channel): m_Coordinate(coordinate), m_Channel(channel) { }
	virtual bool readonly() const { return false; }
	virtual void set(double const &coordinate) {
		if (activeSteerpoint().valid()) {
			LLA lla = activeSteerpoint()->lla();
			switch (m_Coordinate) {
				case LATITUDE: lla.setLatitudeInDegrees(coordinate); break;
				case LONGITUDE: lla.setLongitudeInDegrees(coordinate); break;
				case ELEVATION: lla.setAltitude(coordinate); break;
			};
			activeSteerpoint()->setPosition(lla);
		}
	}
	virtual double get() const {
		if (activeSteerpoint().valid()) {
			LLA lla = activeSteerpoint()->lla();
			switch (m_Coordinate) {
				case LATITUDE: return lla.latitudeInDegrees();
				case LONGITUDE: return lla.longitudeInDegrees();
				case ELEVATION: return lla.altitude();
			};
		}
		return 0.0;
	}
	virtual void importChannels(Bus *bus) {
		DataChannel<NavigationSystem::RefT>::RefT nav = bus->getSharedChannel(m_Channel, false);
		m_Navigation = nav.valid() ? nav->value() : 0;
	}
private:
	Ref<Steerpoint> activeSteerpoint() const { return m_Navigation.valid() ? m_Navigation->activeSteerpoint() : 0; }
	Ref<NavigationSystem> m_Navigation;
	Coordinate m_Coordinate;
	std::string m_Channel;
};

///////


/** A generic widget interface for rendering data on an AlphaNumericDisplay.
 */
class DataWidget: public Referenced {
public:
	typedef Ref<DataWidget> RefT;
	DataWidget(unsigned x, unsigned y): m_X(x), m_Y(y), m_Visible(true) { }
	virtual void render(AlphaNumericDisplay&)=0;
	virtual void importChannels(Bus*) { }
	unsigned x() const { return m_X; }
	unsigned y() const { return m_Y; }
	virtual void setVisible(bool visible) { m_Visible = visible; }
	bool visible() const { return m_Visible; }
private:
	unsigned m_X;
	unsigned m_Y;
	bool m_Visible;
};


/** An F-16 DED widget for cycling through a range of integer values.  The
 *  cycle is typically represented by an "up/dn arrows" character and responds
 *  to the INC/DEC rocker on the ICP.  The actual numeric value is rendered
 *  separately.
 */
class DataCycle: public DataWidget {
public:
	typedef Ref<CycleCallback> Callback;

	typedef Ref<DataCycle> RefT;

	/** Create a new data cycle.  The up/dn arrow symbol will be placed at the
	 *  specified x,y coordinates on the display.  The callback handles increment
	 *  and decrement events.
	 */
	DataCycle(unsigned x, unsigned y, CycleCallback *callback): DataWidget(x, y), m_Callback(callback) {
		assert(callback);
	}

	/** Simply draws an "^" character at the widget coordinates.  The DED font
	 *  should map this character to the appropriate arrow glpyh.
	 */
	virtual void render(AlphaNumericDisplay &ded) {
		if (visible()) ded.write(x(), y(), "^");
	}

	/** Handle the ICP INC event by delegating to the callback.
	 */
	virtual void increment() { m_Callback->increment(); }

	/** Handle the ICP DEC event by delegating to the callback.
	 */
	virtual void decrement() { m_Callback->decrement(); }

	/** Connect callback to the bus.
	 */
	virtual void importChannels(Bus* bus) { m_Callback->importChannels(bus); }

private:
	Callback m_Callback;
};


/** A base class for widgets that allow the pilot to enter new values via
 *  the ICP.  Provides stub methods for input events.
 */
class DataEntry: public DataWidget {
public:
	typedef Ref<DataEntry> RefT;

	DataEntry(unsigned x, unsigned y): DataWidget(x, y), m_Active(false) { }

	/** Handle ICP keys 0-9
	 */
	virtual void onNumber(int) { }

	/** Activate the entry widget.  Widget activation is typically handled
	 *  by DataEntryPage in response to UP/DN ICP events.
	 */
	virtual void setActive(bool active) { m_Active = active; }

	/** Test if the widget is active.
	 */
	virtual bool isActive() const { return m_Active; }

	/** Handle the ICP RCL event.
	 */
	virtual void recall() { }

	/** Handle the ICP ENTR event.  Returns true if the input focus should
	 *  be auto-advanced to the next data entry widget.
	 */
	virtual bool enter() { return false; }

private:
	bool m_Active;
};


/** F16 scratch pad for data entry.  Implements common scratchpad behavior for
 *  modifying a variable of the templated type.
 *
 *  The F16 scratch pad is a data entry widget for modifying a single variable.
 *  When selected the varaible is surrounded by astrisks ('*').  On input the
 *  variable is cleared and the area between the astrisks rendered in reverse
 *  video.  New values are entered digit-by-digit.  The RCL button can be used
 *  to undo the last digit entered.  Pressing RCL twice in a row aborts the
 *  change and restored the original value.  Pressing ENTR validates the input.
 *  If valid, the new value replaces the old value.  Otherwise the scratchpad
 *  flashes.
 *
 *  ScratchPad may be subclassed to implement specialized behavior by extending
 *  the format, formatEntry, and parse methods.
 */
template <typename T>
class ScratchPad: public DataEntry {
public:
	typedef Ref<Accessor<T> > Callback;
	typedef Ref<ScratchPad> RefT;

	/** Construct a new scratch pad.  The leading asterisk will be placed at the
	 *  specified (x,y) coordinates, followed by the value and a trailing asterisk.
	 *  Size is the number of digits that may be entered, and width is the space
	 *  between the asterisks.  Callback provides get/set accessors for the value.
	 */
	ScratchPad(unsigned x, unsigned y, unsigned size, unsigned width, Accessor<T> *callback):
		DataEntry(x, y),
		m_Enabled(true), m_Scratch(false), m_Recall(false), m_Flash(false),
		m_Size(size), m_Width(width), m_Callback(callback)
	{
		assert(callback && !callback->readonly());
		m_Original = get();
		m_Display = format(m_Original);
	}

	/** Render the scratchpad and its contents to the specified display.
	 */
	virtual void render(AlphaNumericDisplay& display) {
		if (visible()) {
			if (isActive()) {
				display.write(x(), y(), "*", AlphaNumericDisplay::INVERSE);
			}
			if (m_Scratch) {
				display.write(x() + 1, y(), m_Display, AlphaNumericDisplay::INVERSE);
			} else {
				display.write(x() + 1, y(), format(get()));
			}
			if (isActive()) {
				display.write(x() + m_Width + 1, y(), "*", AlphaNumericDisplay::INVERSE);
			}
		}
	}

	/** Handle an input key event.  The first (valid) event enters input mode and
	 *  subsequent digits are added to the scratch value.  The number of digits
	 *  that may be entered is limited by the size argument to the constructor.
	 *
	 *  Internally the digits are added sequentially to a string that is passed
	 *  to the formatEntry method for display formatting.  When ENTR is pressed
	 *  the string is passed to the parse method for conversion back to a numeric
	 *  value.  If valid the value is set via tha callback registered with the
	 *  constructor.
	 */
	virtual void onNumber(int x) {
		assert(x >= 0 && x <= 9);
		m_Recall = false;
		m_Flash = false;
		if (m_Enabled) {
			if (!m_Scratch) {
				m_Entry = static_cast<char>('0' + x);
				m_Scratch = formatEntry(m_Entry, m_Display);
			} else if (m_Entry.size() < m_Size) {
				std::string new_entry = m_Entry + static_cast<char>('0' + x);
				std::string new_display;
				if (formatEntry(new_entry, new_display)) {
					m_Entry = new_entry;
					m_Display = new_display;
				}
			}
		}
	}

	/** Mark the scratchpad as active or inactive.  Active scratchpads are marked
	 *  by asterisks.  This method also resets the scratchpad to its non-editing
	 *  state and cancels error flashing.
	 */
	virtual void setActive(bool active) {
		DataEntry::setActive(active);
		m_Scratch = false;
		m_Recall = false;
		m_Flash = false;
	}

	/** Enables scratchpad entry.
	 */
	virtual void enable() {
		m_Enabled = true;
	}

	/** Disables scratchpad entry and resets the scratchpad to its non-editing state.
	 */
	virtual void disable() {
		m_Enabled = false;
		m_Scratch = false;
		m_Recall = false;
		m_Flash = false;
	}

	/** Handle the RCL key, which is used to correct data entered into the scratchpad.
	 *  The first press undoes the last entered digit.  Two presses in a row cancels
	 *  editing and returns the entry to its original value.
	 */
	virtual void recall() {
		m_Flash = 0;
		if (m_Enabled) {
			if (m_Recall) {
				m_Scratch = false;
				m_Recall = false;
			} else if (m_Scratch) {
				if (!m_Entry.empty()) {
					m_Entry.erase(m_Entry.size() - 1);
					formatEntry(m_Entry, m_Display);
				}
				m_Recall = true;
			}
		}
	}

	/** Enters the new scratchpad value.  The value is first validated.  If invalid
	 *  the display will flash to indicate an error.
	 */
	virtual bool enter() {
		m_Recall = false;
		m_Flash = false;
		if (m_Enabled && m_Scratch) {
			T value;
			if (parse(m_Entry, value)) {
				m_Scratch = false;
				set(value);
				m_Original = get();
				m_Display = format(m_Original);
				return true;
			} else {
				m_Flash = true;
			}
		}
		return false;
	}

	/** Get the maximum number of digits that can be entered.
	 */
	unsigned size() const { return m_Size; }

	/** Get the width of the data entry/readout (space between the asterisks).
	 */
	unsigned width() const { return m_Width; }

	/** Connect callback to the bus.
	 */
	virtual void importChannels(Bus* bus) { m_Callback->importChannels(bus); }

protected:

	/** Format entry digits for display.  The input digits are appended sequentially
	 *  to a string.  This method determines how those digits are displayed during
	 *  entry.  Sets output to the display string and returns true if the entry is
	 *  valid, false otherwise.
	 */
	virtual bool formatEntry(std::string const &entry, std::string &output) {
		char buffer[128];
		snprintf(buffer, sizeof(buffer), "%*s", size(), entry.c_str());
		output = buffer;
		return true;
	}

	/** Format the numeric value of the field for display.  For example, an
	 *  altitude display might format a value of 150.7 as "   150FT".
	 */
	virtual std::string format(T const &) { return "XXX"; }

	/** Parse input digits and convert them to a new numeric value.  Returns
	 *  true if the input digits are valid or false otherwise.
	 */
	virtual bool parse(std::string const &, T &) { return false; }

	/** Get the current value (via the callback specified in the constructor).
	 */
	virtual T get() { return m_Callback->get(); }

	/** Set a new value (via the callback specified in the constructor).
	 */
	virtual void set(T const &value) { m_Callback->set(value); }

private:
	bool m_Enabled;
	bool m_Scratch;
	bool m_Recall;
	bool m_Flash;
	unsigned m_Size;
	unsigned m_Width;
	std::string m_Entry;
	std::string m_Display;
	T m_Original;
	Callback m_Callback;
};


class IntegerPad: public ScratchPad<int> {
public:
	IntegerPad(unsigned x, unsigned y, unsigned size, int minimum, int maximum, Accessor<int> *callback)
		: ScratchPad<int>(x, y, size, size, callback), m_Minimum(minimum), m_Maximum(maximum) { }
protected:
	virtual std::string format(int const &value) {
		char buffer[128];
		snprintf(buffer, sizeof(buffer), "%*d", size(), value);
		return buffer;
	}
	virtual bool parse(std::string const &input, int &value) {
		value = atoi(input.c_str());
		return !input.empty() && (value >= m_Minimum) && (value <= m_Maximum);
	}
private:
	int m_Minimum;
	int m_Maximum;
};


/** An entry widget for entering elevations in feet.
 */
class AltitudePad: public ScratchPad<double> {
public:
	AltitudePad(unsigned x, unsigned y, unsigned size, Accessor<double> *callback):
		ScratchPad<double>(x, y, size, size + 2, callback) { }
protected:
	virtual std::string format(double const &value) {
		char text[40];
		snprintf(text, sizeof(text), "%*dFT", size(), static_cast<int>(floor(value + 0.5)));
		return text;
	}
	virtual bool formatEntry(std::string const &entry, std::string &output) {
		char text[40];
		std::string adjusted = entry;
		// a leading 0 is used to enter negative values
		if (!adjusted.empty() && (adjusted[0] == '0')) {
			adjusted[0] = '-';
			// two leading 0's are not allowed
			if (adjusted.size() > 1 && adjusted[1] == '0') return false;
		}
		// right justify with units
		snprintf(text, sizeof(text), "%*sFT", size(), adjusted.c_str());
		output = text;
		return true;
	}
	virtual bool parse(std::string const &entry, double &value) {
		double sign = (entry.empty() || entry[0] != '0') ? 1.0 : -1.0;
		value = sign * atoi(entry.c_str());
		return !entry.empty();
	}
};


/** An entry widget for entering latitude/longitude coordinates.
 */
class CoordinatePad: public ScratchPad<double> {
public:
	typedef enum {LATITUDE, LONGITUDE} Type;
	CoordinatePad(unsigned x, unsigned y, Type type, Accessor<double> *callback):
		ScratchPad<double>(x, y, 9, 13, callback), m_Type(type) { }
protected:

	bool directionSign(char key, double &sign) {
		switch (key) {
			case '2': /*N*/ sign = +1.0; return m_Type == LATITUDE;
			case '4': /*W*/ sign = -1.0; return m_Type == LONGITUDE;
			case '6': /*E*/ sign = +1.0; return m_Type == LONGITUDE;
			case '8': /*S*/ sign = -1.0; return m_Type == LATITUDE;
			default: break;
		}
		return false;
	}

	char directionSymbol(double value) {
		return (m_Type == LATITUDE) ? ((value < 0) ? 'S':'N') : ((value < 0 ? 'W':'E'));
	}

	virtual std::string format(double const &value) {
		char buffer[40];
		const double clamp = (m_Type == LATITUDE) ? 90.0 : 180.0;
		const double cvalue = clampTo(value, -clamp, clamp);
		int degrees = static_cast<int>(cvalue);
		double minutes = std::abs(cvalue - degrees) * 60.0;
		snprintf(buffer, sizeof(buffer), "%c%3d\027 %6.3f'", directionSymbol(cvalue), std::abs(degrees), minutes);
		return std::string(buffer);
	}

	virtual bool formatEntry(std::string const &entry, std::string &output) {
		if (entry.empty()) {
			output = "";
			return true;
		}

		char dirkey = entry[0];
		double sign = 1.0;
		if (!directionSign(dirkey, sign)) return false;

		// parse subfields (not sure if this is the correct entry format)
		char direction = directionSymbol(sign);
		std::string degrees_part = (entry.size() > 1) ? entry.substr(1, 3) : "";
		std::string minutes_part = (entry.size() > 4) ? entry.substr(4, 2) : "";
		std::string fraction_part = (entry.size() > 6) ? entry.substr(6, 6) : "";

		// range check (not sure if this is the correct behavior)
		int degrees = atoi(degrees_part.c_str());
		if (degrees > ((m_Type == LATITUDE) ? 89 : 179)) return false;
		int minutes = atoi(minutes_part.c_str());
		if (minutes > 59) return false;

		// format scratchpad text (not sure if this is the correct display format)
		std::string assembled;
		assembled = assembled + direction + degrees_part;
		if (!minutes_part.empty()) assembled = assembled + ' ' + minutes_part;
		if (!fraction_part.empty()) assembled = assembled  + '.' + fraction_part;
		char buffer[40];
		snprintf(buffer, sizeof(buffer), "%12s", assembled.c_str());
		output = buffer;
		return true;
	}

	virtual bool parse(std::string const &entry, double &value) {
		if (entry.size() < 2) return false;
		const char direction = entry[0];
		double sign = 1.0;
		if (!directionSign(direction, sign)) return false;
		double degrees = 0;
		double minutes = 0;
		std::string degrees_part = (entry.size() > 1) ? entry.substr(1, 3) : "";
		std::string minutes_part = (entry.size() > 4) ? entry.substr(4, 5) : "";
		degrees = atoi(degrees_part.c_str());
		minutes = atoi(minutes_part.c_str()) * pow(10.0, 5.0 - minutes_part.size()) * 0.001;
		double abs_value = degrees + minutes / 60.0;
		if (m_Type == LATITUDE && abs_value > 90.0) return false;
		if (m_Type == LONGITUDE && abs_value > 180.0) return false;
		value = sign * abs_value;
		return true;
	};
	Type m_Type;
};


class SpeedDirectionWidget: public DataWidget {
	DataChannel<Vector3>::CRefT b_Vector;
	std::string m_Channel;
public:
	SpeedDirectionWidget(unsigned x, unsigned y, std::string const &channel): DataWidget(x, y), m_Channel(channel) {
	}

	virtual void importChannels(Bus *bus) {
		b_Vector = bus->getChannel(m_Channel, false);
	}

	virtual void render(AlphaNumericDisplay& display) {
		if (visible() && b_Vector.valid()) {
			Vector3 const &vector = b_Vector->value();
			double speed = convert::mps_kts(vector.length());
			int direction = static_cast<int>(toDegrees(atan2(vector.x(), vector.y())));
			if (direction < 0) direction += 360;
			display.write(x(), y(), "%03d\027", direction);
			display.write(x() + 4, y(), "%3d", static_cast<int>(speed));
		}
	}
};


class EnumMap {

public:
	EnumMap(std::string const &label_map) {
		parseLabelMap(label_map);
	}

	std::string getLabel(EnumLink const &link) {
		std::string setting = link.getToken();
		LabelMap::const_iterator iter = m_LabelMap.find(setting);
		return (iter != m_LabelMap.end()) ? iter->second : "";
	}

private:

	typedef std::map<std::string, std::string> LabelMap;
	LabelMap m_LabelMap;

	void parseLabelMap(std::string const &label_map) {
		StringTokenizer tokens(label_map, ",");
		for (std::deque<std::string>::const_iterator iter = tokens.begin(); iter != tokens.end(); ++iter) {
			std::string token = *iter;
			std::string::size_type colon = token.find(':');
			if (colon != std::string::npos && colon < token.size()) {
				m_LabelMap[token.substr(0, colon)] = token.substr(colon + 1);
				//std::cout << token.substr(0, colon) << " -> " << token.substr(colon + 1) << "\n";
			}
		}
	}

};


class EnumDisplayWidget: public DataWidget {
	DataChannel<EnumLink>::CRefT b_Setting;
	std::string m_Channel;
	EnumMap m_EnumMap;
public:
	EnumDisplayWidget(unsigned x, unsigned y, std::string const &channel, std::string const &label_map): DataWidget(x, y), m_Channel(channel), m_EnumMap(label_map) { }

	virtual void render(AlphaNumericDisplay& display) {
		if (visible() && b_Setting.valid()) {
			display.write(x(), y(), m_EnumMap.getLabel(b_Setting->value()));
		}
	}

	virtual void importChannels(Bus *bus) {
		b_Setting = bus->getChannel(m_Channel, false);
	}
};

} // namespace csp
