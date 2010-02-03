/* iffmsgs.c --- The IFF error msgs indexed by iffp
 * Use:  extern char *IFFPMessages[];  in application to access
 */

#ifndef IFF_H
#include "iff/iff.h"
#endif

/* Message strings for IFFP codes. */
char MsgOkay[]        = {"(IFF_OKAY) No FORM of correct type in file." };
char MsgEndMark[]     = {"(END_MARK) How did you get this message?" };
char MsgDone[]        = {"(IFF_DONE) All done."};
char MsgDos[]         = {"(DOS_ERROR) The DOS returned an error." };
char MsgNot[]         = {"(NOT_IFF) Not an IFF file." };
char MsgNoFile[]      = {"(NO_FILE) No such file found." };
char MsgClientError[] = {"(CLIENT_ERROR) Probably insufficient RAM."};
char MsgForm[]        = {"(BAD_FORM) File contains a malformed FORM." };
char MsgShort[]       = {"(SHORT_CHUNK) File contains a short Chunk." };
char MsgBad[]         = {"(BAD_IFF) A mangled IFF file." };

/* THESE MUST APPEAR IN RIGHT ORDER!! */
char *IFFPMessages[-LAST_ERROR+1] = {
    /*IFF_OKAY*/  MsgOkay,
    /*END_MARK*/  MsgEndMark,
    /*IFF_DONE*/  MsgDone,
    /*DOS_ERROR*/ MsgDos,
    /*NOT_IFF*/   MsgNot,
    /*NO_FILE*/   MsgNoFile,
    /*CLIENT_ERROR*/ MsgClientError,
    /*BAD_FORM*/  MsgForm,
    /*SHORT_CHUNK*/  MsgShort,
    /*BAD_IFF*/   MsgBad
    };

