%module HashUtility
%{
#include "SimData/HashUtility.h"
%}

#include "SimData/ns-simdata.h"

typedef  unsigned long  int  u4;   /* unsigned 4-byte type */
typedef  unsigned      char  u1;   /* unsigned 1-byte type */

%rename(hash_string) SIMDATA(newhasht_cstring)(const char*);

NAMESPACE_SIMDATA

struct HashT {
	HashT();
	HashT(guint32 x);
	HashT(guint32 b_, guint32 a_);
	HashT(const HashT &x);
	bool operator ==(guint32 x) const;
	bool operator !=(guint32 x) const;
	bool operator ==(HashT const &x) const;
	bool operator !=(HashT const &x) const;
};

typedef HashT hasht;

HashT newhasht_cstring(const char*);

NAMESPACE_END

