#ifdef DirectAudio

#include <WinDXAudio.h>

// ------------------------------------------
//  Constructor / Deconstructor
// ------------------------------------------
WinDXAudio::WinDXAudio()
{
  // some internal stuff
	lpdsbPrimary=NULL;
	lpds=NULL;
}

WinDXAudio::~WinDXAudio()
{
  Uninitialize();
}



/*
 *
 *
 *
 *
 -------------------------------------------
 HERE STARTS THE IMPORTANT STUFFFFFFFFFFFFF
 -------------------------------------------
 *
 *
 *
 *
 *
 */
int WinDXAudio::Initialize(StandardApp *App, _HANDLE WindowHandle)
{
  p_App = App;

  // ------

	HRESULT dsrval;

  // Init called a second time !
  if (lpds || lpdsbPrimary)
    return -1;

  // now we try to get DirectSound under control
  ZeroMemory( &dsbd, sizeof(DSBUFFERDESC));
  dsbd.dwSize = sizeof(DSBUFFERDESC);
  dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME ;
  //dsrval = DirectSoundCreate( (LPCGUID)NULL , &lpds, NULL );  
  dsrval = DirectSoundCreate( (struct _GUID *)NULL , &lpds, NULL );

  if( dsrval!=DS_OK) return(-1);

  //Get caps
  ZeroMemory(&dscaps, sizeof(DSCAPS));
  lpds->GetCaps(&dscaps);
	//if (platform)
  dsrval = lpds->SetCooperativeLevel((HWND)WindowHandle, DSSCL_EXCLUSIVE );
  if (dsrval!=DS_OK) return(-1);

  // try to create the Primary Soundbuffer
  dsrval = lpds->CreateSoundBuffer( &dsbd, &lpdsbPrimary, NULL );
	if (dsrval!=DS_OK) return(-1);
  if (lpdsbPrimary==NULL) return -1;

  // We have the buffer, now lets play it in a loop :D
  dsrval=lpdsbPrimary->Play(0,0,DSBPLAY_LOOPING);
  if (dsrval==DSERR_BUFFERLOST) {
    lpdsbPrimary->Restore();
    dsrval=lpdsbPrimary->Play(0,0,DSBPLAY_LOOPING);
  }
  if (dsrval!=DS_OK)
    return -1;
  return 0;
}


/*
 Reset the DirectSound-handlers
 */
int WinDXAudio::Uninitialize()
{
  for (int i=0; i<m_listdata.size(); ++i)
    if (m_listdata[i]) delete m_listdata[i];
  m_listdata.erase(m_listdata.begin(), m_listdata.end());

  if (lpdsbPrimary!=NULL)
  {
    lpdsbPrimary->Stop();
    lpdsbPrimary->Release();
  }
  lpdsbPrimary=NULL;

  if (lpds)
    lpds->Release();
  lpds=NULL;
  return 0;
}


/*
 Load datas from a file
 */
int WinDXAudio::Load_Sound(char *_filename)
{
  int        retcode, t;
  AudioData *data;

  // create a new object, try to load
  data=new AudioData();
  retcode=data->Load(_filename);
  if (retcode!=0)
    {
      delete data;
      return _SOUND_LOADING;
    }

  // find a free entry in the list (if one sound was discarded before)
  for (t=0; t<m_listdata.size(); t++)
    if (m_listdata[t]==NULL) break;

  if (t==m_listdata.size())
  {
    // no free entry found, add at the end of the list
    m_listdata.insert(m_listdata.end(), data);
    return m_listdata.size()-1;
  } else {
    // add right here
    m_listdata[t]=data;
    return t;
  }
}


/*
*/
int WinDXAudio::Cache_Sound(int _handle, char _static)
{
  int t;
  LPDIRECTSOUNDBUFFER sbuffer;

  if (_handle<0 || _handle>=m_listdata.size())
    return _SOUND_OUTOFRANGE;

  if (m_listdata[_handle]->m_soundbuffer>=0)
    return -1;

  sbuffer=CreateSB(m_listdata[_handle], _static);
  if (sbuffer==NULL) return _SOUND_CREATE_SECONDARY;

  sbuffer->SetCurrentPosition(0);
  // look for a free entry
  for (t=0; t<m_listbuffer.size(); t++)
    if (m_listbuffer[t]==NULL) break;

  if (t==m_listbuffer.size())
  {
    // no free entry, add at the end of the buffer-list
    m_listbuffer.insert(m_listbuffer.end(), sbuffer);
    m_listdata[_handle]->m_soundbuffer=m_listbuffer.size()-1;
  } else {
    // add the sound here ...
    m_listbuffer[t]=sbuffer;
    m_listdata[_handle]->m_soundbuffer=t;
  }
  return 0;
}

