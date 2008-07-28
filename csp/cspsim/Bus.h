// Combat Simulator Project
// Copyright 2003, 2004 The Combat Simulator Project
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
 * @file Bus.h
 * @brief Classes for sharing data between vehicle subsystems.
 *
 * The bus and data channel infrastructure provides a higly flexible way of
 * interfacing disparate vehicle subsystems.  Rather than relying on base class
 * interfaces and virtual dispatch to connect subsystems, the bus architecture
 * provides dynamic data channels that are constructed when the systems are
 * first connected.  Individual systems export named channels, which provide
 * strongly-typed access to internal data.  These channels are registered with
 * a bus, which acts as a broker between the subsystems in a vehicle.  To
 * retrieve external data, systems request specific channels by name from the
 * bus.  Once a channel is retrieved, the channel instance can be used to
 * efficiently retrieve data from the system providing the channel.  The two
 * subsystems need no specific knowledge of each other, just an agreed set of
 * channel names and types.  Since the binding is dynamic (at runtime), systems
 * can provide fallback behavior in the event that a particular channel is
 * unavailable.  Channels provide both passive data transfer, as well as
 * push-style data updates and active (multi-parameter) callbacks via the
 * sigc++ library.
 *
 **/

/**
 * @namespace csp::bus
 * @brief Namespace used for data channel identifiers.
 */

#ifndef __CSPSIM_BUS_H__
#define __CSPSIM_BUS_H__

#include <csp/csplib/util/Log.h>
#include <csp/csplib/util/Callback.h>
#include <csp/csplib/util/Ref.h>
#include <csp/cspsim/Export.h>

#include <sigc++/sigc++.h>
#include <map>  // TODO move to .cpp

namespace csp {

class Bus;

/** Base class for channels on a bus.
 *
 *  Channels are reference counted objects that are stored in a bus and can be
 *  shared by components with access to the bus. Each component can provide
 *  channels to the bus and access channels provided by other components. Since
 *  the channels are identified by strings, this provides a very flexible way
 *  to share interfaces. Although the initial retrieval and connection of a
 *  channel involves some over- head, references to channels can be stored
 *  locally to provide low-overhead data and method sharing between components.
 */
class CSPSIM_EXPORT ChannelBase: public Referenced {
friend class Bus;

	/// Identifier for this channel.
	std::string m_Name;

	/// Bitmask for channel options.
	mutable unsigned int m_Mask;

protected:

	/// Channel state flags.
	enum {
		MASK_ENABLED = 0x00000001,
		MASK_BLOCKED = 0x00000002,
		MASK_SHARED  = 0x00000004,
		DERIVED_MASK = 0xffff0000
	};

	/** Access mode.
	 *
	 *  Local channels can only be modified by the owner, whereas
	 *  shared channel can be modified by all systems.
	 */
	typedef enum {
		ACCESS_LOCAL,
		ACCESS_SHARED
	} AccessType;

	/// Test bit flags used by derived classes (>= 0x10000).
	inline bool isMask(unsigned int bits) const {
		return (m_Mask & bits & DERIVED_MASK) != 0;
	}

	/// Set bit flags used by derived classes (>= 0x10000).
	inline void setMask(unsigned int bits) const {
		m_Mask |= (bits & DERIVED_MASK);
	}

	/// Clear bit flags used by derived classes (>= 0x10000).
	inline void clearMask(unsigned int bits) const {
		m_Mask &= ~(bits & DERIVED_MASK);
	}

	/// Get the flag state for derived classes (>= 0x10000).
	inline unsigned int getMask() const { return m_Mask & DERIVED_MASK; }

	/** Mark a channel as enabled or disabled.
	 *
	 *  The channel enabled/disabled flag is not actively
	 *  enforced.  It is the responsibility of code using
	 *  the channel to test the channel status and behave
	 *  accordingly.
	 */
	inline void setEnabled(bool enabled) {
		m_Mask = (m_Mask & ~MASK_ENABLED) | (enabled ? MASK_ENABLED : 0U);
	}

public:
	/// ChannelBase reference (convenience) type for shared channels.
	typedef Ref<ChannelBase> RefT;

