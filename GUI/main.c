#define WINVER 0x0A00  
#define _WIN32_WINNT 0x0A00

#include <windows.h>
#include <stdio.h>
#include <initguid.h>
#include <winsock2.h>
#include "ws2bth.h"
#include "BluetoothAPIs.h"
#include "resource.h"

/* Definitions. */
#define MAX_DEVICES     25
#define MAX_NAME_LEN    50
#define BLUE_ADDR_LEN   17
#define DEFAULT_BUFLEN  512

/* Structures. */
typedef struct BlueConfig
{
  char BD_NAME[MAX_NAME_LEN + 1];
  char BD_ADDR[BLUE_ADDR_LEN + 1];
  BOOL FULL;
} BLUECONFIG, *PBLUECONFIG;

typedef struct FlipCube
{
  BOOL rbT;
  BOOL rbR;
  BOOL rbN;
} FLIPCUBE, *PFLIPCUBE;

/* Window constants */
const char winClassName[] = "controlPanel";
PBLUECONFIG bluetooth_configs[MAX_DEVICES];
int cur_device = 0;
int update_device = -1;
int connected_device = -1;
SOCKET blue_sock = INVALID_SOCKET;
SOCKADDR_BTH blue_conn = {0};
HANDLE bluetex;
BOOL saved = TRUE;

/* Function Prototypes. */
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
  LPSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

void DoFileOpen (HWND hwnd);
void DoFileSave (HWND hwnd);
void DoHelpAbout (HWND hwnd);
DWORD WINAPI DoMainFlip (LPVOID args);

BOOL SaveConfig (HWND hEdit, LPCTSTR pszFileName);
BOOL LoadConfig (HWND hEdit, LPCTSTR pszFileName);

BOOL CALLBACK AboutDlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ConfigEditDlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ConfigCreateDlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ConfigConnectDlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

BOOL bluetooth_connect (HWND hwnd, PBLUECONFIG blue);
BOOL bluetooth_disconnect (HWND hwnd);

