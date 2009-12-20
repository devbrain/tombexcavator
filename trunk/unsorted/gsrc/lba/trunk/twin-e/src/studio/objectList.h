#ifdef LBASTUDIO

#ifndef _OBJECT_LIST_H_
#define _OBJECT_LIST_H_

#include "wx/dialog.h"
#include "wx/button.h"

class objectListWindow : public wxDialog
{
private:
  wxWindow* mParent;

  wxButton *m_btnFocused;
  wxButton *m_btnDelete;

  DECLARE_EVENT_TABLE()

public:
    objectListWindow( wxWindow* parent, wxWindowID id );
};

#endif

#endif