	/// ChannelBase reference (convenience) type for non-shared channels.
	typedef Ref<ChannelBase const> CRefT;

	/** Get the (unique) identifier key for this channel.
	 */
	std::string const &getName() const { return m_Name; }

	/** Test if a channel is enabled.
	 */
	inline bool isEnabled() const { return (m_Mask & MASK_ENABLED) != 0; }

	/** Test if this channel is shared.
	 *
	 *  Shared channels can be updated by any system, whereas non-shared
	 *  channels are read-only (except for the system that creates them).
	 */
	inline bool isShared() const { return (m_Mask & MASK_SHARED) != 0; }

protected:
	/** Constructor.
	 *
	 *  @param name The name of the channel (must be unique on a bus).
	 *  @param access The access mode for the channel.  Once established
	 *    this cannot be changed.
	 */
	ChannelBase(std::string const &name, AccessType access = ACCESS_LOCAL):
		m_Name(name), m_Mask(0)
	{
		m_Mask |= (access == ACCESS_SHARED) ? MASK_SHARED : 0U;
	}
};


/** A channel for calling methods of a remote object.
 *
 *  Method channels to allow components on a bus to call methods of other
 *  components (and retrieve the results). This is somewhat similar to "pull"
 *  data channels, except that the called method can take arguments. The
 *  calling mechanism is based on libsigc++ signals (see the libsigc++
 *  documentation for details on using signals).
 */
template <class SIGNAL>
class MethodChannel: public ChannelBase {
	mutable SIGNAL m_Signal;
	sigc::connection m_Connection;
	typedef typename SIGNAL::InSlotType InSlotType;
public:
	typedef Ref<MethodChannel> RefT;
	typedef Ref<const MethodChannel> CRefT;
	MethodChannel(std::string const &name, InSlotType const &slot):
		ChannelBase(name)
	{
		m_Connection = m_Signal.connect(slot);
	}
	SIGNAL &call() const { return m_Signal; }
	sigc::connection &connection() { return m_Connection; }
};


/** Base class for data channels on a bus.
 *
 *  This class provides basic data channel functionality without specializing
 *  to a particular data type.  Data channels are reference counted objects.
 *  The references are stored both in the bus (which provides name-based
 *  channel lookups) and in individual systems as member variable references.
 *  The data channel may therefore persist even if the system originally
 *  defining the channel is destroyed.
 *
 *  Data channels support either push and pull based value updates, and must be
 *  specialized when the first signal handler is connected.  For push channels,
 *  any change to the channel value will send a signal to all callbacks
 *  connected to the channel.  For pull channels, any attempt to read a channel
 *  value will first send a signal to a callback that can update the value.  In
 *  this case only one callback may be connected to the channel, usually by the
 *  system that creates the channel.
 */
class DataChannelBase: public ChannelBase {
private:
	/// Callback signal for push/pull channels.
	mutable sigc::signal<void> m_Signal;

	enum {
		MASK_HANDLER = 0x00010000,
		MASK_PUSH    = 0x00020000,
		MASK_PULL    = 0x00040000,
		MASK_DIRTY   = 0x00080000
	};

protected:

	typedef enum {
		NO_SIGNAL,
		PUSH_SIGNAL,
		PULL_SIGNAL
	} SignalType;

	/** Construct a new channel.
	 *
	 *  @param name The name of the channel (used to generate the
	 *     identifier key).
	 *  @param access_ Shared channels may be accessed via the bus as
	 *     non-const references, allowing any system to update the channel
	 *     value.
	 *  @param signal_ Signal type to be used when the value changes.
	 */
	DataChannelBase(std::string const &name, AccessType access_=ACCESS_LOCAL, SignalType signal_=NO_SIGNAL): ChannelBase(name, access_) {
		if (signal_ == PUSH_SIGNAL) {
			setMask(MASK_PUSH);
		} else
		if (signal_ == PULL_SIGNAL) {
			setMask(MASK_PULL | MASK_DIRTY);
		}
	}

