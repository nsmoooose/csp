#include <SimData/Ref.h>
#include <SimData/Key.h>
#include <map>
#include <sigc++/signal_system.h>


/*
   README

   This a preliminary implementation of a data bus architecture
   for sharing data between multiple systems comprising a detailed
   vehicle model.  This code is not yet part of CSPSim but has
   been added to CVS for safekeeping and public access.  Please
   direct comments to the csp developer's forum.
    
   The code compiles with gcc-3.3 under Linux, but has not been
   tested at all under Windows.  To build an executable you will
   need to install libsigc++ (Google: libsigc++).  The following
   command works for me but may need tweaking on other systems:

     g++-3.3 -I/usr/include/python2.2 
             -I/usr/include/sigc++-1.0 
             -I/usr/lib/sigc++-1.0/include 
             -L/usr/lib/python2.2/site-packages/SimData 
             -g -O2 
             bus.cpp 
             -lSimData -lsigc -lpthread
*/

class Bus;


/** Base class for vehicle system components that can be connected
 *  to a data bus.
 *
 *  Subclasses should implement registerChannels() and bind().  The
 *  former is used to expose internal data that can be accessed by 
 *  other systems over the bus, and is called automatically by the
 *  bus when the system is first attached.  The bind() method is
 *  called once by the bus after all systems have been attached and
 *  registered.  Use this method to bind to data channels provided 
 *  by other systems over the bus.
 */
class System: public simdata::Referenced, public SigC::Object {
friend class Bus;
protected:
	/** Register channels for access to internal data by other 
	 *  systems on the bus.
	 *
	 *  Use bus->getName() to select which channels to register 
	 *  if the system may be attached to more than one bus. 
	 */
	virtual void registerChannels(Bus *bus) {}

	/** Bind to channels provided by other systems on the bus.
	 * 
	 *  Call bus->getChannel() and bus->getSharedChannel() to
	 *  obtain references to data channels available on the
	 *  bus.  These references should generally be stored as
	 *  system member variables for later use.
	 */
	virtual void bind(Bus *bus) {}
public:

	/** System reference (convenience) type.
	 */
	typedef simdata::Ref< System > Ref;

	/** Destructor.
	 */
	virtual ~System() {}
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
class ChannelBase: public simdata::Referenced {
friend class Bus;
private:
	/// Channel state flags.
	enum {
	      ENABLED = 1,
	      BLOCKED = 2,
	      HANDLER = 4,
	      PUSH = 8,
	      SHARED = 16,
	};

	/// Hash key identifier for this channel.
	simdata::Key m_Key;

	/// Bitmask for channel options.
	mutable unsigned int m_Mask;

	/// Callback signal for push/pull channels.
	mutable SigC::Signal0<void> m_Signal;

protected:

	/** Get the (unique) identifier key for this channel.
	 */
	simdata::Key const &getKey() const { return m_Key; }

	/** Mark a channel as enabled or disabled.
	 *
	 *  The channel enabled/disabled flag is not actively
	 *  enforced.  It is the responsibility of code using
	 *  the channel to test the channel status and behave
	 *  accordingly.
	 */
	void setEnabled(bool enabled) {
		m_Mask = (m_Mask & ~ENABLED) | (enabled ? ENABLED : 0);
	}

