#ifndef __HASH_MAP_H__
#define __HASH_MAP_H__

#ifdef __GNUC__
  #if __GNUC__ == 3
    #include <ext/hash_map>
    #if __GNUC_MINOR__ > 0
	#define HASH_MAP __gnu_cxx::hash_map
        #define HASH __gnu_cxx::hash
    #else
	#define HASH_MAP std::hash_map
	#define HASH std::hash
    #endif
  #else
    #include <hash_map>
    #define HASH_MAP std::hash_map
    #define HASH std::hash
  #endif
#else
  #ifdef _MSC_VER 
   #if (_MSC_VER <= 1200) && defined(_STLP_MSVC)
    #include <hash_map>
    #define HASH_MAP std::hash_map
    #define HASH std::hash
  #elif (_MSC_VER == 1300) 
    #include <hash_map>
    #define HASH_MAP std::hash_map
    #define HASH std::hash_compare
  #endif
  #else
    #error "PLEASE PROVIDE CORRECT #INCLUDE<HASH_MAP> STATEMENT FOR YOUR PLATFORM!"
  #endif
#endif

template <class key, class val, class hash, class eq> struct HASH_MAPS {
#if defined(_MSC_VER) && (_MSC_VER == 1300)
	typedef HASH_MAP<key, val, eq> Type;
#else
	typedef HASH_MAP<key, val, hash, eq> Type;
#endif
};

#endif // __HASH_MAP_H__