	/** Test if the channel has had at least one signal handler connected.
	 *
	 *  Note that handlers are not tracked beyond their initial connection to
	 *  the channel, so it is possible for this method to return true even if
	 *  no handlers are currently connected.  This flag is used as an internal
	 *  optimization for channels that have never been connected to a signal
	 *  handler.
	 */
	bool hasHandler() const { return isMask(MASK_HANDLER); }

	/** Send a signal to all connected handlers.
	 *
	 *  For push channels this is used to signal a change of the
	 *  data value, while for pull channels this signal is used
	 *  to update the data value.
	 */
	void signal() const { m_Signal.emit(); }

	/// Test if this is a push channel.
	bool isPush() const { return isMask(MASK_PUSH); }
	
	/// Test if this is a pull channel.
	bool isPull() const { return isMask(MASK_PULL); }

public:

	/** Mark this channel as dirty.
	 *
	 *  Used by pull channels to inform users of the channel that the current
	 *  data value is stale.  Calls to pull() when the channel is dirty will
	 *  result in a signal being sent to the channel provider, which should
	 *  update the value and clear the dirty flag by calling setClean().
	 */
	void setDirty() { setMask(MASK_DIRTY); }

	/** Mark this channel as clean.
	 *
	 *  @see setDirty().
	 */
	void setClean() { clearMask(MASK_DIRTY); }

	/** Test if the channel is dirty or clean.
	 *
	 *  @see setDirty().
	 */
	bool isDirty() const { return isMask(MASK_DIRTY); }

	/** A convenience class for defining Python signal handlers using SWIG.
	 */
	class Handler: public sigc::trackable {
	public:
		virtual ~Handler() {}
		void handle() { operator()(); }
		virtual void operator()(void)=0;
	};

	/** Connect a signal handler to this channel.
	 *
	 *  If push is true, this method may be called any number of times to
	 *  connect multiple handlers.  If push is false, only one callback can be
	 *  connected (additional calls to connect() will raise an assertion).
	 *
	 *  @param object The instance providing the handler.
	 *  @param callback The class method used to handle the signal.
	 */
	template <class T>
	sigc::connection connect(T *object, void (T::*callback)()) const {
		assert(isPush() || (isPull() && !hasHandler()));
		setMask(MASK_HANDLER);
		return m_Signal.connect(sigc::mem_fun(*object, callback));
	}

	/** Connect a signal handler to this channel.
	 *
	 *  @see connect(T*, void (T::*)(), bool).
	 *
	 *  @param handler The callback handler.
	 *  @param push Connect a push (or pull) handler.
	 */
	sigc::connection connect(Handler *handler) const {
		return connect(handler, &Handler::handle);
	}
	
};

/** Channel for passing data between systems over a Bus.
 *
 *  Channels are type-specialized objects which store a data value and can be
 *  shared (by reference) by multiple systems connected to a data bus.  In
 *  addition to the push/pull signaling functionality provided by the
 *  ChannelBase base class, this subclass defines accessor methods for the
 *  underlying data value.
 */
template <typename T>
class DataChannel: public DataChannelBase {
	mutable sigc::signal<bool, T const&> m_RequestSetSignal;
	bool m_HasRequestSetHandler;

	/// The data value provided by the channel.
	T m_Value;

	void pull() const {
		if (isDirty() && hasHandler() && !isPush()) {
			signal();
		}
	}

public:

	/// Channel reference (convenience) type for shared channels.
	typedef Ref<DataChannel<T> > RefT;

	/// Channel reference (convenience) type for non-shared channels.
	typedef Ref<DataChannel<T> const> CRefT;

	/** Push data to listeners on this channel.
	 *
	 *  Should only be called for push channels (asserts false otherwise).
	 */
	void push() const {
		assert(isPush());
		if (hasHandler() && isPush()) {
			signal();
		}
	}

	/** Set the channel value and signal the change to listeners.
	 *
	 *  Should only be called for push channels (asserts false otherwise).
	 */
	void push(const T& value) {
		m_Value = value;
		push();
	}

	/** Set the channel value and signal the change to listeners if the value has
	 *  changed.
	 *
	 *  Should only be called for push channels (asserts false otherwise).
	 */
	void pushOnChange(const T& value) {
		if (m_Value != value) {
			m_Value = value;
			push();
		}
	}