int WINAPI
WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
  LPSTR lpCmdLine, int nCmdShow)
{
  /* Setup Window Functionals. */
  WNDCLASSEX wc;
  HWND hwnd;
  MSG msg;

  /* Setup winsock 2.2. */
  WSADATA wsd;
  ULONG ulRetCode, CXN_SUCCESS = 0;
  ulRetCode = WSAStartup(MAKEWORD(2, 2), &wsd);
  if (CXN_SUCCESS != ulRetCode)
  {
    MessageBox (hwnd, "Unable to initialize Winsock version 2.2", "Error", MB_OK);
    return FALSE;
  }

  /* Setup mutex. */
  bluetex = CreateMutex (NULL, FALSE, NULL);
  if (bluetex == NULL)
  {
    MessageBox (NULL, "bluetex creation failed!", "Error!",
      MB_ICONEXCLAMATION | MB_OK);
    return 1;
  }

  /* Create default bluetooth device. */
  PBLUECONFIG blue;
  blue = (PBLUECONFIG) GlobalAlloc (GPTR, sizeof (BLUECONFIG));
  memcpy(blue->BD_NAME, "DEFAULT\0", MAX_NAME_LEN);
  memcpy(blue->BD_ADDR, "C8:FD:19:3E:BE:47\0", BLUE_ADDR_LEN);
  blue->FULL = TRUE;
  bluetooth_configs[cur_device] = blue;
  cur_device++;


  /* Assign window defaults. */
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
    return 1;
  }

  /* Create new Window. */
  hwnd = CreateWindowEx(
    0,
    winClassName,
    "#NoSleep: Control",
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT, 320, 260,
    NULL, NULL, hInstance, NULL);

  if (hwnd == NULL)
  {
    MessageBox(NULL, "Window Creation Failed!", "Error!",
      MB_ICONEXCLAMATION | MB_OK);
    return 1;
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

LRESULT CALLBACK 
WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
  {
    case WM_CREATE:
    {
      HFONT hfDefault;
      hfDefault = (HFONT) GetStockObject (DEFAULT_GUI_FONT);

      HWND radioButton, button, text;
      radioButton = CreateWindowEx (WS_EX_WINDOWEDGE, "BUTTON", "TOP", 
        WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP, 140, 25, 12, 12, hwnd,
        (HMENU) ID_MAIN_RTOP, GetModuleHandle (NULL), NULL);
      if (radioButton == NULL)
        MessageBox (hwnd, "Could not create top radio button.", "Error", MB_OK | MB_ICONERROR);

      text = CreateWindow ("STATIC", "TOP", 
        WS_VISIBLE | WS_CHILD | SS_CENTER, 133, 10, 30, 14, hwnd,
        NULL, GetModuleHandle (NULL), NULL);
      if (text == NULL)
        MessageBox (hwnd, "Could not create top text.", "Error", MB_OK | MB_ICONERROR);

      radioButton = CreateWindowEx (WS_EX_WINDOWEDGE, "BUTTON", "RIGHT", 
        WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, 195, 65, 12, 12, hwnd,
        (HMENU) ID_MAIN_RRIGHT, GetModuleHandle (NULL), NULL);
      if (radioButton == NULL)
        MessageBox (hwnd, "Could not create right radio button.", "Error", MB_OK | MB_ICONERROR);

      text = CreateWindow ("STATIC", "RIGHT", 
        WS_VISIBLE | WS_CHILD | SS_LEFT, 210, 63, 45, 14, hwnd,
        NULL, GetModuleHandle (NULL), NULL);
      if (text == NULL)
        MessageBox (hwnd, "Could not create right text.", "Error", MB_OK | MB_ICONERROR);

      radioButton = CreateWindowEx (WS_EX_WINDOWEDGE, "BUTTON", "BOTTOM", 
        WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, 138, 110, 12, 12, hwnd,
        (HMENU) ID_MAIN_RBOTTOM, GetModuleHandle (NULL), NULL);
      if (radioButton == NULL)
        MessageBox (hwnd, "Could not create bottom radio button.", "Error", MB_OK | MB_ICONERROR);

      text = CreateWindow ("STATIC", "BOTTOM", 
        WS_VISIBLE | WS_CHILD | SS_CENTER, 116, 130, 60, 14, hwnd,
        NULL, GetModuleHandle (NULL), NULL);
      if (text == NULL)
        MessageBox (hwnd, "Could not create bottom text.", "Error", MB_OK | MB_ICONERROR);

      radioButton = CreateWindowEx (WS_EX_WINDOWEDGE, "BUTTON", "LEFT", 
        WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, 80, 65, 12, 12, hwnd,
        (HMENU) ID_MAIN_RLEFT, GetModuleHandle (NULL), NULL);
      if (radioButton == NULL)
        MessageBox (hwnd, "Could not create left radio button.", "Error", MB_OK | MB_ICONERROR);

      text = CreateWindow ("STATIC", "LEFT", 
        WS_VISIBLE | WS_CHILD | SS_RIGHT, 35, 63, 35, 14, hwnd,
        NULL, GetModuleHandle (NULL), NULL);
      if (text == NULL)
        MessageBox (hwnd, "Could not create left text.", "Error", MB_OK | MB_ICONERROR);

      button = CreateWindowEx (WS_EX_WINDOWEDGE, "BUTTON", "FLIP!",
        WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 120, 60, 50, 25, hwnd,
        (HMENU) ID_MAIN_FLIP, GetModuleHandle (NULL), NULL);
      if (button == NULL)
        MessageBox (hwnd, "Could not create button.", "Error", MB_OK | MB_ICONERROR);

      radioButton = CreateWindowEx (WS_EX_WINDOWEDGE, "BUTTON", "45", 
        WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP, 100, 160, 12, 12, hwnd,
        (HMENU) ID_MAIN_45, GetModuleHandle (NULL), NULL);
      if (radioButton == NULL)
        MessageBox (hwnd, "Could not create 45 radio button.", "Error", MB_OK | MB_ICONERROR);

      text = CreateWindow ("STATIC", "45", 
        WS_VISIBLE | WS_CHILD | SS_LEFT, 120, 160, 20, 14, hwnd,
        NULL, GetModuleHandle (NULL), NULL);
      if (text == NULL)
        MessageBox (hwnd, "Could not create 45 text.", "Error", MB_OK | MB_ICONERROR);

      radioButton = CreateWindowEx (WS_EX_WINDOWEDGE, "BUTTON", "90", 
        WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, 150, 160, 12, 12, hwnd,
        (HMENU) ID_MAIN_90, GetModuleHandle (NULL), NULL);
      if (radioButton == NULL)
        MessageBox (hwnd, "Could not create 90 radio button.", "Error", MB_OK | MB_ICONERROR);

      text = CreateWindow ("STATIC", "90", 
        WS_VISIBLE | WS_CHILD | SS_LEFT, 170, 160, 20, 14, hwnd,
        NULL, GetModuleHandle (NULL), NULL);
      if (text == NULL)
        MessageBox (hwnd, "Could not create 90 text.", "Error", MB_OK | MB_ICONERROR);

      SendMessage (button, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));
      SendDlgItemMessage (hwnd, ID_MAIN_RTOP, BM_SETCHECK, 1, 0);
      SendDlgItemMessage (hwnd, ID_MAIN_45, BM_SETCHECK, 1, 0);
    }
    break;
    case WM_COMMAND:
    {
      switch (LOWORD (wParam))
      {
        case ID_FILE_EXIT:
          PostMessage (hwnd, WM_CLOSE, 0, 0);
        break;
        case ID_CONFIG_EDIT:
          DialogBox (GetModuleHandle (NULL),
            MAKEINTRESOURCE (ID_NOSLEEP_LIST), hwnd, ConfigEditDlgProc);
        break;
        case ID_CONFIG_SAVEAS:
          DoFileSave (hwnd);
        break;
        case ID_CONFIG_LOAD:
          DoFileOpen (hwnd);
        break;
        case ID_HELP_ABOUT:
          DoHelpAbout (hwnd);
        break;
        case ID_MAIN_FLIP:
        {
          if (blue_sock == INVALID_SOCKET)
          {
            MessageBox (hwnd, "No bluetooth connected! \n Please connect before continuing.", "Warning",
              MB_OK | MB_ICONWARNING);
            return 1;
          }
          PFLIPCUBE FC = (PFLIPCUBE) HeapAlloc (GetProcessHeap (), HEAP_ZERO_MEMORY, sizeof (FLIPCUBE)); 
          FC->rbT = SendDlgItemMessage(hwnd, ID_MAIN_RTOP, BM_GETCHECK, 0, 0);
          FC->rbR = SendDlgItemMessage(hwnd, ID_MAIN_RRIGHT, BM_GETCHECK, 0, 0);
          FC->rbN = SendDlgItemMessage(hwnd, ID_MAIN_90, BM_GETCHECK, 0, 0);
          CreateThread (NULL, 0, DoMainFlip, FC, 0, NULL);
        }
        break;
      }
    }
    break;
    case WM_CLOSE:
    {
      int iok = IDOK;
      if (!saved)
      {
        iok = MessageBox (hwnd, "This will overwrite unsaved configurations! \n Are you sure you want to proceed?", "Warning",
          MB_OKCANCEL | MB_ICONWARNING);
      }
      if (iok == IDOK)
        DestroyWindow (hwnd);
    }
    break;
    case WM_DESTROY:
      PostQuitMessage (0);
    break;
    default:
      return DefWindowProc (hwnd, msg, wParam, lParam);
  }
  return 0;
}

