#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "AudioData.h"

AudioData::AudioData()
{
	m_data=NULL;
  m_name=NULL;
	m_samplelen=0;
  m_samplerate=0;
  m_bpsecond=0;
  m_bpsample=0;
  m_channels=0;
  m_soundbuffer=-1;
  m_playing=0;
  m_bits=0;
}

AudioData::~AudioData()
{
	Reset();
}

int AudioData::Reset()
{
	if (m_data) free(m_data);
	m_data=NULL;
	if (m_name) free(m_name);
	m_name=NULL;
	m_samplelen=0;
  m_samplerate=0;
  m_bpsecond=0;
  m_bpsample=0;
  m_channels=0;
  m_bits=0;
  m_soundbuffer=-1;
  m_playing=0;
	return 0;
}


int AudioData::Load(char *_filename)
{
	FILE     *fp;
    char      buf[10];
	long int  flen1, flen2, nextpos;

	Reset();
	fp=fopen(_filename, "rb");
	if (fp==NULL) return -1;

    fseek(fp,0,2);
    flen1=ftell(fp);
    fseek(fp,0,0);
    fread(buf, 1, 4, fp);
    if (memcmp(buf, "RIFF", 4)!=0) {fclose(fp); return -1;}
    fread(&flen2, 1, 4, fp); //length of the file after this chunk
    if (flen1!=flen2+8) {
        //LENGTH - MISMATCH !!!
    }
    fread(buf, 1, 4, fp);
    if (memcmp(buf, "WAVE", 4)!=0) {fclose(fp); return -1;}

    while (flen1-ftell(fp)>1) {
        fread(buf, 1, 4, fp); // get chunck-id
        fread(&flen2, 1, 4, fp); //get chunk-length
        //if (DEBUG) printf("Chunklen : %i -> ", flen2);
		// now we identify the chunck and start proper actions
        if (!memcmp(buf, "fmt ", 4)) {
			// FORMAT-Chunck
            nextpos=ftell(fp)+flen2;
            //if (DEBUG) printf("Format-Chunk\n");
            fread(buf, 1, 2, fp);
            if (buf[0]!=1) {fclose(fp); return -1;} //WRONG CODING !
            fread(buf, 1, 2, fp);
            m_channels=buf[0];
            //if (DEBUG) printf("Channels  : %i\n", buf[0]);
            fread(&flen2, 1, 4, fp);
            m_samplerate=flen2;
            //if (DEBUG) printf("SamplRate: %i\n", flen2);
            fread(&flen2, 1, 4, fp);
            m_bpsecond=flen2;
            //if (DEBUG) printf("bpsec    : %i\n", flen2);
            fread(buf, 1, 2, fp);
            m_bpsample=buf[0];
            //if (DEBUG) printf("bpsamp   : %i\n", buf[0]);
            fread(buf, 1, 2, fp);
            m_bits=buf[0];
            //if (DEBUG) printf("sample   : %i bit\n", buf[0]);
            fseek(fp,nextpos,0);
        } else if (!memcmp(buf, "data", 4)) {
			// DATA-Chunck
            //if (DEBUG) printf("Data-Chunk\n");
            m_data=(char *)malloc(flen2);
            if (m_data==NULL) {fclose(fp); return -1;} // MALLOC-Error
            m_samplelen=flen2;
            fread(m_data, 1, flen2, fp);
        } else {
            //if (DEBUG) printf("'%c%c%c%c' - unknown chunk\n", buf[0], buf[1], buf[2], buf[3]);
            fclose(fp);
            return -1;
        }
    }

	fclose(fp);
	return 0;
}