// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2003 The Combat Simulator Project
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
 *
 **/

/* TODO
 * 	add warning/debug logging
 */

#include <Tools.h>
#include <SimData/Ref.h>

#include <sigc++/signal_system.h>
#include <map>


class Bus;

/** Base class for channels on a bus.
 *
 *  Channels are reference counted objects that are stored in
 *  a bus and can be shared by components with access to the
 *  bus.  Each component can provide channels to the bus and
 *  access channels provided by other components.  Since the
 *  channels are identified by strings, this provides a very
 *  flexible way to share interfaces.  Although the initial
 *  retrieval and connection of a channel involves some over-
 *  head, references to channels can be stored locally to
 *  provide low-overhead data and method sharing between
 *  components.
 */
class ChannelBase: public simdata::Referenced {
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
	      DERIVED_MASK = 0xffff0000,
	};

	typedef enum {
		ACCESS_LOCAL,
		ACCESS_SHARED,
	} AccessType;

	bool isMask(unsigned int bits) const {
		return (m_Mask & bits & DERIVED_MASK) != 0;
	}

	void setMask(unsigned int bits) const {
		m_Mask |= (bits & DERIVED_MASK);
	}

	void clearMask(unsigned int bits) const {
		m_Mask &= ~(bits & DERIVED_MASK);
	}

	unsigned int getMask() const { return m_Mask; }

	/** Mark a channel as enabled or disabled.
	 *
	 *  The channel enabled/disabled flag is not actively
	 *  enforced.  It is the responsibility of code using
	 *  the channel to test the channel status and behave
	 *  accordingly.
	 */
	void setEnabled(bool enabled) {
		m_Mask = (m_Mask & ~MASK_ENABLED) | (enabled ? MASK_ENABLED : 0);
	}

public:
	/// ChannelBase reference (convenience) type for shared channels.
	typedef simdata::Ref< ChannelBase > Ref;

	/// ChannelBase reference (convenience) type for non-shared channels.
	typedef simdata::Ref< ChannelBase const > CRef;

	/** Get the (unique) identifier key for this channel.
	 */
	std::string const &getName() const { return m_Name; }

	/** Test if a channel is enabled.
	 */
	bool isEnabled() const { return m_Mask & MASK_ENABLED; }

	/** Test if this channel is shared.
	 * 
	 *  Shared channels can be updated by any system, whereas non-shared
	 *  channels are read-only (except for the system that creates them).
	 */
	bool isShared() const { return m_Mask & MASK_SHARED; }

protected:
	/** Constructor.
	 */
	ChannelBase(std::string const &name, AccessType access = ACCESS_LOCAL): 
		m_Name(name), m_Mask(0) 
	{
		m_Mask |= (access == ACCESS_SHARED) ? MASK_SHARED : 0;
	}
};


/** A channel for calling methods of a remote object.
 *
 *  Method channels to allow components on a bus to call
 *  methods of other components (and retrieve the results).
 *  This is somewhat similar to "pull" data channels, except
 *  that the called method can take arguments.  The calling
 *  mechanism is based on libsigc++ signals (see the
 *  libsigc++ documentation for details on using signals).
 */
template <class SIGNAL>
class MethodChannel: public ChannelBase {
	mutable SIGNAL m_Signal;
	SigC::Connection m_Connection;
	typedef typename SIGNAL::InSlotType InSlotType;
public:
	typedef simdata::Ref<MethodChannel> Ref;
	typedef simdata::Ref<const MethodChannel> CRef;
	MethodChannel(std::string const &name, InSlotType const &slot):
		ChannelBase(name)
	{
		m_Connection = m_Signal.connect(slot);
	}
	SIGNAL &call() const { return m_Signal; }
	SigC::Connection &connection() { return m_Connection; }
};


