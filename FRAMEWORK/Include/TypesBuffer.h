#ifndef TYPESBUFFERTAG

  #define TYPESBUFFERTAG

  class StandardBuffer
  {
	  
    public:

      virtual ~StandardBuffer() {};

      // functions
	    virtual short         Initialize(unsigned long Length) = 0;
	    virtual void*         GetDataPointer() = 0;
	    virtual unsigned long GetSize() = 0;
	    virtual void          Lock() = 0;
	    virtual void          Unlock() = 0;
	    virtual void          Uninitialize() = 0;

  };

#endif
