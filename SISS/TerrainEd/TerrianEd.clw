; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CTerrianEdView
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "terrianed.h"
LastPage=0

ClassCount=9
Class1=CCameraPosDlg
Class2=CInitEngineDlg
Class3=CMainFrame
Class4=CRParamsDlg
Class5=CTerrianEdApp
Class6=CAboutDlg
Class7=CTerrianEdDoc
Class8=CTerrianEdView

ResourceCount=8
Resource1=IDD_DIALOG_TEXTOUTPUT (Englisch (USA))
Resource2=IDD_RPARAMS (Englisch (GB))
Resource3=IDD_ABOUTBOX (Englisch (USA))
Resource4=IDD_DATAOUTPUT (Englisch (USA))
Resource5=IDD_DIALOG_SETCAMERA
Resource6=IDD_DIALOG_DATAOUTPUT (Englisch (USA))
Class9=CTextOutputDlg
Resource7=IDD_INITENGINE (Englisch (USA))
Resource8=IDR_MAINFRAME (Englisch (USA))

[CLS:CCameraPosDlg]
Type=0
BaseClass=CDialog
HeaderFile=CameraPosDlg.h
ImplementationFile=CameraPosDlg.cpp

[CLS:CInitEngineDlg]
Type=0
BaseClass=CDialog
HeaderFile=InitEngineDlg.h
ImplementationFile=InitEngineDlg.cpp
LastObject=IDOK

[CLS:CMainFrame]
Type=0
BaseClass=CFrameWnd
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
LastObject=CMainFrame

[CLS:CRParamsDlg]
Type=0
BaseClass=CDialog
HeaderFile=RParamsDlg.h
ImplementationFile=RParamsDlg.cpp
LastObject=CRParamsDlg

[CLS:CTerrianEdApp]
Type=0
BaseClass=CWinApp
HeaderFile=TerrianEd.h
ImplementationFile=TerrianEd.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=TerrianEd.cpp
ImplementationFile=TerrianEd.cpp
LastObject=CAboutDlg

[CLS:CTerrianEdDoc]
Type=0
BaseClass=CDocument
HeaderFile=TerrianEdDoc.h
ImplementationFile=TerrianEdDoc.cpp

[CLS:CTerrianEdView]
Type=0
BaseClass=CView
HeaderFile=TerrianEdView.h
ImplementationFile=TerrianEdView.cpp
LastObject=ID_VIEW_MAPINFO
Filter=C
VirtualFilter=VWC

[DLG:IDD_DIALOG_SETCAMERA]
Type=1
Class=CCameraPosDlg
ControlCount=16
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_EDIT_XPOS,edit,1350631552
Control4=IDC_EDIT_YPOS,edit,1350631552
Control5=IDC_EDIT_ZPOS,edit,1350631552
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_EDIT_XROT,edit,1350631552
Control10=IDC_EDIT_YROT,edit,1350631552
Control11=IDC_EDIT_ZROT,edit,1350631552
Control12=IDC_STATIC,static,1342308352
Control13=IDC_STATIC,static,1342308352
Control14=IDC_STATIC,static,1342308352
Control15=IDC_EDIT_CAMSPEED,edit,1350631552
Control16=IDC_STATIC,static,1342308352

[DLG:IDD_INITENGINE]
Type=1
Class=CInitEngineDlg

[DLG:IDD_RPARAMS]
Type=1
Class=CRParamsDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg

[DLG:IDD_ABOUTBOX (Englisch (USA))]
Type=1
Class=?
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_INITENGINE (Englisch (USA))]
Type=1
Class=?
ControlCount=17
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_EDIT_FILENAME,edit,1350631552
Control4=IDC_EDIT_NEARCLIP,edit,1350631552
Control5=IDC_EDIT_TOPCOORD,edit,1350631552
Control6=IDC_EDIT_DELTAH,edit,1350631552
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352
Control11=IDC_BUTTON_BROWSE,button,1342242816
Control12=IDC_STATIC,button,1342177287
Control13=IDC_RADIO1,button,1342308361
Control14=IDC_RADIO2,button,1342177289
Control15=IDC_RADIO3,button,1342177289
Control16=IDC_STATIC,button,1342177287
Control17=IDC_RADIO4,button,1342177289

