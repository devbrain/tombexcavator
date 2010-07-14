#ifdef LBASTUDIO

extern "C" {
#include "../lba.h"
}

#include "objectList.h"

#include "wx/colordlg.h"
#include "wx/filedlg.h"
#include "wx/dirdlg.h"
#include "wx/fontdlg.h"
#include "wx/choicdlg.h"
#include "wx/tipdlg.h"
#include "wx/progdlg.h"
#include "wx/fdrepdlg.h"
#include "wx/busyinfo.h"

#include "wx/listctrl.h"

BEGIN_EVENT_TABLE(objectListWindow, wxDialog)
END_EVENT_TABLE()

objectListWindow::objectListWindow( wxWindow* parent, wxWindowID id ) : wxDialog(parent, id, "Object List", wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE)
{
  int i;
  wxBoxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);

  wxButton *btnOk = new wxButton(this, wxID_CANCEL, _T("&Close"));
  wxListView *list = new wxListView(this, -1, wxDefaultPosition, wxSize(200,400), wxLC_VRULES|wxLC_EDIT_LABELS|wxLC_LIST   );

  for(i=0;i<numActorInRoom;i++)
  {
    list->InsertItem(i,actors[i].name);
  }

  sizerTop->Add(list, 0, wxALIGN_CENTER | wxALL, 5);
  sizerTop->Add(btnOk, 0, wxALIGN_CENTER | wxALL, 5);

  SetAutoLayout(TRUE);
  SetSizer(sizerTop);

  sizerTop->SetSizeHints(this);
  sizerTop->Fit(this);

  btnOk->SetFocus();
  btnOk->SetDefault();
}

#endif