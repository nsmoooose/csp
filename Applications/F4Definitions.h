/////////////////////////////////////////////////////////
//	
//	Some definitions we need for the map reader
//
//	they should be removed from the code ASAP...
//
/////////////////////////////////////////////////////////

#if !defined F4DEFINITIONS_H
#define F4DEFINITIONS_H

#define POST_OFFSET_BITS			4
#define POST_OFFSET_MASK			(~(((~1)>>POST_OFFSET_BITS)<<POST_OFFSET_BITS))
#define POSTS_ACROSS_BLOCK			(1<<POST_OFFSET_BITS)
#define POSTS_PER_BLOCK				(1<<(POST_OFFSET_BITS<<1))

#define WORLD_TO_FLOAT_GLOBAL_POST( distance )	((distance)/m_pF4Map->GetQuadWidthFeet())
#define WORLD_TO_GLOBAL_POST( distance )		(FloatToInt32((float)floor(WORLD_TO_FLOAT_GLOBAL_POST(distance))))
#define WORLD_TO_FLOAT_LEVEL_POST( distance, LOD )	((distance) / TheMap.Level(LOD)->FTperPOST())
#define WORLD_TO_LEVEL_POST( distance, LOD )	(WORLD_TO_GLOBAL_POST( distance ) >> (LOD))
#define WORLD_TO_LEVEL_BLOCK( distance, LOD )	(WORLD_TO_GLOBAL_POST( distance ) >> ((LOD)+POST_OFFSET_BITS))

#define GLOBAL_POST_TO_WORLD( distance )		((distance)*m_pF4Map->GetQuadWidthFeet())
#define LEVEL_POST_TO_WORLD( distance, LOD )	(GLOBAL_POST_TO_WORLD( (distance)<<(LOD) ))
#define LEVEL_BLOCK_TO_WORLD( distance, LOD )	(GLOBAL_POST_TO_WORLD( (distance)<<((LOD)+POST_OFFSET_BITS) ))

#define LEVEL_POST_TO_LEVEL_BLOCK( distance )	((distance) >> POST_OFFSET_BITS)
#define LEVEL_POST_TO_BLOCK_POST( distance )	((distance) & POST_OFFSET_MASK)

#define GLOBAL_POST_TO_LEVEL_POST( distance, LOD )	((distance) >> (LOD))
#define LEVEL_POST_TO_GLOBAL_POST( distance, LOD )	((distance) << (LOD))

typedef signed char		INT8;
typedef unsigned char	UINT8;
typedef short			INT16;
typedef unsigned short	UINT16;
//typedef long			INT32;
//typedef unsigned long	UINT32;
typedef UINT8			BYTE;
typedef UINT16			WORD;
//typedef UINT32			DWORD;

#endif