BOOL
bluetooth_disconnect (HWND hwnd)
{
  if (WaitForSingleObject (bluetex, INFINITE) == WAIT_ABANDONED)
  {
    MessageBox (hwnd, "Failed to acquire bluetooth lock", "Error", MB_OK);
    return FALSE;
  }

  if (blue_sock == INVALID_SOCKET)
  {
    ReleaseMutex (bluetex);
    return TRUE;
  }

  if (closesocket (blue_sock) == INVALID_SOCKET)
  {
    MessageBox (hwnd, "Unable to close Bluetooth socket!", "Error", MB_OK);
    ReleaseMutex (bluetex);
    return FALSE;
  }

  blue_sock = INVALID_SOCKET;
  ReleaseMutex (bluetex);

  return TRUE;
}

BOOL
bluetooth_connect (HWND hwnd, PBLUECONFIG blue)
{
  if (blue == NULL)
    return FALSE;

  if (WaitForSingleObject (bluetex, INFINITE) == WAIT_ABANDONED)
  {
    MessageBox (hwnd, "Failed to acquire bluetooth lock", "Error", MB_OK);
    return FALSE;
  }

  ULONG ulRetCode, CXN_SUCCESS = 0;
  int addrLen = sizeof (blue_conn);
  ulRetCode = WSAStringToAddress ((LPSTR) &blue->BD_ADDR, AF_BTH, NULL, (LPSOCKADDR) &blue_conn, &addrLen);
  if (CXN_SUCCESS != ulRetCode)
  {
    MessageBox (hwnd, "Unable to convert address of Bluetooth!", "Error", MB_OK);
    ReleaseMutex (bluetex);
    return FALSE;
  }
  blue_conn.addressFamily = AF_BTH;
  blue_conn.serviceClassId = RFCOMM_PROTOCOL_UUID;
  blue_conn.port = 0;

  blue_sock = socket (AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
  if (blue_sock == INVALID_SOCKET)
  {
    MessageBox (hwnd, "Unable to create Bluetooth socket!", "Error", MB_OK);
    ReleaseMutex (bluetex);
    return FALSE;
  }

  if (connect (blue_sock, (struct sockaddr*) &blue_conn, sizeof(SOCKADDR_BTH)) == SOCKET_ERROR)
  {
    printf ("%d\n", WSAGetLastError ());
    MessageBox (hwnd, "Unable to connect Bluetooth socket!", "Error", MB_OK);
    closesocket (blue_sock);
    blue_sock = INVALID_SOCKET;
    ReleaseMutex (bluetex);
    return FALSE;
  }

  ReleaseMutex (bluetex);
  return TRUE;
}

BOOL CALLBACK
ConfigConnectDlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
  {
    case WM_INITDIALOG:
    {
      if (update_device != -1)
      {
        PBLUECONFIG blue = bluetooth_configs[update_device];
        SetDlgItemText (hwnd, ID_CONNECT_NAME, blue->BD_NAME);
        SetDlgItemText (hwnd, ID_CONNECT_ADDR, blue->BD_ADDR);
      }

      SendMessage (hwnd, ID_CONNECT_START, 0, 0);
      return TRUE;
    }
    break;
    case WM_COMMAND:
    {
      switch (LOWORD (wParam))
      {
        case ID_CONNECT_START:
        {
          if (connected_device == update_device)
            return TRUE;

          BOOL status;

          SetDlgItemText (hwnd, ID_CONNECT_STATUS, "Disconnecting Old...");
          status = bluetooth_disconnect (hwnd);

          if (!status)
          {
            SetDlgItemText (hwnd, ID_CONNECT_STATUS, "Failed Disconnecting Old!");
            return FALSE;
          } else
            connected_device = -1;

          SetDlgItemText (hwnd, ID_CONNECT_STATUS, "Connecting New...");
          status = bluetooth_connect (hwnd, bluetooth_configs[update_device]);

          if (status)
          {
            connected_device = update_device;
            SetDlgItemText (hwnd, ID_CONNECT_STATUS, "Connected!");
            Sleep (250);
            SendMessage (hwnd, WM_CLOSE, 0, 0);
            return TRUE;
          } else
          {
            SetDlgItemText (hwnd, ID_CONNECT_STATUS, "Failed Connecting New!");
            Sleep (250);
            SendMessage (hwnd, WM_CLOSE, 0, 0);
            return FALSE;
          }

          SendMessage (hwnd, WM_CLOSE, 0, 0);
        }
        break;
      }
    }
    break;
    case WM_CLOSE:
      EndDialog (hwnd, 0);
    break;
    default:
      return FALSE;
  }
  return TRUE;
}

