rem @echo off
echo Limpando
del /s *.o *.obj *.pdb *.exp *.ncb *.dcu *.dof *.opt *.pch *.plg *.idb *.~* *.sbr *.ilk *.bsc *.sdf *.user *.tlog

set sz="C:\Program Files\7-Zip\7z.exe"

set nomedist=TK3000e
set version=2.4
set exclude=Docs

echo Criando Distribuicao

cd ..

%sz% a -r -tzip -x!%exclude% -x!.svn -x!ipch -x!Release -x!Debug %nomedist%-%version%-src.zip %nomedist%

move %nomedist%-%version%-src.zip %nomedist%

cd %nomedist%

dir /b %nomedist%-%version%-src.zip

pause