	/** Set a handler for a non-shared data channel that will be called when
	 *  a holder of a const reference to the channel requests that a new value
	 *  be set.  The handler can honor this request by setting the channel
	 *  (calling push if necessary) and returning true.  If the request is
	 *  not honored the handler should return false.
	 */
	void connectRequestSetHandler(callback<bool, T const &> &callback) {
		assert(!isShared() && !m_HasRequestSetHandler);
		callback.bind(m_RequestSetSignal);
		m_HasRequestSetHandler = true;
	}

	/** Request that the value be changed.  This method can only be called for
	 *  non-shared channels.  The handler, if registered, can decide whether to
	 *  honor the request.  If so, this method returns true.
	 */
	bool requestSet(const T& value) const {
		assert(!isShared());
		return m_HasRequestSetHandler ? m_RequestSetSignal.emit(value) : false;
	}

	/**
	 * Get the current value of this channel as a non-const reference.
	 *
	 * This method can be used as an lvalue to assign a new value to the
	 * channel.  Setting the value does not notify any listeners on this
	 * channel, so for push channels you may need to call push() explicitly.
	 */
	inline T &value() {
		return m_Value;
	}

	/** Get the value of a channel.
	 *
	 *  For pull channels, this method sends a signal to the update
	 *  handler before reading the data value.
	 */
	inline T const &value() const {
		if (isPull()) pull();
		return m_Value;
	}

	/** Construct and initialize a new channel.
	 *
	 *  This constructor can be used to create either a shared or
	 *  non-shared channel, and allows the channel data value to be
	 *  explicitly initialized.
	 *
	 *  @param name The string indetifier of the channel.
	 *  @param val The initial value of the channel data.
	 *  @param shared Create a shared (or non-shared) channel.
	 *  @param signal_ The signaling mechanism of the channel (push/pull/none).
	 */
	DataChannel(std::string const &name, T const &val, AccessType access_=ACCESS_LOCAL, SignalType signal_=NO_SIGNAL): DataChannelBase(name, access_, signal_), m_HasRequestSetHandler(false), m_Value(val) {}

	/** Construct and initialize a new channel.  The initial value is
	 *  determined by the default constructor for the channel data type.
	 *  This is useful for types that do not have public copy constructors.
	 *
	 *  @param name The string indetifier of the channel.
	 *  @param shared_ Create a shared (or non-shared) channel.
	 *  @param signal_ The signaling mechanism of the channel (push/pull/none).
	 */
	DataChannel(std::string const &name, AccessType access_=ACCESS_LOCAL, SignalType signal_=NO_SIGNAL): DataChannelBase(name, access_, signal_), m_HasRequestSetHandler(false), m_Value() {}

	static DataChannel<T> *newLocal(std::string const &name, T const &val) {
		return new DataChannel(name, val, ACCESS_LOCAL, NO_SIGNAL);
	}
	static DataChannel<T> *newLocalPull(std::string const &name, T const &val) {
		return new DataChannel(name, val, ACCESS_LOCAL, PULL_SIGNAL);
	}
	static DataChannel<T> *newLocalPush(std::string const &name, T const &val) {
		return new DataChannel(name, val, ACCESS_LOCAL, PUSH_SIGNAL);
	}
	static DataChannel<T> *newShared(std::string const &name, T const &val) {
		return new DataChannel(name, val, ACCESS_SHARED, NO_SIGNAL);
	}
	static DataChannel<T> *newSharedPull(std::string const &name, T const &val) {
		return new DataChannel(name, val, ACCESS_SHARED, PULL_SIGNAL);
	}
	static DataChannel<T> *newSharedPush(std::string const &name, T const &val) {
		return new DataChannel(name, val, ACCESS_SHARED, PUSH_SIGNAL);
	}
};


/** A data bus class for passing data between multiple Systems.
 *
 *  Bus instances are essentially just collections of data channels, providing
 *  some convenience methods for accessing channels and connecting systems.
 *  Systems connected by a Bus can exchange information synchonously or
 *  asynchronously using an efficient string-based interface.  No detailed
 *  (compile-time) knowledge of the system class interfaces is required, which
 *  allows for a much more flexible design of system class hierarchies.
 */
class CSPSIM_EXPORT Bus: public Referenced {

