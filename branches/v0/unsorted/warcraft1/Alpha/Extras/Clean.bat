del ..\Alpha.dll
del ..\Alpha.dll.mdb
del ..\Alpha.pdb
del ..\Alpha.dll.manifest
del ..\Alpha\Alpha.mdp

del ..\Alpha-WC1.exe
del ..\Alpha-WC1.exe.mdb
del ..\Alpha-WC1.pdb
del ..\Alpha-WC1.exe.manifest
del ..\Alpha-WC1\Alpha-WC1.mdp

del ..\Alpha-Demo.exe
del ..\Alpha-Demo.exe.mdb
del ..\Alpha-Demo.pdb
del ..\Alpha-Demo.exe.manifest
del ..\Alpha-Demo\Alpha-Demo.mdp

del ..\SDL.dll
del ..\Tao.*.dll
del ..\Tao.*.dll.config
del ..\Makefile

Prebuild /clean all /file ..\prebuild.xml /pause
