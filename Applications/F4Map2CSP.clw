; CLW-Datei enthält Informationen für den MFC-Klassen-Assistenten

[General Info]
Version=1
LastClass=CF4Map2CSPDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "F4Map2CSP.h"

ClassCount=4
Class1=CF4Map2CSPApp
Class2=CF4Map2CSPDlg
Class3=CAboutDlg

ResourceCount=7
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_F4MAP2CSP_DIALOG
Resource4=IDD_DIALOG1
Resource5=IDD_ABOUTBOX (Englisch (USA))
Resource6=IDD_F4MAP2CSP_DIALOG (Englisch (USA))
Class4=CTextOutputDlg
Resource7=IDD_DIALOG_TEXTOUTPUT (Englisch (USA))

[CLS:CF4Map2CSPApp]
Type=0
HeaderFile=F4Map2CSP.h
ImplementationFile=F4Map2CSP.cpp
Filter=N

[CLS:CF4Map2CSPDlg]
Type=0
HeaderFile=F4Map2CSPDlg.h
ImplementationFile=F4Map2CSPDlg.cpp
Filter=D
LastObject=IDC_RADIO_L0
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=F4Map2CSPDlg.h
ImplementationFile=F4Map2CSPDlg.cpp
Filter=D
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Class=CAboutDlg


[DLG:IDD_F4MAP2CSP_DIALOG]
Type=1
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Class=CF4Map2CSPDlg

[DLG:IDD_F4MAP2CSP_DIALOG (Englisch (USA))]
Type=1
Class=CF4Map2CSPDlg
ControlCount=18
Control1=ID_CONVERT,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_EDIT_FILE_SRC,edit,1350631552
Control4=IDC_EDIT_FILE_DEST,edit,1350631552
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,button,1342177287
Control8=IDC_RADIO_L0,button,1342308361
Control9=IDC_RADIO_L1,button,1342177289
Control10=IDC_RADIO_L2,button,1342177289
Control11=IDC_RADIO_L3,button,1342177289
Control12=IDC_RADIO_L4,button,1342177289
Control13=IDC_RADIO_L5,button,1342177289
Control14=IDC_STATIC,static,1342308352
Control15=IDC_STATIC,static,1342308352
Control16=ID_BROWSE_DEST,button,1342242817
Control17=ID_BROWSE_SRC,button,1342242817
Control18=IDC_STATIC,static,1342308352

[DLG:IDD_ABOUTBOX (Englisch (USA))]
Type=1
Class=CAboutDlg
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,static,1342308352

[DLG:IDD_DIALOG_TEXTOUTPUT (Englisch (USA))]
Type=1
Class=CTextOutputDlg
ControlCount=2
Control1=IDOK,button,1342242817
Control2=IDC_EDIT_TEXTOUTPUT,edit,1350633600

[DLG:IDD_DIALOG1]
Type=1
Class=?
ControlCount=2
Control1=IDOK,button,1342242817
Control2=IDC_EDIT_TEXTOUTPUT,edit,1350633600

[CLS:CTextOutputDlg]
Type=0
HeaderFile=TextOutputDlg.h
ImplementationFile=TextOutputDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDC_EDIT_TEXTOUTPUT

