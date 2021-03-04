#ifndef ONYX_MACROS_HH
#define ONYX_MACROS_HH

#define CONCATENATE_DIRECT(s1,s2) s1##s2
#define CONCATENATE(s1,s2) CONCATENATE_DIRECT(s1,s2)
#define STRINGIZE_HELPER(exp) #exp
#define STRINGIZE(exp) STRINGIZE_HELPER(exp)
#define ANONYMOUS_VAR(str) CONCATENATE(str, __LINE__)

#endif
