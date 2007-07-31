// Combat Simulator Project
// Copyright (C) 2002-2005 The Combat Simulator Project
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
 * @file AnimationSequence.h
 *
 **/

#ifndef __CSPSIM_ANIMATION_SEQUENCE_H__
#define __CSPSIM_ANIMATION_SEQUENCE_H__

#include <csp/cspsim/Bus.h>
#include <csp/csplib/util/Math.h>
#include <csp/csplib/data/Object.h>
#include <string>

CSP_NAMESPACE

/** A virtual base class for driving a shared animation path.  A
 *  shared animation path is a series of quasi-independent animations
 *  controlled by a single key.  Each independent animation uses a
 *  separate SharedSequence instance to drive a single shared channel
 *  with the animation key.
 *
 *  For example, for landing gear the shock absorber compression and
 *  gear retraction require the coordinated movement of many common
 *  parts and are conveniently expressed as a single animation path
 *  over disjoint ranges of the sequence key.  Key values ranging from
 *  0 to 1 might control shock absorber compression, while key values
 *  from 1 to 5 would retract the gear.  Each range would be covered
 *  by a separate SharedSequence instance.  The SharedSequences drive
 *  the animation key value over their respective ranges and coordinate
 *  to ensure that only one sequence is active at a time.
 *
 *  Exports 2 channels: the sequence key and a normalized key.  The
 *  sequence key channel is used to drive an animition path and can
 *  be shared by multiple shared sequence instances.  The normalized
 *  key channel is unique to each shared sequence instance and
 *  provides a value that ranges linearly from 0 to 1 as the sequence
 *  key varies between initial_key and final_key.
 *
 *  The normalized channel name is set either in xml or with the
 *  setNormalizedChannel method.  If a name is not specified, the
 *  normalized channel will not be created.  The normalized channel
 *  is a shared push channel that can also be used to control the
 *  sequence.  By setting the channel value and calling the push()
 *  method, the corresponding shared sequence instance will
 *  automatically update the sequence key channel to drive the
 *  animation provided that the sequence is enabled.
 *
 *  TODO is the push aspect of the normalized channel really useful,
 *  and if so would it be better to just export the entire shared
 *  sequence on the bus (giving access to the enable method)?
 */
class SharedSequence: public Object {

	class UpdateSequenceKey: public DataChannelBase::Handler {
		SharedSequence &m_SharedSequence;
	public:
		UpdateSequenceKey(SharedSequence &shared_sequence): m_SharedSequence(shared_sequence) { }
		virtual void operator()() { m_SharedSequence.updateSequenceKeyFromNormalizedChannel(); }
	};

	friend class UpdateSequenceKey;

public:
	CSP_DECLARE_ABSTRACT_OBJECT(SharedSequence)

	SharedSequence(): m_SequenceKey(0), m_NormalizedKey(0), m_Updater(0) { }
	virtual ~SharedSequence() { delete m_Updater; }

	virtual void setSequenceChannel(std::string const &name) { m_SequenceChannel = name; }
	virtual void setNormalizedChannel(std::string const &name) { m_NormalizedChannel = name; }

	virtual double normalizedKey() const { return m_NormalizedKey; }
	virtual double sequenceKey() const { return m_SequenceKey; }
	double initialKey() const { return m_InitialKey; }
	double finalKey() const { return m_FinalKey; }
	virtual void registerChannels(Bus *bus);

	/** Activate this sequence.  All other sequences sharing the output
	 *  channel will be disabled.
	 */
	virtual void enable() { update(); }

protected:
	typedef DataChannel<double>::RefT KeyChannel;
	KeyChannel const &sequenceKeyChannel() const { return b_SequenceKey; }
	KeyChannel const &normalizedKeyChannel() const { return b_NormalizedKey; }

	void setNormalizedKey(double key);
	void updateSequenceKeyFromNormalizedChannel();
	void setSequenceKey(double key);

private:
	void update();

	std::string m_SequenceChannel;
	std::string m_NormalizedChannel;
	double m_InitialKey;
	double m_FinalKey;
	double m_SequenceKey;
	double m_NormalizedKey;
	bool m_Enabled;
	UpdateSequenceKey *m_Updater;

	KeyChannel b_SequenceKey;    // shared channel
	KeyChannel b_NormalizedKey;  // push channel

};


class TimedSequence: public SharedSequence {
public:
	CSP_DECLARE_OBJECT(TimedSequence)

	TimedSequence(): m_Duration(0), m_Timescale(1.0), m_Playscale(0), m_Play(false) { }

	virtual void postCreate() {
		if (m_Duration > 0) m_Timescale = (finalKey() - initialKey()) / m_Duration;
	}

	void update(double dt) {
		if (m_Play) {
			double key = sequenceKey() + dt * m_Playscale;
			m_Play = (m_Playscale > 0) ? (key < finalKey()) : (key > initialKey());
			setSequenceKey(key);
		}
	}

	void play() {
		if (atEnd()) {
			playBackward();
		} else if (atStart()) {
			playForward();
		}
	}

	void playForward() {
		m_Play = true;
		m_Playscale = m_Timescale;
	}

	void playBackward() {
		m_Play = true;
		m_Playscale = -m_Timescale;
	}

	bool atStart() const { return (sequenceKey() - initialKey()) < 0.001 * m_Timescale; }
	bool atEnd() const { return (finalKey() - sequenceKey()) < 0.001 * m_Timescale; }

	void reverse() { m_Playscale = -m_Playscale; }
	void pause() { m_Play = false; }
	void unpause() { m_Play = true; }

	void jumpToTime(double t) { setSequenceKey(initialKey() + t * m_Timescale); }
	void jumpToEnd() { setSequenceKey(finalKey()); }
	void jumpToStart() { setSequenceKey(initialKey()); }

private:
	double m_Duration;
	double m_Timescale;
	double m_Playscale;
	bool m_Play;
};


class DrivenSequence: public SharedSequence {
public:
	CSP_DECLARE_OBJECT(DrivenSequence)

	DrivenSequence(): m_Scale(1.0), m_Offset(0.0) { }
	void setValue(double value) { setNormalizedKey(value * m_Scale + m_Offset); }

private:
	double m_Scale;
	double m_Offset;
};

CSP_NAMESPACE_END

#endif // __CSPSIM_ANIMATION_SEQUENCE_H__

