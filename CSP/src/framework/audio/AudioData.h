#ifndef __AUDIO_DATA_H__
#define __AUDIO_DATA_H__

/** This is a containter for loading and storing sounds in various
	formats. For now it just supports uncompressed WAV-files, but
	feel free to add some more stuff.
	@date $Date: 2002/09/24 22:09:04 $
	@version $Revision: 1.1 $
	@author $Author: boddman $*/
class AudioData
{
public:
	/// Constructor. Its will set some value to initial values.
	AudioData();
	/// Releases the used memory.
	~AudioData();

	/** Loads a soundfile. For now, just raw uncompressed WAV-format is supported.
		The needed memory is allocated.
		@param _filename Filename to an existing sound-file
		@return 0 if everything went okay, -1 else*/
	int   Load(char *_filename);

  /// Returns the number of bits per sample.
  int   GetBits()         {return m_bits;}
  /// Returns the number of bytes per sample.
  int   GetBPSample()     {return m_bpsample;}
  /// Returns the number of bytes per second
  int   GetBPSecond()     {return m_bpsecond;}
  /// Returns the pointer to the data-buffer. Do not modify it directly !
  char *GetBuffer()       {return m_data;}
  /// Returns the number of channels.
  int   GetChannels()     {return m_channels;}
  /// Return the length of the sound (-sample).
  int   GetSampleLength() {return m_samplelen;}
  /// Returns the sample-rate.
  int   GetSampleRate()   {return m_samplerate;}

  /// Index of the used soundbuffer
  int   m_soundbuffer;
  /// Indicates the current status. If sound is playing, this var will be set to 1.
  char  m_playing;

private:
	/// This resets all the internal values, allocated memory will be freed !
	int       Reset();

	char     *m_name;
	char     *m_data;
	long int  m_samplelen, m_samplerate, m_bpsecond;
	char      m_bpsample, m_channels, m_bits;
};

#endif