/*
  Play the sound .. yeah
 */
int WinDXAudio::Play_Sound(int _handle, int _volume, int _looped)
{
  LPDIRECTSOUNDBUFFER  sbuffer;
  AudioData           *dat;

  if (_handle<0 || _handle>m_listdata.size())
    return _SOUND_OUTOFRANGE;

  // Play(...DSBPLAY_TERMINATEBY_TIME);
  dat=m_listdata[_handle];
  // if not prepared, load into SecondBuffer now
  if (dat->m_soundbuffer==-1)
  {
    sbuffer=CreateSB(dat,1);
    if (sbuffer==NULL) return _SOUND_CREATE_SECONDARY;
    m_listbuffer.insert(m_listbuffer.end(), sbuffer);
    dat->m_soundbuffer=m_listbuffer.size()-1;;
  }

  // start the playing
  sbuffer=m_listbuffer[dat->m_soundbuffer];
  sbuffer->SetCurrentPosition(0);
  if (_looped)
    dsrval=sbuffer->Play(0,0, DSBPLAY_LOOPING);
  else
    dsrval=sbuffer->Play(0,0, 0);

  if (dsrval==DSERR_BUFFERLOST)
    {
      sbuffer->Restore();
      sbuffer->Restore();
      if (_looped)
	dsrval=sbuffer->Play(0,0, DSBPLAY_LOOPING);
      else
	dsrval=sbuffer->Play(0,0, 0);
    }
  
  if (dsrval!=DS_OK) return _SOUND_PLAY_FAILED;
  return 0;
}


/*
 */
int WinDXAudio::Stop_Sound(int _handle)
{
  if (_handle<0 || _handle>m_listbuffer.size())
    return _SOUND_OUTOFRANGE;

  dsrval=m_listbuffer[m_listdata[_handle]->m_soundbuffer]->Stop();
  if (dsrval!=DS_OK) return -1;

  m_listdata[_handle]->m_playing=0;
  return 0;
}


int WinDXAudio::Continue_Sound(int _handle, int _volume, int _looped)
{
  LPDIRECTSOUNDBUFFER  sbuffer;
  AudioData           *dat;

  if (_handle<0 || _handle>m_listdata.size())
    return _SOUND_OUTOFRANGE;

  // Play(...DSBPLAY_TERMINATEBY_TIME);
  dat=m_listdata[_handle];
  // if not prepared, load into SecondBuffer now
  if (dat->m_soundbuffer==-1)
  {
    sbuffer=CreateSB(dat,1);
    if (sbuffer==NULL) return _SOUND_CREATE_SECONDARY;
    sbuffer->SetCurrentPosition(0);
    m_listbuffer.insert(m_listbuffer.end(), sbuffer);
    dat->m_soundbuffer=m_listbuffer.size()-1;;
  }

  // start the playing
  sbuffer=m_listbuffer[dat->m_soundbuffer];
  if (_looped)
    dsrval=sbuffer->Play(0,0, DSBPLAY_LOOPING);
  else
    dsrval=sbuffer->Play(0,0, 0);
	if (dsrval==DSERR_BUFFERLOST)
  {
		sbuffer->Restore();
    if (_looped)
      dsrval=sbuffer->Play(0,0, DSBPLAY_LOOPING);
    else
      dsrval=sbuffer->Play(0,0, 0);
	}

  if (dsrval!=DS_OK) return _SOUND_PLAY_FAILED;
  return 0;
}


/*
  Uncache the sound, remove it from (soundcards) memory, but keep it in RAM
*/
int WinDXAudio::UnCache_Sound(int _handle)
{
  DWORD status;

  if (_handle<0 || _handle>m_listbuffer.size())
    return _SOUND_OUTOFRANGE;

  // check if cached
  if (m_listdata[_handle]->m_soundbuffer==-1) return -1;

  // check for playing
  if (m_listdata[_handle]->m_playing>0)
  {
    m_listbuffer[m_listdata[_handle]->m_soundbuffer]->GetStatus(&status);
    if (status & DSBSTATUS_PLAYING)
      m_listbuffer[m_listdata[_handle]->m_soundbuffer]->Stop();
    m_listdata[_handle]->m_playing=0;
  }
  dsrval=m_listbuffer[m_listdata[_handle]->m_soundbuffer]->Release();
  m_listbuffer[m_listdata[_handle]->m_soundbuffer]=NULL;
  m_listdata[_handle]->m_soundbuffer=-1;
  return 0;
}