BOOL CALLBACK
ConfigCreateDlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
  {
    case WM_INITDIALOG:
    {
      if (update_device == -1)
      {
        SendDlgItemMessage(hwnd, ID_CREATE_FULL, BM_SETCHECK, 1, 0);
        SendDlgItemMessage(hwnd, ID_CREATE_SINGLE, BM_SETCHECK, 0, 0);
      } else
      {
        PBLUECONFIG b = bluetooth_configs[update_device];
        SetWindowText (hwnd, "Update Cube...");
        SetDlgItemText (hwnd, ID_CREATE_ADD, "Update");
        SetDlgItemText (hwnd, ID_CREATE_NAME, (LPCTSTR) b->BD_NAME);
        SetDlgItemText (hwnd, ID_CREATE_ADDR, (LPCTSTR) b->BD_ADDR);
        if (b->FULL)
        {
          SendDlgItemMessage (hwnd, ID_CREATE_FULL, BM_SETCHECK, 1, 0);
          SendDlgItemMessage (hwnd, ID_CREATE_SINGLE, BM_SETCHECK, 0, 0);
        } else
        {
          SendDlgItemMessage (hwnd, ID_CREATE_SINGLE, BM_SETCHECK, 1, 0);
          SendDlgItemMessage (hwnd, ID_CREATE_FULL, BM_SETCHECK, 0, 0);
        }
      }
    }
    break;
    case WM_COMMAND:
    {
      switch (LOWORD (wParam))
      {
        case ID_CREATE_ADD:
        {
          int len_name = GetWindowTextLength (GetDlgItem (hwnd, ID_CREATE_NAME));
          int len_addr = GetWindowTextLength (GetDlgItem (hwnd, ID_CREATE_ADDR));
          int full = SendDlgItemMessage(hwnd, ID_CREATE_FULL, BM_GETCHECK, 0, 0);
          if (0 < len_name && 0 < len_addr)
          {
            if (MAX_NAME_LEN < len_name)
              MessageBox (hwnd, "Name too long!", "Error", MB_OK);
            else if (len_addr != BLUE_ADDR_LEN)
              MessageBox (hwnd, "Bluetooth address incorrect length!", "Error", MB_OK);
            else
            {
              PBLUECONFIG blue;
              if (update_device == -1)
                blue = (PBLUECONFIG) GlobalAlloc (GPTR, sizeof (BLUECONFIG));
              else
                blue = bluetooth_configs[update_device];

              GetDlgItemText (hwnd, ID_CREATE_NAME, (char*) blue->BD_NAME, len_name + 1);
              GetDlgItemText (hwnd, ID_CREATE_ADDR, (char*) blue->BD_ADDR, len_addr + 1);
              if (full == 1)
                blue->FULL = TRUE;
              else
                blue->FULL = FALSE;

              HWND parent = GetParent (hwnd);
              SendDlgItemMessage (parent, ID_CONFIG_LIST, LB_INSERTSTRING, update_device, (LPARAM) blue->BD_NAME);
              SetDlgItemText (parent, ID_CONFIG_TEXT, (LPCSTR) blue->BD_NAME);

              if (update_device == -1)
              {
                bluetooth_configs[cur_device] = blue;
                cur_device++;
              } else
              {
                HWND hList = GetDlgItem (parent, ID_CONFIG_LIST);
                SendMessage (hList, LB_DELETESTRING, (WPARAM) update_device + 1, 0);
              }

              saved = FALSE;
              EndDialog (hwnd, 0);
            }
          } 
        else if (len_name == 0)
          MessageBox (hwnd, "You didn't enter a name!", "Error", MB_OK);
        else if (len_addr == 0)
          MessageBox (hwnd, "You didn't enter and address!", "Error", MB_OK);
        else
          MessageBox (hwnd, "You didn't enter anything!", "Error", MB_OK);
        }
        break;
        case ID_CREATE_FULL:
        {
          if (SendDlgItemMessage (hwnd, ID_CREATE_FULL, BM_GETCHECK, 0, 0) == 0)
          {
            SendDlgItemMessage (hwnd, ID_CREATE_FULL, BM_SETCHECK, 1, 0);
            SendDlgItemMessage (hwnd, ID_CREATE_SINGLE, BM_SETCHECK, 0, 0);
          }
        }
        break;
        case ID_CREATE_SINGLE:
        {
          if (SendDlgItemMessage (hwnd, ID_CREATE_SINGLE, BM_GETCHECK, 0, 0) == 0)
          {
            SendDlgItemMessage (hwnd, ID_CREATE_SINGLE, BM_SETCHECK, 1, 0);
            SendDlgItemMessage (hwnd, ID_CREATE_FULL, BM_SETCHECK, 0, 0);
          }
        }
        break;
      }
    }
    break;
    case WM_CLOSE:
      EndDialog (hwnd, 0);
    break;
    default:
      return FALSE;
  }
  return TRUE;
}

