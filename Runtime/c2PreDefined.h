#ifndef C2ENGINE_PREDEFINED_H_
#define C2ENGINE_PREDEFINED_H_
////////////////////////////////////////////////////////////////////////////////

//TODO
#define C2API

////////////////////////////////////////////////////////////////////////////////
//TODO: 明确长度的数字类型应该仅仅用在本处，所以最好后面把名字修改一下。其他请见README.md
#ifdef _MSC_VER
using c2BufLen = unsigned __int64;//32位和64下size_t并不明确，故定义个明确大小的缓冲消息长度值
typedef __int8 Sint8;
typedef unsigned __int8 Uint8;
typedef __int16 Sint16;
typedef unsigned __int16 Uint16;
typedef __int32 Sint32;
typedef unsigned __int32 Uint32;
typedef __int64 Sint64;
typedef unsigned __int64 Uint64;
#else
using c2BufLen64 = unsigned long long;//32位和64下size_t并不明确，故定义个明确大小的消息长度值
typedef signed char Sint8;
typedef unsigned char Uint8;
typedef signed short Sint16;
typedef unsigned short Uint16;
typedef signed int Sint32;
typedef unsigned int Uint32;
typedef signed long long Sint64;
typedef unsigned long long Uint64;
#endif

////////////////////////////////////////////////////////////////////////////////
#endif// C2ENGINE_PREDEFINED_H_
