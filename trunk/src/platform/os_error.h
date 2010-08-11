#ifndef __PLATFORM_OS_ERROR_H__
#define __PLATFORM_OS_ERROR_H__

enum os_error_id_t_
  {
    eNO_ERROR,
    
    eACCESS_DENIED,
    ePATHNAME_EXISTS,
    ePATHNAME_IS_DIR,
    ePATHNAME_NOT_DIR,
    eNO_DESCRIPTORS,
    eFILE_NOT_FOUND,
    eNO_SPACE_LEFT,

    eUNKNOWN_ERROR
  };

typedef enum os_error_id_t_ os_error_id_t;

struct os_error_s_;

typedef struct os_error_s_ os_error_s;

extern os_error_id_t get_os_error_id     (const os_error_s* os_error);
extern const char*   get_os_error_string (const os_error_s* os_error);

extern void free_os_error (os_error_s* os_error);
extern os_error_s* create_os_error_from_last_error (void);

#endif
