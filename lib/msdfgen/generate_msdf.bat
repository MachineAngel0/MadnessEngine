@echo off

:: msdfgen.exe <mode> <input> <options>
:: msdfgen.exe -defineshape "{ 1471,0; 1149,0; 1021,333; 435,333; 314,0; 0,0; 571,1466; 884,1466; # }{ 926,580; 724,1124; 526,580; # }" -size 16 16 -autoframe -testrender render.png 1024 1024


:: msdfgen.exe msdf -defineshape "{ 1471,0; 1149,0; 1021,333; 435,333; 314,0; 0,0; 571,1466; 884,1466; # }{ 926,580; 724,1124; 526,580; # }" -size 16 16 -font "C:\Users\Adams Humbert\Documents\Clion\MadnessEngine\z_assets\fonts\arial.ttf" 'Y' -noemnormalize -o test.png

:: msdfgen.exe -help
:: msdf-atlas-gen.exe -help

:: dimensions <width> <height> set atlus dimensions  -size set gylph dimensions

msdf-atlas-gen.exe -font "C:\Users\Adams Humbert\Documents\Clion\MadnessEngine\z_assets\fonts\arial.ttf" -type msdf -dimensions 256 256 -size 32 -yorigin top -imageout arial.png
msdf-atlas-gen.exe -font "C:\Users\Adams Humbert\Documents\Clion\MadnessEngine\z_assets\fonts\arial.ttf" -type msdf -dimensions 256 256 -size 32 -yorigin top -imageout "C:\Users\Adams Humbert\Documents\Clion\MadnessEngine\z_assets\msdf_fonts\arial_msdf.png" -csv "C:\Users\Adams Humbert\Documents\Clion\MadnessEngine\z_assets\msdf_fonts\arial_msdf.csv"

@pause