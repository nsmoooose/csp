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
    #include <hash_map>
    #define HASH_MAP std::hash_map
    #define HASH std::hash
  #else
    #error "PLEASE PROVIDE CORRECT #INCLUDE<HASH_MAP> STATEMENT FOR YOUR PLATFORM!"
  #endif
#endif

