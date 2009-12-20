#ifdef LBASTUDIO

extern "C" {
#include "../lba.h"
}

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

// for all others, include the necessary headers
#ifndef WX_PRECOMP
  #include "wx/app.h"
  #include "wx/log.h"
  #include "wx/frame.h"
  #include "wx/panel.h"
  #include "wx/stattext.h"
  #include "wx/menu.h"
  #include "wx/layout.h"
  #include "wx/msgdlg.h"
  #include "wx/thread.h"
  #include "wx/button.h"
  #include "wx/window.h"
  #include "wx/event.h"
#endif

#include "objectList.h"

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_GAMETICK, 7777)
END_DECLARE_EVENT_TYPES()

DEFINE_EVENT_TYPE(wxEVT_GAMETICK)

// it may also be convenient to define an event table macro for this event type
#define EVT_GAMETICK(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_GAMETICK, id, -1, \
        (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)&fn, \
        (wxObject *) NULL \
    ),

class MyFrame;
// ----------------------------------------------------------------------------
// GameTimerThread
// ----------------------------------------------------------------------------

class GameTimerThread : public wxThread
{
public:
    GameTimerThread(MyFrame *frame);

    // thread execution starts here
    virtual void *Entry();

    // called when the thread exits - whether it terminates normally or is
    // stopped with Delete() (but not when it is Kill()ed!)
    virtual void OnExit();

    // write something to the text control
    void WriteText(const wxString& text);

public:
    size_t   m_count;
    MyFrame *m_frame;
};

GameTimerThread::GameTimerThread(MyFrame *frame) : wxThread()
{
    m_count = 0;
    m_frame = frame;
}

void GameTimerThread::WriteText(const wxString& text)
{
    wxString msg;

    // before doing any GUI calls we must ensure that this thread is the only
    // one doing it!

    wxMutexGuiEnter();

   
    wxMutexGuiLeave();
}

void GameTimerThread::OnExit()
{/*
    wxCriticalSectionLocker locker(wxGetApp().m_critsect);

    wxArrayThread& threads = wxGetApp().m_threads;
    threads.Remove(this);

    if ( threads.IsEmpty() )
    {
        // signal the main thread that there are no more threads left if it is
        // waiting for us
        if ( wxGetApp().m_waitingUntilAllDone )
        {
            wxGetApp().m_waitingUntilAllDone = FALSE;

            wxMutexLocker lock(wxGetApp().m_mutexAllDone);
            wxGetApp().m_condAllDone.Signal();
        }
    }*/
}

void *GameTimerThread::Entry()
{
    wxString text;

    text.Printf(wxT("Thread 0x%x started (priority = %u).\n"), GetId(), GetPriority());
    WriteText(text);
    // wxLogMessage(text); -- test wxLog thread safeness

    while(1)
    {
        // check if we were asked to exit
        if ( TestDestroy() )
            break;
/*
        lba_time++;

        mainLoopInteration();*/
        wxCommandEvent eventCustom(wxEVT_GAMETICK);

       
        wxPostEvent((wxEvtHandler*)m_frame, eventCustom);

        // wxSleep() can't be called from non-GUI thread!
        wxThread::Sleep(10);
    }

    text.Printf(wxT("Thread 0x%x finished.\n"), GetId());
    WriteText(text);
    // wxLogMessage(text); -- test wxLog thread safeness

    return NULL;
}

// ----------------------------------------------------------------------------
// Button_objSelect
// ----------------------------------------------------------------------------

class Button_objSelect : public wxButton
{
public:
    Button_objSelect(wxWindow *parent,
             wxWindowID id,
             const wxString& label = wxEmptyString,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize)
        : wxButton(parent, id, label, pos, size)
    {
    }

    void OnDClick(wxMouseEvent& event)
    {
        wxLogMessage(_T("MyButton::OnDClick"));

        event.Skip();
    }

private:
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(Button_objSelect, wxButton)
    EVT_LEFT_DOWN(Button_objSelect::OnDClick)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// Button_listObjects
// ----------------------------------------------------------------------------

class Button_listObjects : public wxButton
{
private:
  wxWindow* mParent;

public:
    Button_listObjects(wxWindow *parent,
             wxWindowID id,
             const wxString& label = wxEmptyString,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize)
        : wxButton(parent, id, label, pos, size)
    {
      mParent = parent;
    }

    void OnDClick(wxMouseEvent& event)
    {
        objectListWindow* objectList = new objectListWindow(mParent,-1);

        objectList->ShowModal();

      //  event.Skip();
    }

private:
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(Button_listObjects, wxButton)
    EVT_LEFT_DOWN(Button_listObjects::OnDClick)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// Button_heroBrick
// ----------------------------------------------------------------------------

class Button_heroBrick : public wxButton
{
public:
    Button_heroBrick(wxWindow *parent,
             wxWindowID id,
             const wxString& label = wxEmptyString,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize)
        : wxButton(parent, id, label, pos, size)
    {
    }