BOOL CALLBACK
ConfigEditDlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
  {
    case WM_INITDIALOG:
    {
      int i;
      for (i = 0; i < cur_device; i++)
        SendDlgItemMessage (hwnd, ID_CONFIG_LIST, LB_ADDSTRING, 0, (LPARAM) bluetooth_configs[i]->BD_NAME);
    }
    break;
    case WM_COMMAND:
    {
      switch (LOWORD (wParam))
      {
        case ID_CONFIG_UPDATE:
        {
          HWND hList = GetDlgItem (hwnd, ID_CONFIG_LIST);
          int count = SendMessage(hList, LB_GETSELCOUNT, 0, 0);
          if (count != LB_ERR)
          {
            if (count != 0)
            {
              if (count == 1)
              {
                int *buf = (int*) GlobalAlloc (GPTR, sizeof (int) * count);
                SendMessage (hList, LB_GETSELITEMS, (WPARAM) count, (LPARAM) buf);
                update_device = buf[0];

                int ret = DialogBox (GetModuleHandle (NULL),
                  MAKEINTRESOURCE (ID_NOSLEEP_CREATE), hwnd, ConfigCreateDlgProc);

                if (ret == -1)
                  MessageBox (hwnd, "Dialog failed!", "Error",
                    MB_OK | MB_ICONINFORMATION);

                update_device = -1;
                GlobalFree (buf);
              } else
                MessageBox (hwnd, "Select only one item for update.", "Error", MB_OK);
            } else 
              MessageBox (hwnd, "No items selected.", "Warning", MB_OK);
          } else
            MessageBox (hwnd, "Error counting items :(", "Error", MB_OK);
        }
        break;
        case ID_CONFIG_CREATE:
        {
          if (cur_device < MAX_DEVICES)
          {
            int ret = DialogBox (GetModuleHandle (NULL),
              MAKEINTRESOURCE (ID_NOSLEEP_CREATE), hwnd, ConfigCreateDlgProc);

            if (ret == -1)
              MessageBox (hwnd, "Dialog failed!", "Error",
                MB_OK | MB_ICONINFORMATION);
          } else
            MessageBox (hwnd, "Too many stored addresses! Must delete some before adding more.", "Error", MB_OK);
        }
        break;
        case ID_CONFIG_DELETE:
        {
          HWND hList = GetDlgItem (hwnd, ID_CONFIG_LIST);
          int count = SendMessage(hList, LB_GETSELCOUNT, 0, 0);
          if (count != LB_ERR)
          {
            if (count != 0)
            {
              int i, j;
              int *buf = (int*) GlobalAlloc (GPTR, sizeof (int) * count);
              SendMessage (hList, LB_GETSELITEMS, (WPARAM) count, (LPARAM) buf);

              for (i = 0; i < count; i++)
              {
                SendMessage (hList, LB_DELETESTRING, (WPARAM) buf[i], 0);
                GlobalFree ((HANDLE) bluetooth_configs[buf[i]]);
                for (j = buf[i]; i < cur_device; i++)
                  bluetooth_configs[j] = bluetooth_configs[j + 1];
              }

              GlobalFree (buf);
              cur_device -= count;
            } else 
              MessageBox (hwnd, "No items selected.", "Warning", MB_OK);
          } else
            MessageBox (hwnd, "Error counting items :(", "Error", MB_OK);
        }
        break;
        case ID_CONFIG_CLEAR:
        {
          SendDlgItemMessage (hwnd, ID_CONFIG_LIST, LB_RESETCONTENT, 0, 0);
          int i;
          for (i = 0; i < cur_device; i++)
            GlobalFree ((HANDLE) bluetooth_configs[i]);
          cur_device = 0;
        }
        break;
        case ID_CONFIG_CONNECT:
        {
          HWND hList = GetDlgItem (hwnd, ID_CONFIG_LIST);
          int count = SendMessage(hList, LB_GETSELCOUNT, 0, 0);
          if (count != LB_ERR)
          {
            if (count != 0)
            {
              if (count == 1)
              {
                int *buf = (int*) GlobalAlloc (GPTR, sizeof (int) * count);
                SendMessage (hList, LB_GETSELITEMS, (WPARAM) count, (LPARAM) buf);
                update_device = buf[0];

                int ret = DialogBox (GetModuleHandle (NULL),
                  MAKEINTRESOURCE (ID_NOSLEEP_CONNECT), hwnd, ConfigConnectDlgProc);

                if (ret == -1)
                  MessageBox (hwnd, "Dialog failed!", "Error",
                    MB_OK | MB_ICONINFORMATION);

                update_device = -1;
                GlobalFree (buf);
              } else
                MessageBox (hwnd, "Select only one item for update.", "Error", MB_OK);
            } else 
              MessageBox (hwnd, "No items selected.", "Warning", MB_OK);
          } else
            MessageBox (hwnd, "Error counting items :(", "Error", MB_OK);
        }
        break;
        break;
        case ID_NOSLEEP_LIST:
          switch (HIWORD (wParam))
          {
            case LBN_SELCHANGE:
            {
              HWND hList = GetDlgItem (hwnd, ID_CONFIG_LIST);
              int count = SendMessage (hList, LB_GETSELCOUNT, 0, 0);
              if (count != LB_ERR)
              {
                if (count == 1)
                {
                  int index;
                  int err = SendMessage (hList, LB_GETSELITEMS, (WPARAM) 1, (LPARAM) &index);

                  if (err != LB_ERR)
                    SendMessage (hList, LB_GETITEMDATA, (WPARAM) index, 0);
                  else
                    MessageBox (hwnd, "Error getting selected item :(", "Error", MB_OK);
                }
              } else
                MessageBox (hwnd, "Error counting items :(", "Error", MB_OK);
            }
            break;
          }
        break;
      }
    }
    break;
    case WM_CLOSE:
      EndDialog (hwnd, 0);
    break;
    default:
      return FALSE;
  }
  return TRUE;
}

