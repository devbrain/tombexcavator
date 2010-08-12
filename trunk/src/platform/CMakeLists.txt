set (src 
  unicode/unicode.c
  os_error.c
  os.c
)

set (hdr 
  types.h         
  proper_export.h
  platform_export.h

  unicode/unicode.h
  unicode/unicode.hpp

  os_error.h
  config.h
  os.hpp
)

te_shared_lib (platform src hdr)
