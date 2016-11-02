This project represents a rough, bare bones stub of an architecture that could be used for 
handling animation support, including extracting data from FBX files.

This project should generally be in working order and able to compile and run, though not
yet do much of anything interesting. The project does require the FBX SDK, and will need 
the paths added or changed to include the locations of your include and library files
for the FBX SDK.

1.) Open AnimationStub project properties by selecting the project in the solution explorer then 
	pressing alt+enter.
2.) Click on VC++ Directories on the left hand side of the properties window.
3.) Set Configuration and Platform drop down options both to "All ..."
4.) Edit the "Include Directories" and add a path for the include folder of your FBX SDK, such as 
	C:\Program Files\Autodesk\FBX\FBX SDK\2016.1.2\include
5.) Set the library directories option the same way, but do not include the x86/x64 & release/debug
	portions, such as C:\Program Files\Autodesk\FBX\FBX SDK\2016.1.2\lib\vs2015. If you look in the project 
	properties Linker->Input you will see we include the platform and configuration in our includes, 
	such as "x86\debug\libfbxsdk-md.lib"
6.) It should be able to compile and run the project at this point, though it will assert on a null
	pointer referenc called "animPtr"
7.) Missing functionality should have comments to direct you in adding that functionality, as well
	as a "TODO" flag that can be searched for to find all the areas where code is missing. There are
	likely a few "TODO"s in the wizard code in "AnimationStub.cpp" that can be ignored.
























========================================================================
    WIN32 APPLICATION : AnimationStub Project Overview
========================================================================

AppWizard has created this AnimationStub application for you.

This file contains a summary of what you will find in each of the files that
make up your AnimationStub application.


AnimationStub.vcxproj
    This is the main project file for VC++ projects generated using an Application Wizard.
    It contains information about the version of Visual C++ that generated the file, and
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

AnimationStub.vcxproj.filters
    This is the filters file for VC++ projects generated using an Application Wizard. 
    It contains information about the association between the files in your project 
    and the filters. This association is used in the IDE to show grouping of files with
    similar extensions under a specific node (for e.g. ".cpp" files are associated with the
    "Source Files" filter).

AnimationStub.cpp
    This is the main application source file.

/////////////////////////////////////////////////////////////////////////////
AppWizard has created the following resources:

AnimationStub.rc
    This is a listing of all of the Microsoft Windows resources that the
    program uses.  It includes the icons, bitmaps, and cursors that are stored
    in the RES subdirectory.  This file can be directly edited in Microsoft
    Visual C++.

Resource.h
    This is the standard header file, which defines new resource IDs.
    Microsoft Visual C++ reads and updates this file.

AnimationStub.ico
    This is an icon file, which is used as the application's icon (32x32).
    This icon is included by the main resource file AnimationStub.rc.

small.ico
    This is an icon file, which contains a smaller version (16x16)
    of the application's icon. This icon is included by the main resource
    file AnimationStub.rc.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named AnimationStub.pch and a precompiled types file named StdAfx.obj.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" comments to indicate parts of the source code you
should add to or customize.

/////////////////////////////////////////////////////////////////////////////