    void OnDClick(wxMouseEvent& event)
    {
        wxLogMessage(_T("MyButton::OnDClick"));

        event.Skip();
    }

private:
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(Button_heroBrick, wxButton)
    EVT_LEFT_DOWN(Button_heroBrick::OnDClick)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// Button_cubeClip
// ----------------------------------------------------------------------------

class Button_cubeClip : public wxButton
{
public:
    Button_cubeClip(wxWindow *parent,
             wxWindowID id,
             const wxString& label = wxEmptyString,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize)
        : wxButton(parent, id, label, pos, size)
    {
    }

    void OnDClick(wxMouseEvent& event)
    {
        wxLogMessage(_T("MyButton::OnDClick"));

        event.Skip();
    }

private:
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(Button_cubeClip, wxButton)
    EVT_LEFT_DOWN(Button_cubeClip::OnDClick)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// Button_restartCube
// ----------------------------------------------------------------------------

class Button_restartCube : public wxButton
{
public:
    Button_restartCube(wxWindow *parent,
             wxWindowID id,
             const wxString& label = wxEmptyString,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize)
        : wxButton(parent, id, label, pos, size)
    {
    }

    void OnDClick(wxMouseEvent& event)
    {
        wxLogMessage(_T("MyButton::OnDClick"));

        event.Skip();
    }

private:
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(Button_restartCube, wxButton)
    EVT_LEFT_DOWN(Button_restartCube::OnDClick)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
};

class MyPanel : public wxPanel
{
public:
    MyPanel(wxFrame *frame);

    // turn on/off the specified style bit on the calendar control
    void ToggleCalStyle(bool on, int style);

    void HighlightSpecial(bool on);

    void SetDate();
    void Today();
    
private:
    DECLARE_EVENT_TABLE()
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void GameTick(wxCommandEvent& event);

private:
    MyPanel *m_panel;

    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Calendar_File_About = 100,
    Calendar_File_Quit,
    Calendar_File_StartGame,


};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Calendar_File_Quit,  MyFrame::OnQuit)
    EVT_MENU(Calendar_File_About, MyFrame::OnAbout)

    EVT_GAMETICK(-1, MyFrame::GameTick)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyPanel, wxPanel)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWindows to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// `Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // Create the main application window
    MyFrame *frame = new MyFrame(_T("Twin-e Studio"),  wxPoint(50, 50), wxSize(450, 340));

    frame->Show(TRUE);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned FALSE here, the
    // application would exit immediately.
    return TRUE;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
    // create a menu bar
    wxMenu *menuFile = new wxMenu;

    menuFile->Append(Calendar_File_StartGame, _T("&Start Engine\tCtrl-A"), _T("Start Engine"));
    menuFile->Append(Calendar_File_About, _T("&About...\tCtrl-A"), _T("Show about dialog"));
    menuFile->AppendSeparator();
    menuFile->Append(Calendar_File_Quit, _T("E&xit\tAlt-X"), _T("Quit Twin-e Studio"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, _T("&File"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    m_panel = new MyPanel(this);

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText(_T("Welcome to Twin-e Studio!"));
#endif // wxUSE_STATUSBAR

    lbaMain(0,NULL);
    mainLoopInteration();

    GameTimerThread* pThread = new GameTimerThread(this);

    pThread->Create();
    pThread->Run(); 
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_T("Twin-e Studio\n© 2002-2004 Vincent Hamm"),
                 _T("About Twin-e Studio"), wxOK | wxICON_INFORMATION, this);
}

void MyFrame::GameTick(wxCommandEvent& event)
{
  lba_time++;
  mainLoopInteration();
}

// ----------------------------------------------------------------------------
// MyPanel
// ----------------------------------------------------------------------------

MyPanel::MyPanel(wxFrame *frame)
       : wxPanel(frame, -1)
{
  wxButton* pButton;
  int sizeX;
  int sizeY;
  int yPos = 0;

  //SetAutoLayout(TRUE);

  pButton = new Button_objSelect  ( this, -1, _T("Obj Select"),   wxPoint(0,yPos), wxSize(80,-1) );
  pButton->GetSize(&sizeX,&sizeY);
  yPos += sizeY;

  yPos += 10;

  pButton = new  Button_listObjects( this, -1, _T("Liste Objets"), wxPoint(0,yPos), wxSize(80,-1) );
  pButton->GetSize(&sizeX,&sizeY);
  yPos += sizeY;

  pButton = new  Button_heroBrick  ( this, -1, _T("Hero Brick"),   wxPoint(0,yPos), wxSize(80,-1) );
  pButton->GetSize(&sizeX,&sizeY);
  yPos += sizeY;

  pButton = new  Button_cubeClip  ( this, -1, _T("Cube clip"),   wxPoint(0,yPos), wxSize(80,-1) );
  pButton->GetSize(&sizeX,&sizeY);
  yPos += sizeY;

  pButton = new  Button_restartCube( this, -1, _T("Restart cube"), wxPoint(0,yPos), wxSize(80,-1) );
  pButton->GetSize(&sizeX,&sizeY);
  yPos += sizeY;

}

#endif