BOOL CALLBACK
AboutDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_INITDIALOG:
      return TRUE;
    case WM_COMMAND:
      switch (LOWORD (wParam))
      {
        case IDOK:
          EndDialog (hwnd, IDOK);
        break;
        case IDCANCEL:
          EndDialog (hwnd, IDCANCEL);
        break;
      }
    break;
    default:
      return FALSE;
  }
  return TRUE;
}

DWORD WINAPI
DoMainFlip (LPVOID args)
{
  DWORD res;
  PFLIPCUBE FC;
  FC = (PFLIPCUBE) args;

  res = WaitForSingleObject (bluetex, INFINITE);
  if (res == WAIT_ABANDONED)
    goto threadEnd;

  if (blue_sock == INVALID_SOCKET)
  {
    res = 1;
    ReleaseMutex (bluetex);
    goto threadEnd;
  }

  char msg[3] = "FFF";
  if (FC->rbT)
    msg[0] = 'T';
  if (FC->rbR)
    msg[1] = 'T';
  if (FC->rbN)
    msg[2] = 'T';

  if (send (blue_sock, msg, 3, 0) == SOCKET_ERROR)
  {
    res = 2;
    ReleaseMutex (bluetex);
    goto threadEnd;
  }

  if (!ReleaseMutex (bluetex))
    res = 3;
  else
    res = 0;

  threadEnd:
  HeapFree (GetProcessHeap (), HEAP_ZERO_MEMORY, FC);
  ExitThread (res);
}

