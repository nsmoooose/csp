@echo off
REM Edit this as needed
set ICONV=iconv-1.7
set LIBXML2=libxml2-2.4.10
set LIBXSLT=libxslt-1.0.7

if "%MSVCDir%" == "" goto error1
if "%MSDevDir%" == "" set MSDevDir="%MSVCDir%"
if "%DLL_PATH%" == "" set dll_path="%MSDevDir%\Bin"
if "%CMD_PATH%" == "" set cmd_path="%MSDevDir%\Bin"

REM Copy all DLL in a directory in path
md "%DLL_PATH%"
md "%CMD_PATH"
copy "%ICONV%\util\iconv.exe" "%CMD_PATH%"
copy "%ICONV%\lib\iconv.dll" "%DLL_PATH%"
copy "%LIBXML2%\util\xmllint.exe" "%CMD_PATH%"
copy "%LIBXLM2%\lib\libxml2.dll" "%DLL_PATH%"
copy "%LIBXSLT%\util\xsltproc.exe" "%CMD_PATH%"
copy "%LIBXSLT%\lib\libxslt.dll" "%DLL_PATH%"
copy "%LIBXSLT%\lib\libexslt.dll" "%DLL_PATH%"

REM Copy all include headers
md "%MSVCDir%\Include"
copy "%ICONV%\include\iconv.h" "%MSVCDir%\Include"
md "%MSVCDir%\Include\libxml"
copy "%LIBXML2%\include\libxml\catalog.h" "%MSVCDir%\Include\libxml2"
copy "%LIBXML2%\include\libxml\debugXML.h" "%MSVCDir%\Include\libxml2"
copy "%LIBXML2%\include\libxml\DOCBparser.h" "%MSVCDir%\Include\libxml2"
copy "%LIBXML2%\include\libxml\encoding.h" "%MSVCDir%\Include\libxml2"
copy "%LIBXML2%\include\libxml\entities.h" "%MSVCDir%\Include\libxml2"
copy "%LIBXML2%\include\libxml\globals.h" "%MSVCDir%\Include\libxml2"
copy "%LIBXML2%\include\libxml\hash.h" "%MSVCDir%\Include\libxml2"
copy "%LIBXML2%\include\libxml\HTMLparser.h" "%MSVCDir%\Include\libxml2"
copy "%LIBXML2%\include\libxml\HTMLtree.h" "%MSVCDir%\Include\libxml2"
copy "%LIBXML2%\include\libxml\list.h" "%MSVCDir%\Include\libxml2"
copy "%LIBXML2%\include\libxml\nanoftp.h" "%MSVCDir%\Include\libxml2"
copy "%LIBXML2%\include\libxml\nanohttp.h" "%MSVCDir%\Include\libxml2"
copy "%LIBXML2%\include\libxml\parserInternals.h" "%MSVCDir%\Include\libxml2"
copy "%LIBXML2%\include\libxml\SAX.h" "%MSVCDir%\Include\libxml2"
copy "%LIBXML2%\include\libxml\threads.h" "%MSVCDir%\Include\libxml2"
copy "%LIBXML2%\include\libxml\tree.h" "%MSVCDir%\Include\libxml2"
copy "%LIBXML2%\include\libxml\uri.h" "%MSVCDir%\Include\libxml2"
copy "%LIBXML2%\include\libxml\valid.h" "%MSVCDir%\Include\libxml2"
copy "%LIBXML2%\include\libxml\xinclude.h" "%MSVCDir%\Include\libxml2"
copy "%LIBXML2%\include\libxml\xlink.h" "%MSVCDir%\Include\libxml2"
copy "%LIBXML2%\include\libxml\xpath.h" "%MSVCDir%\Include\libxml2"
copy "%LIBXML2%\include\libxml\xpathInternals.h" "%MSVCDir%\Include\libxml2"
copy "%LIBXML2%\include\libxml\xpointer.h" "%MSVCDir%\Include\libxml2"
md "%MSVCDir%\Include\libexslt"
copy "%LIBXSLT%\include\libexslt\exslt.h" "%MSVCDir%\Include\libexslt"
copy "%LIBXSLT%\include\libexslt\exsltconfig.h" "%MSVCDir%\Include\libexslt"
md "%MSVCDir%\Include\libxslt"
copy "%LIBXSLT%\include\libxslt\attributes.h" "%MSVCDir%\Include\libxslt"
copy "%LIBXSLT%\include\libxslt\documents.h" "%MSVCDir%\Include\libxslt"
copy "%LIBXSLT%\include\libxslt\extensions.h" "%MSVCDir%\Include\libxslt"
copy "%LIBXSLT%\include\libxslt\extra.h" "%MSVCDir%\Include\libxslt"
copy "%LIBXSLT%\include\libxslt\functions.h" "%MSVCDir%\Include\libxslt"
copy "%LIBXSLT%\include\libxslt\imports.h" "%MSVCDir%\Include\libxslt"
copy "%LIBXSLT%\include\libxslt\keys.h" "%MSVCDir%\Include\libxslt"
copy "%LIBXSLT%\include\libxslt\libxslt.h" "%MSVCDir%\Include\libxslt"
copy "%LIBXSLT%\include\libxslt\namespaces.h" "%MSVCDir%\Include\libxslt"
copy "%LIBXSLT%\include\libxslt\numbersInternals.h" "%MSVCDir%\Include\libxslt"
copy "%LIBXSLT%\include\libxslt\pattern.h" "%MSVCDir%\Include\libxslt"
copy "%LIBXSLT%\include\libxslt\preproc.h" "%MSVCDir%\Include\libxslt"
copy "%LIBXSLT%\include\libxslt\templates.h" "%MSVCDir%\Include\libxslt"
copy "%LIBXSLT%\include\libxslt\transform.h" "%MSVCDir%\Include\libxslt"
copy "%LIBXSLT%\include\libxslt\variables.h" "%MSVCDir%\Include\libxslt"
copy "%LIBXSLT%\include\libxslt\win32config.h" "%MSVCDir%\Include\libxslt"
copy "%LIBXSLT%\include\libxslt\xslt.h" "%MSVCDir%\Include\libxslt"
copy "%LIBXSLT%\include\libxslt\xsltconfig.h" "%MSVCDir%\Include\libxslt"
copy "%LIBXSLT%\include\libxslt\xsltInternals.h" "%MSVCDir%\Include\libxslt"
copy "%LIBXSLT%\include\libxslt\xsltutils.h" "%MSVCDir%\Include\libxslt"
copy "%LIBXSLT%\include\libxslt\xsltwin32config.h" "%MSVCDir%\Include\libxslt"

REM Copy all libs
md "%MSVCDir%\lib"
copy "%ICONV%\lib\iconv.lib" "%MSVCDir%\lib"
copy "%ICONV%\lib\iconv_a.lib" "%MSVCDir%\lib"
copy "%LIBXML2%\lib\libxml2.lib" "%MSVCDir%\lib"
copy "%LIBXML2%\lib\libxml2_a.lib" "%MSVCDir%\lib"
copy "%LIBXSLT%\lib\libxslt.lib" "%MSVCDir%\lib"
copy "%LIBXSLT%\lib\libxslt_a.lib" "%MSVCDir%\lib"
copy "%LIBXSLT%\lib\libexslt.lib" "%MSVCDir%\lib"
copy "%LIBXSLT%\lib\libexslt_a.lib" "%MSVCDir%\lib"

goto end

:error1
echo MSVCDIR Environment Variable not defined
echo Typically this is C:\Program Files\Microsoft Visual Studio\VC98
goto end

:end
