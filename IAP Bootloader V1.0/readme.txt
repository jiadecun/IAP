FLASH 

start:0x0800_0000 end:0x080F_FFFF size:(1 MByte)
FLASH ����11��sector
4��16K
1��64k
7��128K

IAP:ռ��ǰ4��16K����64K��sector0~3��
start:0x0800_0000 end:0x0800_FFFF size:64 kbyte

APP1:ռ��1��64K��3��128K����458K(sector4~7)
start:0x0801_0000 end:0x0808_27FF 

APP2��ռ��4��128K����512K��sector8~11��
start:0x0808_2800 end:0x080F_FFFF size:512kbyte

C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe --bin -o ..\OBJ\jia.bin ..\OBJ\jia.axf
D:\MDK5.14\ARM\ARMCC\bin\fromelf.exe  --bin -o  ..\OBJ\TEST.bin ..\OBJ\TEST.axf