void
DoHelpAbout (HWND hwnd)
{
  int ret = DialogBox (GetModuleHandle (NULL), 
      MAKEINTRESOURCE (ID_NOSLEEP_ABOUT), hwnd, AboutDlgProc);
  if (ret == -1)
    MessageBox (hwnd, "About dialog failed!", "Error",
      MB_OK | MB_ICONINFORMATION);
}

void
DoFileOpen (HWND hwnd)
{
  if (!saved)
  {
    int iok = MessageBox (hwnd, "This will overwrite unsaved configurations! \n Are you sure you want to proceed?", "Warning",
      MB_OKCANCEL | MB_ICONWARNING);
    if (iok != IDOK)
      return;
  }

  OPENFILENAME ofn;
  char szFileName[MAX_PATH] = "";

  ZeroMemory (&ofn, sizeof (ofn));

  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = hwnd;
  ofn.lpstrFilter = "Cube Config (*.ccfg)\0*.ccfg\0All Files (*.*)\0*.*\0";
  ofn.lpstrFile = szFileName;
  ofn.nMaxFile = MAX_PATH;
  ofn.lpstrDefExt = "ccfg";
  ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

  if (GetOpenFileName (&ofn))
  {
    HWND hEdit = GetDlgItem (hwnd, ID_NOSLEEP_MAIN);
    LoadConfig (hEdit, szFileName);
    saved = TRUE;
  }
}