	/// name to channel map type
	typedef std::map<std::string, ChannelBase::RefT> ChannelMap;

	/// Map of all channels connected to the bus
	ChannelMap m_Channels;

	typedef std::map<std::string, std::vector<ChannelBase::RefT> > GroupMap;
	GroupMap m_Groups;

	/// Internal flag to help ensure proper bus construction.
	bool m_Bound;

	/// Internal state of the bus.
	bool m_Enabled;

	/// The identifier string of this bus.
	std::string m_Name;

	/// The status [0, 1] used for damage modelling.
	float m_Status;

public:
	/** Construct a new Bus.
	 *
	 *  @param name The name of the bus (currently only used for logging).
	 */
	Bus(std::string const &name);

	/** Test if a particular data channel is available.
	 *
	 *  @param name the name of the channel.
	 */
	bool hasChannel(std::string const &name) {
		return getChannel(name, false).valid();
	}

	/** Register a new channel.
	 *
	 *  This method is typically called from a system's registerChannels method
	 *  to register all data channels that the system provides via this bus.
	 *  The return value can be used to store a local reference to the new
	 *  channel.
	 *
	 *  @param channel The channel to register.
	 *  @param groups A space-separated list of group names.  The channel
	 *    will be associated wich each of these groups (not currently used).
	 */
	ChannelBase* registerChannel(ChannelBase *channel, std::string const &groups = "");

	/** Some registration helpers for data channels
	 */
	//@{
	template <typename T>
	ChannelBase *registerLocalDataChannel(std::string const &name, T const &init) {
		return registerChannel(DataChannel<T>::newLocal(name, init));
	}

	template <typename T>
	ChannelBase *registerLocalPullChannel(std::string const &name, T const &init) {
		return registerChannel(DataChannel<T>::newLocalPull(name, init));
	}

	template <typename T>
	ChannelBase *registerLocalPushChannel(std::string const &name, T const &init) {
		return registerChannel(DataChannel<T>::newLocalPush(name, init));
	}

	template <typename T>
	ChannelBase *registerSharedDataChannel(std::string const &name, T const &init) {
		return registerChannel(DataChannel<T>::newShared(name, init));
	}

	template <typename T>
	ChannelBase *registerSharedPullChannel(std::string const &name, T const &init) {
		return registerChannel(DataChannel<T>::newSharedPull(name, init));
	}

	template <typename T>
	ChannelBase *registerSharedPushChannel(std::string const &name, T const &init) {
		return registerChannel(DataChannel<T>::newSharedPush(name, init));
	}
	//@}

	/** Some registration helpers for method channels
	 */
	//@{
	template <typename R, class O, class O2>
	ChannelBase* registerMethodChannel(std::string const &name, O *obj, R (O2::*method)()) {
		return registerChannel(
			new MethodChannel< sigc::signal<R> >(name, sigc::slot<R>(obj, method))
		);
	}

	template <typename R, typename P1, class O, class O2>
	ChannelBase* registerMethodChannel(std::string const &name, O *obj, R (O2::*method)(P1)) {
		return registerChannel(
			new MethodChannel< sigc::signal<R,P1> >(name, sigc::slot<R,P1>(obj, method))
		);
	}

	template <typename R, typename P1, typename P2, class O, class O2>
	ChannelBase* registerMethodChannel(std::string const &name, O *obj, R (O2::*method)(P1,P2)) {
		return registerChannel(
			new MethodChannel< sigc::signal<R,P1,P2> >(name, sigc::slot<R,P1,P2>(obj, method))
		);
	}

	template <typename R, typename P1, typename P2, typename P3, class O, class O2>
	ChannelBase* registerMethodChannel(std::string const &name, O *obj, R (O2::*method)(P1,P2,P3)) {
		return registerChannel(
			new MethodChannel< sigc::signal<R,P1,P2,P3> >(name, sigc::slot<R,P1,P2,P3>(obj, method))
		);
	}

