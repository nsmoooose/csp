; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=clientServerCom
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "ClientServer.h"

ClassCount=4
Class1=CClientServerApp
Class2=CClientServerDlg
Class3=CAboutDlg

ResourceCount=6
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_CLIENTSERVER_DIALOG
Resource4=IDD_DIALOG1
Resource5=IDD_ABOUTBOX (English (U.S.))
Class4=clientServerCom
Resource6=IDD_CLIENTSERVER_DIALOG (English (U.S.))

[CLS:CClientServerApp]
Type=0
HeaderFile=ClientServer.h
ImplementationFile=ClientServer.cpp
Filter=N

[CLS:CClientServerDlg]
Type=0
HeaderFile=ClientServerDlg.h
ImplementationFile=ClientServerDlg.cpp
Filter=D
LastObject=CClientServerDlg
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=ClientServerDlg.h
ImplementationFile=ClientServerDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Class=CAboutDlg


[DLG:IDD_CLIENTSERVER_DIALOG]
Type=1
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Class=CClientServerDlg

[DLG:IDD_CLIENTSERVER_DIALOG (English (U.S.))]
Type=1
Class=CClientServerDlg
ControlCount=17
Control1=IDOK,button,1342242817
Control2=IDC_IPADDRESS1,SysIPAddress32,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,button,1342177287
Control5=IDC_STATIC,static,1342308352
Control6=IDC_Port,edit,1350631552
Control7=IDC_BUTTON2,button,1342242816
Control8=IDC_BUTTON3,button,1342242816
Control9=IDC_Message,static,1342308352
Control10=IDC_IPADDRESS2,SysIPAddress32,1342242816
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,button,1342177287
Control13=IDC_UseUDPIP,button,1476395017
Control14=IDC_UseTCPIP,button,1342177289
Control15=IDC_STATIC,static,1342308352
Control16=IDC_ClientNr,edit,1350631552
Control17=IDC_STATIC,static,1342308352

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_DIALOG1]
Type=1
Class=clientServerCom
ControlCount=6
Control1=IDC_EDIT1,edit,1350635648
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_Receives,listbox,1352728833
Control5=IDC_BUTTON1,button,1342242816
Control6=IDC_CloseIt,button,1342242816

[CLS:clientServerCom]
Type=0
HeaderFile=clientServerCom.h
ImplementationFile=clientServerCom.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=clientServerCom

