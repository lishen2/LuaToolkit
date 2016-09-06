#ifndef _COMMON_H_
#define _COMMON_H_

/* ������ */
#define ERR_OK        0
#define ERR_FAILED    1
#define ERR_TIMEOUT   2
#define ERR_VERIFY    3  /* У����� */
#define ERR_FORMAT    4  /* ��Ϣ��ʽ���� */

#define BOOL_TRUE     1
#define BOOL_FALSE    0

#define INVALID_LENGTH  -1

#ifdef WIN32 
    #define INLINE __inline
#else 
    #define INLINE inline
#endif

#ifdef WIN32
	#define vsnprintf _vsnprintf
	#define snprintf _snprintf
#endif

#ifndef NULL
    #define NULL 0
#endif

#ifdef _DEBUG
    #define ASSERT(exp, msg) if (!(exp)) {printf(msg); while(1);}
#else
    #define ASSERT(exp, msg)
#endif

#endif

