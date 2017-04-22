#include <windows.h>
#include "resource.h"

/* Window Name. */
const char winClassName[] = "controlPanel";

/* Function Prototypes. */
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
  LPSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void DoFileOpen (HWND hwnd);
void DoFileSave (HWND hwnd);
void DoHelpAbout (HWND hwnd);

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
  LPSTR lpCmdLine, int nCmdShow)
{
  /* Setup Window Functionals. */
  WNDCLASSEX wc;
  HWND hwnd;
  MSG msg;

  /* Assign defaults. */
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.style = 0;
  wc.lpfnWndProc = WndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = hInstance;
  wc.hIcon  = LoadIcon (GetModuleHandle (NULL), MAKEINTRESOURCE (ID_NOSLEEP_ICON));
  wc.hIconSm  = (HICON) LoadImage (GetModuleHandle (NULL), MAKEINTRESOURCE (ID_NOSLEEP_ICON), IMAGE_ICON, 16, 16, 0);
  wc.hCursor = LoadCursor (NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);
  wc.lpszMenuName = MAKEINTRESOURCE (ID_NOSLEEP_MENU);
  wc.lpszClassName = winClassName;
  wc.hIconSm = LoadIcon (NULL, IDI_APPLICATION);

  /* Register new window. */
  if (!RegisterClassEx (&wc))
  {
    MessageBox (NULL, "Window Registration Failed!", "Error!",
      MB_ICONEXCLAMATION | MB_OK);
    return 0;
  }

  /* Create new Window. */
  hwnd = CreateWindowEx(
    0,
    winClassName,
    "#NoSleep: Control Window",
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT, 640, 480,
    NULL, NULL, hInstance, NULL);

  if (hwnd == NULL)
  {
    MessageBox(NULL, "Window Creation Failed!", "Error!",
      MB_ICONEXCLAMATION | MB_OK);
    return 0;
  }

  /* Display and updated window. */
  ShowWindow (hwnd, nCmdShow);
  UpdateWindow (hwnd);

  /* Loop over user interaction and notifications. */
  while (GetMessage (&msg, NULL, 0, 0) > 0)
  {
    TranslateMessage (&msg);
    DispatchMessage (&msg);
  }

  /* Return exit status. */
  return msg.wParam;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
  {
    case WM_CREATE:
    {
      HFONT hfDefault;
      HWND hEdit;

      hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", 
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL, 
        0, 0, 100, 100, hwnd, (HMENU) ID_NOSLEEP_MAIN, GetModuleHandle (NULL), NULL);
      if (hEdit == NULL)
        MessageBox (hwnd, "Could not create edit box.", "Error", MB_OK | MB_ICONERROR);

      hfDefault = (HFONT) GetStockObject (DEFAULT_GUI_FONT);
      SendMessage (hEdit, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));
    }
    break;
    case WM_CLOSE:
      DestroyWindow (hwnd);
    break;
    case WM_DESTROY:
      PostQuitMessage (0);
    break;
    case WM_COMMAND:
      switch (LOWORD (wParam))
      {
        case ID_FILE_EXIT:
          PostMessage (hwnd, WM_CLOSE, 0, 0);
        break;
        case ID_FILE_NEW:
          SetDlgItemText (hwnd, ID_NOSLEEP_MAIN, "");
        break;
        case ID_FILE_OPEN:
          DoFileOpen (hwnd);
        break;
        case ID_FILE_SAVEAS:
          DoFileSave (hwnd);
        break;
        case ID_HELP_ABOUT:
          DoHelpAbout (hwnd);
        break;
      }
      break;
    default:
      return DefWindowProc (hwnd, msg, wParam, lParam);
  }

  return 0;
}

void DoFileOpen (HWND hwnd)
{
  OPENFILENAME ofn;
  char szFileName[MAX_PATH] = "";

  ZeroMemory(&ofn, sizeof(ofn));

  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = hwnd;
  ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
  ofn.lpstrFile = szFileName;
  ofn.nMaxFile = MAX_PATH;
  ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
  ofn.lpstrDefExt = "txt";

  if(GetOpenFileName (&ofn))
  {
    HWND hEdit = GetDlgItem (hwnd, ID_NOSLEEP_MAIN);
    // LoadTextFileToEdit (hEdit, szFileName);
  }
}

void DoFileSave (HWND hwnd)
{
  OPENFILENAME ofn;
  char szFileName[MAX_PATH] = "";

  ZeroMemory(&ofn, sizeof(ofn));

  ofn.lStructSize = sizeof (OPENFILENAME);
  ofn.hwndOwner = hwnd;
  ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
  ofn.lpstrFile = szFileName;
  ofn.nMaxFile = MAX_PATH;
  ofn.lpstrDefExt = "txt";
  ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

  if (GetSaveFileName (&ofn))
  {
    HWND hEdit = GetDlgItem (hwnd, ID_NOSLEEP_MAIN);
    // SaveTextFileFromEdit (hEdit, szFileName);
  }
}

void DoHelpAbout (HWND hwnd)
{
  ;
}