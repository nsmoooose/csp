#include "stdio.h"
#include "WinFile.h"
#include "WinBuff.h"

int main(int argc, char* argv[])
{
  unsigned long size;
  void *data;

	StandardBuffer *buffer;
  StandardFile *file;

	buffer = new WinBuff;
  file = new WinFile;

  // ----- SAVING A FILE -----------
    
  // create a buffer and copy info into buffer
  buffer->CreateBuffer(10);           // size is 10 bytes
  buffer->Lock();                     // lock (so contents are copied to physical RAM.)
  buffer->GetDataPointer(&data);      // get the pointer to that location.
  strcpy((char *)data, "Hello!");     // copy data into that buffer
  buffer->Unlock();                   // unlocks data, the system will page it automatically.

  // save that buffer
  file->SaveFile("c:\\test.dat", buffer); // save this buffer to that filename.

  // delete that buffer
  buffer->DestroyBuffer();            // delete the buffer now that it's saved.

  // ----- LOADING A FILE ----------

  // loading a file will create the buffer
  file->LoadFile("c:\\csscomvb.txt", buffer); // load that file into this buffer.

  buffer->GetSize(&size);             // the size is (25k) (on Brandon's Machine)
  buffer->Lock();                     // lock (so contents are copied to physical RAM.)
  buffer->GetDataPointer(&data);      // get the pointer to that location.
  // look at it, do something with it, copy it, whatever.
  buffer->Unlock();                   // unlocks data, the system will page it automatically.
  buffer->DestroyBuffer();            // delete the buffer now that we are done.

	delete buffer;
  delete file;

	return 0;
}
