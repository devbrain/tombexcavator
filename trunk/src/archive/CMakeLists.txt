set (src
abstract_files_visitor.cpp
abstract_archive.cpp
abstract_file.cpp
)

set (hdr
abstract_files_visitor.hpp
abstract_archive.hpp
abstract_file.hpp
archive.h
archive_export.h
)

te_shared_lib (archive src hdr)