/** Base class for data channels on a bus.
 *
 *  This class provides basic data channel functionality without 
 *  specializing to a particular data type.  Data channels are
 *  reference counted objects.  The references are stored both in 
 *  the bus (which provides name-based channel lookups) and in 
 *  individual systems as member variable references.  The data
 *  channel may therefore persist even if the system originally
 *  defining the channel is destroyed.
 *
 *  Data channels support either push and pull based value updates,
 *  and must be specialized when the first signal handler is 
 *  connected.  For push channels, any change to the channel value 
 *  will send a signal to all callbacks connected to the channel.  
 *  For pull channels, any attempt to read a channel value will 
 *  first send a signal to a callback that can update the value.  
 *  In this case only one callback may be connected to the channel, 
 *  usually by the system that creates the channel.
 */
class DataChannelBase: public ChannelBase {
private:
	/// Callback signal for push/pull channels.
	mutable SigC::Signal0<void> m_Signal;

	enum {
	      MASK_HANDLER = 0x00010000,
	      MASK_PUSH    = 0x00020000,
	      MASK_PULL    = 0x00040000,
	      MASK_DIRTY   = 0x00080000,
	};

protected:

	typedef enum {
		NO_SIGNAL,
		PUSH_SIGNAL,
		PULL_SIGNAL,
	} SignalType;

	/** Construct a new channel.
	 *
	 *  @param name The name of the channel (used to generate the
	 *              identifier key).
	 *  @param shared Shared channels may be accessed via the bus as
	 *                non-const references, allowing any system to
	 *                update the channel value.
	 */
	DataChannelBase(std::string const &name, 
	                AccessType access=ACCESS_LOCAL, 
	                SignalType signal=NO_SIGNAL): 
		ChannelBase(name, access)
	{
		if (signal == PUSH_SIGNAL) {
			setMask(MASK_PUSH);
		} else
		if (signal == PULL_SIGNAL) {
			setMask(MASK_PULL | MASK_DIRTY);
		}
	}

	/** Test if the channel has had at least one signal handler
	 *  connected.
	 * 
	 *  Note that handlers are not tracked beyond their initial 
	 *  connection to the channel, so it is possible for this
	 *  method to return true even if no handlers are currently
	 *  connected.  This flag is used as an internal optimization
	 *  for channels that have never been connected to a signal
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

	/** Test if this is a push (or pull) channel.
	 */
	bool isPush() const { return isMask(MASK_PUSH); }
	bool isPull() const { return isMask(MASK_PULL); }

public:

	void setDirty() { setMask(MASK_DIRTY); }
	void setClean() { clearMask(MASK_DIRTY); }
	bool isDirty() const { return isMask(MASK_DIRTY); }

	/** A convenience class for defining Python signal handlers using SWIG.
	 */
	class Handler: public SigC::Object {
	public:
		virtual ~Handler() {}
		void handle() { operator()(); }
		virtual void operator()(void)=0;
	};

	/** Connect a signal handler to this channel.
	 *
	 *  If push is true, this method may be called any number of times to
	 *  connect multiple handlers.  If push is false, no additional calls
	 *  to <tt>connect</tt> may be made.  The first call to <tt>connect</tt>
	 *  permanently establishes whether the channel is push or pull.
	 *
	 *  @param object The instance providing the handler.
	 *  @param callback The class method used to handle the signal.
	 *  @param push Connect a push (or pull) handler.
	 */
	template <class T>
	SigC::Connection connect(T *object, void (T::*callback)()) const {
		assert(isPush() || (isPull() && !hasHandler()));
		setMask(MASK_HANDLER);
		return m_Signal.connect(SigC::slot(object, callback));
	}

	/** Connect a signal handler to this channel.
	 *
	 *  See <tt>connect(T*, void (T::*)(), bool)</tt>.
	 * 
	 *  @param handler The callback handler.
	 *  @param push Connect a push (or pull) handler.
	 */
	SigC::Connection connect(Handler *handler) const {
		return connect(handler, &Handler::handle);
	}
	
};