	/** Construct a new channel.
	 *
	 *  @param name The name of the channel (used to generate the
	 *              identifier key).
	 *  @param shared Shared channels may be accessed via the bus as
	 *                non-const references, allowing any system to
	 *                update the channel value.
	 */
	ChannelBase(std::string const &name, bool shared=true): m_Mask(0) {
		simdata::Key key(name);
		m_Key = key;
		m_Mask |= shared ? SHARED : 0;
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
	int hasHandler() const { return m_Mask & HANDLER; }

	/** Send a signal to all connected handlers.
	 *
	 *  For push channels this is used to signal a change of the
	 *  data value, while for pull channels this signal is used
	 *  to update the data value.
	 */
	void signal() const { m_Signal.emit(); }

	/** Test if this is a push (or pull) channel.
	 */
	bool isPush() const { return m_Mask & PUSH; }

public:
	/// ChannelBase reference (convenience) type for shared channels.
	typedef simdata::Ref< ChannelBase > Ref;

	/// ChannelBase reference (convenience) type for non-shared channels.
	typedef simdata::Ref< ChannelBase const > CRef;

	/** Test if a channel is enabled.
	 */
	bool isEnabled() const { return m_Mask & ENABLED; }

	/** Test if this channel is shared.
	 * 
	 *  Shared channels can be updated by any system, whereas non-shared
	 *  channels are read-only (except for the system that creates them).
	 */
	bool isShared() const { return m_Mask & SHARED; }

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
	SigC::Connection connect(T *object, void (T::*callback)(), bool push=true) const {
		assert(push == isPush() || !hasHandler());
		if (push) m_Mask |= PUSH;
		m_Mask |= HANDLER;
		return m_Signal.connect(SigC::slot(object, callback));
	}

	/** Connect a signal handler to this channel.
	 *
	 *  See <tt>connect(T*, void (T::*)(), bool)</tt>.
	 * 
	 *  @param handler The callback handler.
	 *  @param push Connect a push (or pull) handler.
	 */
	SigC::Connection connect(Handler *handler, bool push=true) const {
		return connect(handler, &Handler::handle, push);
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
class Channel: public ChannelBase {

	/// The data value provided by the channel.
	T m_Value;

public:

	/// Channel reference (convenience) type for shared channels.
	typedef simdata::Ref< Channel<T> > Ref;

	/// Channel reference (convenience) type for non-shared channels.
	typedef simdata::Ref< Channel<T> const > CRef;

	/** Set the value of a channel.
	 * 
	 *  See set().
	 */
	T const & operator = (T const &value) {
		bool changed = hasHandler() && isPush() && (m_Value != value);
		m_Value = value;
		if (changed) {
			signal();
		}
	}

	/** Get the value of a channel.
	 * 
	 *  For pull channels, this method sends a signal to the update
	 *  handler before reading the data value.
	 */
	T const &get() const { 
		if (hasHandler() && !isPush()) {
			signal();
		}
		return m_Value; 
	}

	/** Set the value of a channel.
	 * 
	 *  For push channels this method sends a signal to all connected
	 *  handlers after the data value changes.
	 */
	void set(T const &value) { *this = value; }

	/** Construct a new shared channel.
	 *  
	 *  This constructor does not explicitly initialize the data value
	 *  provided by the channel.
	 *
	 *  @param name The string identifier of the channel.
	 */
	Channel(std::string const &name): ChannelBase(name, true) {}

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
	Channel(std::string const &name, T const &value, bool shared=true): m_Value(value), ChannelBase(name, shared) {}
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

	/// Key to system map type
	typedef std::map<simdata::Key, System::Ref> SystemMap;

	/// Key to channl map type
	typedef std::map<simdata::Key, ChannelBase::Ref> ChannelMap;

	/// Map of all systems connected to the bus
	SystemMap m_Systems;

	/// Map of all channels connected to the bus
	ChannelMap m_Channels;

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

	/** Attach a system to this bus.
	 *  
	 *  Any number of systems may be attached to the bus prior
	 *  to calling bind().  Systems are identified internally by 
	 *  a hash key generated from their name.  These keys must be
	 *  unique and an assertion will be raised if a system with
	 *  the same hash key has already been added to the bus.
	 *
	 *  This method calls the system's registerChannels() method.
	 *
	 *  @param system The system to attach.
	 *  @param name The string identifier of the system.
	 */
	void attachSystem(System::Ref system, std::string const &name) {
		assert(!m_Bound);
		assert(!hasSystem(name));
		simdata::Key key(name);
		m_Systems[key] = system;
		system->registerChannels(this);
	}

	/** Test if a particular data channel is available.
	 */
	bool hasChannel(std::string const &name) {
		return getChannel(name).valid();
	}
		
	/** Test if a particular system is connected.
	 */
	bool hasSystem(std::string const &name) {
		return getSystem(name).valid();
	}
	
	/** Register a new channel.
	 *
	 *  This method is typically called from a system's registerChannels
	 *  method to register all data channels that the system provides
	 *  via this bus.  The return value can be used to store a local
	 *  reference to the new channel.
	 */
	ChannelBase* registerChannel(ChannelBase *channel) {
		simdata::Key key = channel->getKey();
		assert(m_Channels.find(key) == m_Channels.end());
		m_Channels[key] = channel;
		return channel;
	}
	
	/** Bind all connected systems to data channels on the bus.
	 *
	 *  Bus construction is a two step process.  First systems are
	 *  added using attachSystem().  This method in turn calls each
	 *  system's registerChannels method to declare all the channels
	 *  that the system exports.  Once all systems are attached, 
	 *  bindSystems() calls each system's bind() method to allow it to
	 *  obtain references to data channels provided by other systems.
	 *  This is only done once, and subsequent calls to bindSystems()
	 *  have no effect.  
	 * 
	 *  It is currently an (assertion) error to call attachSystem() 
	 *  after bindSystems() has been called.  If the need arises
	 *  to provide greater flexibility for dynamically attaching
	 *  systems to the bus, this requirement can be relaxed.
	 */
	void bindSystems() {
		if (m_Bound) return;
		for (SystemMap::iterator iter = m_Systems.begin(); 
		     iter != m_Systems.end(); ++iter) { 
			iter->second->bind(this);
		}
		m_Bound = true;
	}

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
		simdata::Key key(name);
		ChannelMap::iterator iter = m_Channels.find(key);
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
		simdata::Key key(name);
		ChannelMap::iterator iter = m_Channels.find(key);
		if (iter == m_Channels.end()) {
			assert(!required);
			return 0;
		}
		return iter->second;
	}

	/** Get a system by name.
	 *
	 *  @returns A reference to the system or a null reference if the
	 *           system is not connected to the bus.
	 */
	System::Ref getSystem(std::string const &name) {
		simdata::Key key(name);
		SystemMap::iterator iter = m_Systems.find(key);
		if (iter == m_Systems.end()) return 0;
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
	



/////////////////////////////////////////////////////////////////////////////////////
// TESTING
/////////////////////////////////////////////////////////////////////////////////////

#include <SimData/Date.h>
#include <iostream>
#include <cstdio>

struct MFD: public System {
	void registerChannels(Bus *bus);
	Channel<int>::Ref m_MasterMode;
	Channel<int>::Ref m_Submode;
	void getSubMode() { *m_Submode = 42; }
};

void MFD::registerChannels(Bus *bus) {
	if (bus->getName()=="A") {
		m_MasterMode = bus->registerChannel(new Channel<int>("MasterMode", 0));
		m_Submode = bus->registerChannel(new Channel<int>("Submode", 1));
		m_Submode->connect(this, &MFD::getSubMode, false);
	}
	m_MasterMode->set(1);
}

struct FCS: public System {
	void bind(Bus *bus);
	void onMasterMode();
	Channel<int>::CRef m_MasterMode;
	Channel<int>::CRef m_Submode;
};

void FCS::bind(Bus *bus) {
	if (bus->getName()=="A") {
		m_MasterMode = bus->getChannel("MasterMode");
		m_MasterMode->connect(this, &FCS::onMasterMode);
		m_Submode = bus->getChannel("Submode");
		std::cout << m_Submode->get() << " is the answer?\n";

	}
}

void FCS::onMasterMode() {
	return;
	int mode = m_MasterMode->get();
	if (mode < 0) std::cout << "master mode set to " << mode << "\n";
	switch (mode) {
		case 0: // ...
			break;
		case 1: // ...
			break;
		// ...
	}
}

void AvionicsSuite() {
	std::cout << "avionics suite\n";
	Bus::Ref busA = new Bus("A");
	System::Ref m_FCS = new FCS();
	System::Ref m_MFD = new MFD();
	busA->attachSystem(m_FCS, "FCS");
	busA->attachSystem(m_MFD, "MFD");
	busA->bindSystems();
	Channel<int>::Ref mm = busA->getSharedChannel("MasterMode");
	Channel<int>::Ref sm = busA->getSharedChannel("Submode");
	std::cout << sizeof(*mm) << " bytes\n";
	simdata::SimTime t0, t1;
	t0 = simdata::SimDate::getSystemTime();
	for (int i = 0; i < 1000000; i++) {
		mm->set(i);
	}
	t1 = simdata::SimDate::getSystemTime();
	std::cout << (t1-t0) << " us\n";
	t0 = simdata::SimDate::getSystemTime();
	for (int i = 0; i < 1000000; i++) {
		sm->set(i);
	}
	t1 = simdata::SimDate::getSystemTime();
	std::cout << (t1-t0) << " us\n";
};

int main() {
	AvionicsSuite();
	return 0;
}

