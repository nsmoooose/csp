#ifndef StandardBufferTag

  #define StandardBufferTag

  class StandardBuffer
  {
	  
  public:

	  virtual short CreateBuffer(unsigned long Length) = 0;
	  virtual short GetDataPointer(void **) = 0;
	  virtual short GetSize(unsigned long *size) = 0;
	  virtual short Lock() = 0;
	  virtual short Unlock() = 0;
	  virtual short DestroyBuffer() = 0;

  };

#endif