/** Channel for passing data between systems over a Bus.
 *  
 *  Channels are type-specialized objects which store a data
 *  value and can be shared (by reference) by multiple systems
 *  connected to a data bus.  In addition to the push/pull
 *  signaling functionality provided by the ChannelBase base
 *  class, this subclass defines accessor methods for the
 *  underlying data value.
 */
template <typename T>
class DataChannel: public DataChannelBase {

	/// The data value provided by the channel.
	T m_Value;

	void pull() const {
		if (isDirty() && hasHandler() && !isPush()) {
			signal();
		}
	}

public:

	/// Channel reference (convenience) type for shared channels.
	typedef simdata::Ref< DataChannel<T> > Ref;

	/// Channel reference (convenience) type for non-shared channels.
	typedef simdata::Ref< DataChannel<T> const > CRef;

	void push() const {
		assert(isPush());
		if (hasHandler() && isPush()) {
			signal();
		}
	}

	T &value() { 
		return m_Value; 
	}

	/** Get the value of a channel.
	 * 
	 *  For pull channels, this method sends a signal to the update
	 *  handler before reading the data value.
	 */
	T const &value() const { 
		pull();
		return m_Value; 
	}

	/** Construct and initialize a new channel.
	 *
	 *  This constructor can be used to create either a shared or 
	 *  non-shared channel, and allows the channel data value to be
	 *  explicitly initialized.
	 *
	 *  @param name The string indetifier of the channel.
	 *  @param value The initial value of the channel data.
	 *  @param shared Create a shared (or non-shared) channel.
	 */
	DataChannel(std::string const &name, T const &value, AccessType access=ACCESS_LOCAL, SignalType signal=NO_SIGNAL): m_Value(value), DataChannelBase(name, access, signal) {}

	static DataChannel<T> *newLocal(std::string const &name, T const &value) {
		return new DataChannel(name, value, ACCESS_LOCAL, NO_SIGNAL);
	}
	static DataChannel<T> *newLocalPull(std::string const &name, T const &value) {
		return new DataChannel(name, value, ACCESS_LOCAL, PULL_SIGNAL);
	}
	static DataChannel<T> *newLocalPush(std::string const &name, T const &value) {
		return new DataChannel(name, value, ACCESS_LOCAL, PUSH_SIGNAL);
	}
	static DataChannel<T> *newShared(std::string const &name, T const &value) {
		return new DataChannel(name, value, ACCESS_SHARED, NO_SIGNAL);
	}
	static DataChannel<T> *newSharedPull(std::string const &name, T const &value) {
		return new DataChannel(name, value, ACCESS_SHARED, PULL_SIGNAL);
	}
	static DataChannel<T> *newSharedPush(std::string const &name, T const &value) {
		return new DataChannel(name, value, ACCESS_SHARED, PUSH_SIGNAL);
	}
};


/** A data bus class for passing data between multiple Systems.
 *
 *  Bus instances are essentially just collections of data channels,
 *  providing some convenience methods for accessing channels and 
 *  connecting systems.  Systems connected by a Bus can exchange
 *  information synchonously or asynchronously using an efficient
 *  string-based interface.  No detailed (compile-time) knowledge
 *  of the system class interfaces is required, which allows for a
 *  much more flexible design of system class hierarchies.
 */
class Bus: public simdata::Referenced {

	/// name to channel map type
	typedef std::map<std::string, ChannelBase::Ref> ChannelMap;

	/// Map of all channels connected to the bus
	ChannelMap m_Channels;

	typedef std::map<std::string, std::vector<ChannelBase::Ref> > GroupMap;
	GroupMap m_Groups;

	/// Internal flag to help ensure proper bus construction.
	bool m_Bound;

	/// The identifier string of this bus.
	std::string m_Name;

	/// The status (0-1) used for damage modelling.
	float m_Status;

	/// Internal state of the bus.
	bool m_Enabled;
	
public:
	/// Bus reference (convenience) type
	typedef simdata::Ref<Bus> Ref;

	/** Test if a particular data channel is available.
	 */
	bool hasChannel(std::string const &name) {
		return getChannel(name).valid();
	}
		
