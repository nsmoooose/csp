#ifndef __TYPESCOLOR_H__
#define __TYPESCOLOR_H__



class StandardColor
{
	public:
      unsigned char b;
      unsigned char g;
      unsigned char r;
      unsigned char a;

	  StandardColor()
	  {
		  r = 0; g = 0; b = 0; a = 0;
	  }

	  StandardColor(unsigned char _r, unsigned char _g, unsigned char _b)
	  {
		  r = _r; g = _g; b = _b; a = 255;
	  }

	  StandardColor(unsigned char _r, unsigned char _g, 
		  unsigned char _b, unsigned char _a)
	  {
		  r = _r; g = _g; b = _b; a = _a;
	  }

	  bool operator!=( StandardColor cs )
	  {
		  if( (r!=cs.r) || (g!=cs.g) || (b!=cs.b) || (a!=cs.a) )
		  {
			  return( true );
		  }
		  return( false );
	  }

	  bool operator==( StandardColor cs )
	  {
		  if ( (r == cs.r) && (g == cs.g) && (b == cs.b) && (a == cs.b))
		  {
			  return true;
		  }
		  return false;
	  }

    void Initialize(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a)
    {
		  r = _r; g = _g; b = _b; a = _a;
    }

    friend ostream & operator << (ostream & os, const StandardColor& color);

};

#endif