[DLG:IDD_RPARAMS (Englisch (GB))]
Type=1
Class=?
ControlCount=20
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_SLIDERDXYZ,msctls_trackbar32,1342242825
Control4=IDC_STATICDXYZ,static,1342308352
Control5=IDC_STATICDZ,static,1342308352
Control6=IDC_SLIDERDZ,msctls_trackbar32,1342242824
Control7=IDC_STATICDL,static,1342308352
Control8=IDC_SLIDERDL,msctls_trackbar32,1342242824
Control9=IDC_STATICRD,static,1342308352
Control10=IDC_SLIDERRD,msctls_trackbar32,1342242824
Control11=IDC_SLIDERMIRD,msctls_trackbar32,1342242824
Control12=IDC_SLIDERMXRD,msctls_trackbar32,1342242824
Control13=IDC_STATICMIRD,static,1342308352
Control14=IDC_STATICMXRD,static,1342308352
Control15=IDC_EDIT_DXYZ,edit,1350631552
Control16=IDC_EDIT_DZ,edit,1350631552
Control17=IDC_EDIT_DETAIL_LEVEL,edit,1350631552
Control18=IDC_EDIT_RENDER_DISTANCE,edit,1350631552
Control19=IDC_EDIT_MIN_RENDER_DISTANCE,edit,1350631552
Control20=IDC_EDIT_MAX_RENDER_DISTANCE,edit,1350631552

[TB:IDR_MAINFRAME (Englisch (USA))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
Command8=ID_APP_ABOUT
CommandCount=8

[MNU:IDR_MAINFRAME (Englisch (USA))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_SAVE_AS
Command5=ID_FILE_MRU_FILE1
Command6=ID_APP_EXIT
Command7=ID_EDIT_UNDO
Command8=ID_EDIT_CUT
Command9=ID_EDIT_COPY
Command10=ID_EDIT_PASTE
Command11=ID_VIEW_TOOLBAR
Command12=ID_VIEW_STATUS_BAR
Command13=ID_VIEW_RENDERPARAMS
Command14=ID_VIEW_MODE
Command15=ID_VIEW_CAMERAPOS
Command16=ID_VIEW_MAPINFO
Command17=ID_APP_ABOUT
CommandCount=17

[ACL:IDR_MAINFRAME (Englisch (USA))]
Type=1
Class=?
Command1=ID_EDIT_COPY
Command2=ID_VIEW_MODE
Command3=ID_FILE_NEW
Command4=ID_FILE_OPEN
Command5=ID_VIEW_CAMERAPOS
Command6=ID_VIEW_RENDERPARAMS
Command7=ID_FILE_SAVE
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_NEXT_PANE
Command12=ID_PREV_PANE
Command13=ID_EDIT_COPY
Command14=ID_EDIT_PASTE
Command15=ID_EDIT_CUT
Command16=ID_EDIT_UNDO
CommandCount=16

[DLG:IDD_DATAOUTPUT (Englisch (USA))]
Type=1
Class=?
ControlCount=2
Control1=IDOK,button,1342242817
Control2=IDC_TEXTOUTPUT,edit,1350633600

[DLG:IDD_DIALOG_DATAOUTPUT (Englisch (USA))]
Type=1
Class=?
ControlCount=2
Control1=IDOK2,button,1342242817
Control2=IDC_TEXTOUTPUT,edit,1350633600

[DLG:IDD_DIALOG_TEXTOUTPUT (Englisch (USA))]
Type=1
Class=CTextOutputDlg
ControlCount=2
Control1=IDOK,button,1342242817
Control2=IDC_EDIT_TEXTOUTPUT,edit,1352734724

[CLS:CTextOutputDlg]
Type=0
HeaderFile=TextOutputDlg.h
ImplementationFile=TextOutputDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CTextOutputDlg
VirtualFilter=dWC