void
DoFileSave (HWND hwnd)
{
  OPENFILENAME ofn;
  char szFileName[MAX_PATH] = "";

  ZeroMemory (&ofn, sizeof (ofn));

  ofn.lStructSize = sizeof (OPENFILENAME);
  ofn.hwndOwner = hwnd;
  ofn.lpstrFilter = "Cube Config (*.ccfg)\0*.ccfg\0All Files (*.*)\0*.*\0";
  ofn.lpstrFile = szFileName;
  ofn.nMaxFile = MAX_PATH;
  ofn.lpstrDefExt = "ccfg";
  ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

  if (GetSaveFileName (&ofn))
  {
    HWND hEdit = GetDlgItem (hwnd, ID_NOSLEEP_MAIN);
    saved = SaveConfig (hEdit, szFileName);
  }
}

BOOL
SaveConfig (HWND hEdit, LPCTSTR pszFileName)
{
  HANDLE hFile;
  BOOL bSuccess = FALSE;

  hFile = CreateFile (pszFileName, GENERIC_WRITE, 0, NULL,
    CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile != INVALID_HANDLE_VALUE)
  {
    DWORD dwTextLength;

    dwTextLength = GetWindowTextLength (hEdit);

    if (0 < dwTextLength)
    {
      LPSTR pszText;
      DWORD dwBufferSize = dwTextLength + 1;

      pszText = (LPSTR) GlobalAlloc (GPTR, dwBufferSize);
      if(pszText != NULL)
      {
        if (GetWindowText (hEdit, pszText, dwBufferSize))
        {
          DWORD dwWritten;

          if (WriteFile (hFile, pszText, dwTextLength, &dwWritten, NULL))
            bSuccess = TRUE;
        }
        GlobalFree (pszText);
      }
    }
    CloseHandle (hFile);
  }
  return bSuccess;
}

BOOL
LoadConfig (HWND hEdit, LPCTSTR pszFileName)
{
  HANDLE hFile;
  BOOL bSuccess = FALSE;

  hFile = CreateFile (pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
    OPEN_EXISTING, 0, NULL);
  if (hFile != INVALID_HANDLE_VALUE)
  {
    DWORD dwFileSize;

    dwFileSize = GetFileSize (hFile, NULL);
    if (dwFileSize != 0xFFFFFFFF)
    {
      LPSTR pszFileText;

      pszFileText = (LPSTR) GlobalAlloc (GPTR, dwFileSize + 1);
      if (pszFileText != NULL)
      {
        DWORD dwRead;

        if (ReadFile (hFile, pszFileText, dwFileSize, &dwRead, NULL))
        {
          pszFileText[dwFileSize] = 0;
          if (SetWindowText (hEdit, pszFileText))
            bSuccess = TRUE;
        }
        GlobalFree (pszFileText);
      }
    }
    CloseHandle (hFile);
  }
  return bSuccess;
}
