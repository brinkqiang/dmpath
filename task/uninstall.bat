cd %~dp0
nssm stop dmcrontask
nssm remove dmcrontask confirm
pause