!macro KeyExists ROOT MAIN_KEY KEY
push $R0
push $R1

!define Index 'Line${__LINE__}'

StrCpy $R1 "0"

"${Index}-Loop:"
; Check for Key
EnumRegKey $R0 ${ROOT} "${MAIN_KEY}" "$R1"
StrCmp $R0 "" "${Index}-False"
  IntOp $R1 $R1 + 1
  StrCmp $R0 "${KEY}" "${Index}-True" "${Index}-Loop"

"${Index}-True:"
;Return 1 if found
push "1"
goto "${Index}-End"

"${Index}-False:"
;Return 0 if not found
push "0"
goto "${Index}-End"

"${Index}-End:"
!undef Index
exch 2
pop $R0
pop $R1
!macroend