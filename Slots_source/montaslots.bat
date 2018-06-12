@echo off
echo Montando os firmwares dos Slots
echo.

set slot1=Impresso
set slot2=Serial
set slot3=Relogio
set slot4=TKClock
set slot5=Mouse
set slot6=Disk_II
set slot7=HD
set slot8=CPMZ80
set slot9=IDE

if exist %slot1%.asm     call compilar %slot1%
if exist %slot1%_Ext.asm call compilar %slot1%_Ext
if exist %slot2%.asm     call compilar %slot2%
if exist %slot2%_Ext.asm call compilar %slot2%_Ext
if exist %slot3%.asm     call compilar %slot3%
if exist %slot3%_Ext.asm call compilar %slot3%_Ext
if exist %slot4%.asm     call compilar %slot4%
if exist %slot4%_Ext.asm call compilar %slot4%_Ext
if exist %slot5%.asm     call compilar %slot5%
if exist %slot5%_Ext.asm call compilar %slot5%_Ext
if exist %slot6%.asm     call compilar %slot6%
if exist %slot6%_Ext.asm call compilar %slot6%_Ext
if exist %slot7%.asm     call compilar %slot7%
if exist %slot7%_Ext.asm call compilar %slot7%_Ext
if exist %slot8%.asm     call compilar %slot8%
if exist %slot8%_Ext.asm call compilar %slot8%_Ext
if exist %slot9%.asm     call compilar %slot9%
if exist %slot9%_Ext.asm call compilar %slot9%_Ext

copy /b %slot1%.rom+%slot1%_Ext.rom Firmware_%slot1%.rom
copy /b %slot2%.rom+%slot2%_Ext.rom Firmware_%slot2%.rom
copy /b %slot3%.rom+%slot3%_Ext.rom Firmware_%slot3%.rom
copy /b %slot4%.rom+%slot4%_Ext.rom Firmware_%slot4%.rom
copy /b %slot5%.rom+%slot5%_Ext.rom Firmware_%slot5%.rom
copy /b %slot6%.rom+%slot6%_Ext.rom Firmware_%slot6%.rom
copy /b %slot7%.rom+%slot7%_Ext.rom Firmware_%slot7%.rom
copy /b %slot8%.rom+%slot8%_Ext.rom Firmware_%slot8%.rom
copy /b %slot9%.rom+%slot9%_Ext.rom Firmware_%slot9%.rom

echo.
echo Arquivos montados.
pause