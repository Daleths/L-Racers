del Assets.dat
del CopyAssets.log

REM --- BITMAPS ---
MyMiniz.exe Assets.dat + .\Assets\polepig03.bmpx >> CopyAssets.log
MyMiniz.exe Assets.dat + .\Assets\lightning01.bmpx >> CopyAssets.log
MyMiniz.exe Assets.dat + .\Assets\6x7font.bmpx >> CopyAssets.log
MyMiniz.exe Assets.dat + .\Assets\XBox360Controller.bmpx >> CopyAssets.log
MyMiniz.exe Assets.dat + .\Assets\TopAccent.bmpx >> CopyAssets.log

REM --- WAV FILES ---
MyMiniz.exe Assets.dat + .\Assets\Menu.wav >> CopyAssets.log
MyMiniz.exe Assets.dat + .\Assets\Choose.wav >> CopyAssets.log
MyMiniz.exe Assets.dat + .\Assets\Explosion.wav >> CopyAssets.log
MyMiniz.exe Assets.dat + .\Assets\SplashScreen.wav >> CopyAssets.log

REM --- OGG FILES ---
MyMiniz.exe Assets.dat + .\Assets\L-Racers-BG2.ogg >> CopyAssets.log

REM copy Assets.dat into whatever directory or directories it needs to be in

copy Assets.dat .\x64\Debug\Assets.dat
copy Assets.dat .\x64\Release\Assets.dat