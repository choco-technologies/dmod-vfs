#ifndef DMOD_MOD_DEFS_H_dmfsi
#define DMOD_MOD_DEFS_H_dmfsi

#include "dmod_defs.h"


#ifdef DMOD_dmfsi
#  define dmod_dmfsi_api_to_mal(MODULE,NAME)                            \
            DMOD_API_TO_MAL(dmfsi, MODULE, NAME)
#  define dmod_dmfsi_api_to_mal_ex(NAME_IN, MODULE_MAL, NAME_MAL)       \
            DMOD_API_TO_MAL_EX(dmfsi, MODULE_IN, NAME_IN, MODULE_MAL, NAME_MAL)
#  define dmod_dmfsi_api(VERSION, RET, NAME, PARAMS)                    \
            DMOD_INPUT_API(dmfsi, VERSION, RET, NAME, PARAMS)
#  define dmod_dmfsi_global_api(VERSION, RET, NAME, PARAMS)             \
            DMOD_GLOBAL_INPUT_API(dmfsi, VERSION, RET, NAME, PARAMS)
#  define dmod_dmfsi_mal(VERSION, RET, NAME, PARAMS)                    \
            DMOD_MAL_OUTPUT_API(dmfsi , VERSION, RET, NAME, PARAMS)
#  define dmod_dmfsi_global_mal(VERSION, RET, NAME, PARAMS)             \
            DMOD_GLOBAL_MAL_OUTPUT_API(dmfsi, VERSION, RET, NAME, PARAMS)
#  define dmod_dmfsi_api_declaration(VERSION, RET, NAME, PARAMS)        \
            DMOD_INPUT_API_DECLARATION(dmfsi, VERSION, RET, NAME, PARAMS)
#  define dmod_dmfsi_dif(VERSION, RET, NAME, PARAMS)                    \
    typedef RET (*dmod_dmfsi##NAME##_t) PARAMS; \
    extern const char* const DMOD_MAKE_DIF_SIG_NAME(dmfsi, NAME);
#  define dmod_dmfsi_dif_api_declaration(VERSION, IMPL_MODULE, RET, NAME, PARAMS)  \
            DMOD_DIF_API_DECLARATION(dmfsi, IMPL_MODULE, VERSION, RET, NAME, PARAMS)
#   define DMOD_MODULE_NAME        "dmfsi"
#   define DMOD_MODULE_VERSION     "1.0"
#   define DMOD_AUTHOR_NAME        "DMOD DMFSI Team"
#   define DMOD_STACK_SIZE         1024
#   define DMOD_PRIORITY           1
#   define DMOD_MODULE_TYPE        Dmod_ModuleType_Library
#   define DMOD_MANUAL_LOAD        OFF
#else
#  ifdef DMOD_MAL_dmfsi
#  define dmod_dmfsi_mal(VERSION, RET, NAME, PARAMS)            \
                DMOD_MAL_INPUT_API(dmfsi, VERSION, RET, NAME, PARAMS)
#  define dmod_dmfsi_global_mal(VERSION, RET, NAME, PARAMS)     \
                DMOD_GLOBAL_MAL_INPUT_API(dmfsi, VERSION, RET, NAME, PARAMS)
#else 
#  define dmod_dmfsi_mal(VERSION, RET, NAME, PARAMS)            \
                DMOD_MAL_OUTPUT_API(dmfsi , VERSION, RET, NAME, PARAMS)
#  define dmod_dmfsi_global_mal(VERSION, RET, NAME, PARAMS)     \
                DMOD_GLOBAL_MAL_OUTPUT_API(dmfsi, VERSION, RET, NAME, PARAMS)
#endif
#  define dmod_dmfsi_api(VERSION, RET, NAME, PARAMS)            \
                DMOD_OUTPUT_API(dmfsi, VERSION, RET, NAME, PARAMS)
#  define dmod_dmfsi_global_api(VERSION, RET, NAME, PARAMS)     \
                DMOD_GLOBAL_OUTPUT_API(dmfsi, VERSION, RET, NAME, PARAMS)
# ifdef ENABLE_DIF_REGISTRATIONS
#  define dmod_dmfsi_dif(VERSION, RET, NAME, PARAMS)            \
                typedef RET (*dmod_dmfsi##NAME##_t) PARAMS; \
                const char* const DMOD_MAKE_DIF_SIG_NAME(dmfsi, NAME) = DMOD_MAKE_DIF_SIGNATURE(dmfsi, VERSION, NAME);
#  else
#  define dmod_dmfsi_dif(VERSION, RET, NAME, PARAMS)            \
                typedef RET (*dmod_dmfsi##NAME##_t) PARAMS; \
                _DMOD_DIF_SIGNATURE_REGISTRATION(DMOD_MAKE_DIF_SIG_NAME(dmfsi, NAME), DMOD_MAKE_DIF_SIGNATURE(dmfsi, VERSION, NAME))
#  endif
#  define dmod_dmfsi_dif_api_declaration(VERSION, IMPL_MODULE, RET, NAME, PARAMS)  \
                RET DMOD_MAKE_DIF_API_FUNCTION_NAME(dmfsi, IMPL_MODULE, NAME) PARAMS; \
                _DMOD_DIF_API_REGISTRATION(dmfsi, IMPL_MODULE, VERSION, NAME) \
                RET DMOD_MAKE_DIF_API_FUNCTION_NAME(dmfsi, IMPL_MODULE, NAME) PARAMS
#endif

#endif // DMOD_MOD_DEFS_H_dmfsi