/*
  kill the sound completely, remove it from soundcards memory and from RAM
*/
int WinDXAudio::Discard_Sound(int _handle)
{
  if (_handle<0 || _handle>m_listbuffer.size())
    return _SOUND_OUTOFRANGE;

  UnCache_Sound(_handle);
  delete m_listdata[_handle];
  m_listdata[_handle]=NULL;
  return 0;
}


/*
 *
 *
 NOW INTERNAL STUFF
 *
 *
 */
LPDIRECTSOUNDBUFFER WinDXAudio::CreateSB(AudioData *_data, int _static)
{
  DSBUFFERDESC	       dsbd2;
  PCMWAVEFORMAT        pcmwf;
  LPDIRECTSOUNDBUFFER  tmp;
  void                *p1, *p2;
  DWORD                len1,len2;

  // prepare the sample to be stored in the buffer
  if (_data->GetSampleLength()<1) return NULL;
  memset(&pcmwf, 0, sizeof(PCMWAVEFORMAT));
  pcmwf.wf.wFormatTag = WAVE_FORMAT_PCM;
  pcmwf.wf.nChannels = (char)abs(_data->GetChannels());
  pcmwf.wf.nSamplesPerSec = _data->GetSampleRate();
  pcmwf.wf.nBlockAlign = (char)(_data->GetChannels() + _data->GetBPSample()/8);
  pcmwf.wf.nAvgBytesPerSec = _data->GetBPSecond();
  //    pcmwf.wf.nSamplesPerSec * pcmwf.wf.nBlockAlign;
  pcmwf.wBitsPerSample = (char)_data->GetBits();

  // prepare for the buffer
  ZeroMemory( &dsbd2, sizeof(DSBUFFERDESC));
  dsbd2.dwSize = sizeof(DSBUFFERDESC);
  if (_static) dsbd2.dwFlags = DSBCAPS_STATIC;
  dsbd2.dwBufferBytes = _data->GetSampleLength();
  dsbd2.lpwfxFormat = (LPWAVEFORMATEX)&pcmwf;    
  dsrval = lpds->CreateSoundBuffer( &dsbd2, &tmp, NULL );
  if (!_static && (dsrval!=DS_OK || tmp==NULL)) return NULL;
	if (dsrval!=DS_OK || tmp==NULL)
  {
    // retry without static buffer
    ZeroMemory( &dsbd2, sizeof(DSBUFFERDESC));
    dsbd2.dwSize = sizeof(DSBUFFERDESC);
    dsbd2.dwBufferBytes = _data->GetSampleLength();
    dsbd2.lpwfxFormat = (LPWAVEFORMATEX)&pcmwf;    
    dsrval = lpds->CreateSoundBuffer( &dsbd2, &tmp, NULL );
    if (dsrval!=DS_OK || tmp==NULL) return NULL;
  }

  // now we have the buffer
  dsrval=tmp->Lock(0, dsbd2.dwBufferBytes, &p1, &len1, &p2, &len2, 0);
	if (dsrval==DSERR_BUFFERLOST) {
		tmp->Restore();
	  dsrval=tmp->Lock(0, dsbd2.dwBufferBytes, &p1, &len1, &p2, &len2, 0);
	}
  if (dsrval!=DS_OK) return NULL;
  if (len1<dsbd2.dwBufferBytes) return NULL;
  memcpy(p1, _data->GetBuffer(), len1);
  dsrval=tmp->Unlock(p1, len1, p2, len2);
  return tmp;
}


/*
*/
int WinDXAudio::ReleaseSB(int _handle) {
	if (_handle>m_listbuffer.size()) return -1;

  m_listbuffer[_handle]->Release();
  //m_listbuffer.erase(_handle);
	return 0;
}

/*
int c_dsound::SetVolume(int handle, long int volume) {
	if ((handle>sbuffers) || (lpdsbSec[handle]==NULL)) return -1;
	return DS_Err(lpdsbSec[handle]->SetVolume(volume));
}

int c_dsound::GetVolume(int handle, long int *volume) {
	if ((handle>sbuffers) || (lpdsbSec[handle]==NULL)) return -1;
	return DS_Err(lpdsbSec[handle]->GetVolume(volume));
}

int c_dsound::SetPan(int handle, long int pan) {
	if ((handle>sbuffers) || (lpdsbSec[handle]==NULL)) return -1;
	return DS_Err(lpdsbSec[handle]->SetPan(pan));
}

int c_dsound::GetPan(int handle, long int *pan) {
	if ((handle>sbuffers) || (lpdsbSec[handle]==NULL)) return -1;
	return DS_Err(lpdsbSec[handle]->GetPan(pan));
}
*/

#endif
