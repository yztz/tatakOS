#ifdef profile
#undef profile
#endif

#ifdef profile_void
#undef profile_void
#endif

#ifdef PROFILE_PER_CALL
#undef PROFILE_PER_CALL
#endif

#define profile(func, ...) func(__VA_ARGS__)
#define profile_void(func, ...) func(__VA_ARGS__)

#ifdef DEBUG
#ifndef PROFILE_DEF
#define PROFILE_DEF
#endif
#include "generated/profile_gen.h"
#endif