	template <typename R, typename P1, typename P2, typename P3, typename P4, class O, class O2>
	ChannelBase* registerMethodChannel(std::string const &name, O *obj, R (O2::*method)(P1,P2,P3,P4)) {
		return registerChannel(
			new MethodChannel< sigc::signal<R,P1,P2,P3,P4> >(name, sigc::slot<R,P1,P2,P3,P4>(obj, method))
		);
	}
	//@}
	
	/** Get the name of this bus.
	 */
	std::string const &getName() const { return m_Name; }

	/** Get a reference to a shared data channel.
	 *
	 *  This method will raise an assertion if the channel is non-shared.
	 *
	 *  @param name The name of the channel.
	 *  @param required If true, an assertion will be raised if the
	 *                  requested channel does not exist.  Otherwise
	 *                  missing channels will be returned as null
	 *                  references.
	 *  @returns A non-const reference to the data channel or a null
	 *           reference if the data channel is not found and required
	 *           is false.
	 */
	ChannelBase::RefT getSharedChannel(std::string const &name, bool required = true, bool override = false);

	/** Get a reference to a shared or non-shared data channel.
	 *
	 *  This method is identical to getSharedChannel, but returns a
	 *  const reference to the channel so that the data value may be
	 *  read but not modified.
	 *
	 *  @param name The name of the channel.
	 *  @param required If true, an assertion will be raised if the
	 *                  requested channel does not exist.  Otherwise
	 *                  missing channels will be returned as null
	 *                  references.
	 */
	ChannelBase::CRefT getChannel(std::string const &name, bool required = true);

	/** Convenience method to get or create a local data channel.
	 *
	 *  @param bus The bus providing the channel; may be null.
	 *  @param name The name of the channel.
	 *  @param default_value The default value for the new channel, if created.
	 *  @returns The named channel if it already exists.  Otherwise a new
	 *    local data channel with the specified default value is created and
	 *    returned.
	 */
	template <typename T>
	static ChannelBase::CRefT defaultDataChannel(Ref<Bus> bus, std::string const &name, T const &default_value) {
		typename DataChannel<T>::CRefT channel;
		if (bus.valid()) channel = bus->getChannel(name, false);
		if (!channel) {
			CSPLOG(DEBUG, OBJECT) << "Bus::defaultDataChannel(" << name << ") creating default.";
			channel = DataChannel<T>::newLocal(name, default_value);
		}
		return channel;
	}

	/** Convenience function for accessing a data channel that may be null.
	 *  (The channel reference must have already been already been retrieved
	 *  from the bus.)
	 *
	 *  @param channel A data channel that may be null.
	 *  @param fallback The value to return if the channel is null.
	 *  @returns The channel value or the fallback.
	 */
	template <typename T>
	static T readChannel(typename DataChannel<T>::CRefT &channel, T const &fallback) {
		return channel.isNull() ? fallback : channel->value();
	}

	/** Get the bus status value.
	 *
	 *  Bus degradation is not currently implemented.
	 *
	 *  @returns a value in the range [0, 1] representing the health of
	 *     the bus.  0 is completely non-functional, 1 is completely
	 *     functional.
	 */
	float getStatus() const { return m_Status; }

	/** Test if the bus is enabled.
	 *
	 *  If disabled, all data channels provided by the bus will be inactive.
	 *
	 *  @returns True if the bus is enabled.
	 */
	bool isEnabled() const { return m_Enabled; }

	/** Enable or disable the bus.
	 *
	 *  All data channel in the bus will be set to the corresponding state.
	 *
	 *  @param enabled True to enable the bus, False to disable it.
	 */
	void setEnabled(bool enabled);

	/** Change the bus status value.
	 *
	 *  Will enable or disable a proportionate number of channels.  Not
	 *  currently implemented.
	 *
	 *  @param status a value in the range [0, 1] representing the health
	 *    of the bus.  0 is completely non-functional, 1 is completely
	 *    functional.
	 */
	virtual void setStatus(float status) {
		m_Status = status;
		// disable or enable a proportionate number of accessors
	}
};

} // namespace csp

#endif // __CSPSIM_BUS_H__

