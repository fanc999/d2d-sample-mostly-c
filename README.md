# d2d-sample-mostly-c
Mostly C port of Microsoft's Simple Direct2D tutorial

This is a largely-C port of the Direct2D tutorial that can be found at
https://docs.microsoft.com/en-us/windows/win32/direct2d/direct2d-quickstart,
where all the code are converted to plain C89 C, except for the part where we use
ID2D1HwndRenderTarget::GetSize, as there are issues in the C interface of this
code that we must call this using C++.

This is tested on Visual Studio 2008 Win32/x64 configs, and should work on later
versions of Visual Studio, up to and including 2019.

You will need a d2d1.h header that contains C definitions of methods for this to work, so
for Visual Studio 2015 and later you will need to first target the Windows 10 SDK 10586 or earlier,
using [vcvars|vcvarsx64].bat 10.0.10586.0, as the later Direct2D headers removed the C definitions
of the Direct2D COM functions.

Compile in a Visual Studio command prompt as follows (for release builds):
cl /MD /O2 /W3 /GL /Zi d2d-sample.c d2d-sample-helper.cpp /link /DEBUG /opt:ref /LTCG d2d1.lib ole32.lib user32.lib

(for debug builds):
cl /MDd /Od /W3 /GL /Zi d2d-sample.c d2d-sample-helper.cpp /link /DEBUG d2d1.lib ole32.lib user32.lib