	/** Register a new channel.
	 *
	 *  This method is typically called from a system's registerChannels
	 *  method to register all data channels that the system provides
	 *  via this bus.  The return value can be used to store a local
	 *  reference to the new channel.
	 */
	ChannelBase* registerChannel(ChannelBase *channel, std::string const &groups = "") {
		assert(channel);
		std::string name = channel->getName();
		assert(m_Channels.find(name) == m_Channels.end());
		m_Channels[name] = channel;
		StringTokenizer grouplist(groups, " ");
		StringTokenizer::iterator group = grouplist.begin();
		for (; group != grouplist.end(); ++group) {
			m_Groups[*group].push_back(channel);
		}
		return channel;
	}

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
			new MethodChannel< SigC::Signal0<R> >(name, SigC::slot(obj, method))
		);
	}

	template <typename R, typename P1, class O, class O2>
	ChannelBase* registerMethodChannel(std::string const &name, O *obj, R (O2::*method)(P1)) {
		return registerChannel(
			new MethodChannel< SigC::Signal1<R,P1> >(name, SigC::slot(obj, method))
		);
	}

	template <typename R, typename P1, typename P2, class O, class O2>
	ChannelBase* registerMethodChannel(std::string const &name, R (O2::*method)(P1,P2)) {
		return registerChannel(
			new MethodChannel< SigC::Signal2<R,P1,P2> >(name, SigC::slot(obj, method))
		);
	}

	template <typename R, typename P1, typename P2, typename P3, class O, class O2>
	ChannelBase* registerMethodChannel(std::string const &name, R (O2::*method)(P1,P2,P3)) {
		return registerChannel(
			new MethodChannel< SigC::Signal3<R,P1,P2,P3> >(name, SigC::slot(obj, method))
		);
	}

	template <typename R, typename P1, typename P2, typename P3, typename P4, class O, class O2>
	ChannelBase* registerMethodChannel(std::string const &name, R (O2::*method)(P1,P2,P3,P4)) {
		return registerChannel(
			new MethodChannel< SigC::Signal4<R,P1,P2,P3,P4> >(name, SigC::slot(obj, method))
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
	ChannelBase::Ref getSharedChannel(std::string const &name, bool required = true) {
		ChannelMap::iterator iter = m_Channels.find(name);
		if (iter == m_Channels.end()) {
			assert(!required);
			return 0;
		}
		assert(iter->second->isShared());
		return iter->second;
	}

	/** Get a reference to a shared or non-shared data channel.
	 *
	 *  This method is identical to getSharedChannel, but returns a
	 *  const reference to the channel so that the data value may be
	 *  read but not modified.
	 */
	ChannelBase::CRef getChannel(std::string const &name, bool required = true) {
		ChannelMap::iterator iter = m_Channels.find(name);
		if (iter == m_Channels.end()) {
			assert(!required);
			return 0;
		}
		return iter->second;
	}

	/** Get the bus status value.
	 */
	float getStatus() const { return m_Status; }

	/** Test if the bus is enabled.
	 */
	bool isEnabled() const { return m_Enabled; }

	/** Enable or disable the bus.
	 */
	void setEnabled(bool enabled) {
		if (enabled == m_Enabled) return;
		for (ChannelMap::iterator iter = m_Channels.begin();
		     iter != m_Channels.end(); ++iter) {
			iter->second->setEnabled(enabled);
		}
		m_Enabled = enabled;
	}

	/** Change the bus status value.
	 */
	virtual void setStatus(float status) {
		m_Status = status;
		// disable or enable a proportionate number of accessors
	}
	
	/** Construct a new Bus.
	 *
	 *  The bus is enabled by default.
	 *
	 *  @param name The name of the bus.
	 */
	Bus(std::string const &name): 
		m_Name(name), 
		m_Bound(false), 
		m_Status(1.0), 
		m_Enabled(true) 
	{
	}

};
	
