# Caelum Magna VR
Caelum Magna VR Star Map made with Amazon Lumberyard and the PLY PostgreSQL Connector Gem

## Introduction

This repository serves as a reference project for using the PLY PostgreSQL Database connector Gem https://github.com/ash-j-f/PLY-public

For now, the intersting bit is the source code found in https://github.com/ash-j-f/CaelumMagnaVR-public/tree/master/Gem/Code which shows how to use various aspects of the PLY Gem.

**Until the star database creation and import process is documented, this project is not currently in a publicly usable state, but this will be completed in the near future.**

The project will build and run, but without a star database to connect to, it will not display any stars.

For a preview of the star map software running using my locally hosted star database, check out a video of the prototype at https://youtu.be/VHa8dVGE2vs

Check out my portfolio at https://ajflynn.io/

## Running the Executable

To load the Star Map Demo, open a windows command line console, change directory to the binary builds directory, and run the following command:
```
CaelumMagnaVR.exe +map StarMapDemo
```

To load the Object and Database synchronisation demo (which demonstrates a feature of the PLY Gem), open a windows command line console, change directory to the binary builds directory, and run the following command:
```
CaelumMagnaVR.exe +map SaveStateDemo
```

## Controls

### Star Map

TBC

### Object and Database Synchronisation

TBC

## Installation

1. Download and install the latest version of the Amazon Lumberyard game engine https://aws.amazon.com/lumberyard/

1. Run setup assistant from directory (lumberyard)\dev\Tools\LmbrSetup\Win\SetupAssistant.exe

1. In Setup Assistant, click "Customise" from the options presented.

1. On the first page of the Setup Assistant, ensure the following tickboxes are enabled:
    * Run your game project
    * Run the Lumberyard Editor and Tools
    * Compile the game code
    * Compile the engine and asset pipeline
    * Compile the Lumberyard Editor and Tool
    * Select option to use Visual Studio 2017
    
1. Go to the required SDKs tab and click "Install All". The download process may take a long time.

1. Check for any required SDKs that didn't get installed automatically by the previous step, and click their "Install SDK" button.

1. Configure Visual Studio 2017 for use with Lumberyard:
    * The Visual Studio 2017 default installation may not include all of the required features to run Lumberyard. Ensure these features are selected during installation. 
    * To verify your current installation of Visual Studio 2017 From Windows, click Control Panel, Programs and Features, Microsoft Visual Studio version_number. Select Modify. On the Workloads tab, do the following:
        * Select Universal Windows Platform Development and select the following: 
	    * C++ Universal Windows Platform tools 
	    * Graphics debugger and GPU profiler for DirectX 
	* Select Desktop development with C++ and select the following: 
	    * Visual C++ ATL for x86 and x64
	    * Visual C++ MFC for x86 and x64 
	    * *Note: You can run Lumberyard Editor without Visual C++ ATL and MFC libraries. However, to build Lumberyard Editor, you must install these libraries.*
	* Select Game development with C++ and select the following: 
	    * Windows 8.1 SDK and UCRT SDK 
	* On the Individual components tab, under Compilers, build tools, and runtime: You must select at least one version of the VC++ 2017 toolset.

1. Edit all shortcuts on Desktop and in Start Menu for Lumberyard software to point to the correct VC version. When using Visual Studio 2017 with Lumberyard, you need to change "Bin64vc140" in all shortcut paths to "Bin64vc141". This will affect at least the shortcuts to "Lumberyard Editor" and "Project Configurator".

1. Edit the file `(Lumberyard)\dev\_WAF_\user_settings.options` and update the `[Windows Options]` section to look like (be sure to remove the ";" from the start of the two lines as indicated):
    ```
    [Windows Options]
    
    ;win_enable_clang_for_windows = False
    ;win_vs2015_winkit = @get_wsdk_version
    win_vs2017_vswhere_args = -version [15,16]
    ;win_vs2017_vcvarsall_args = 
    win_vs2017_winkit = 10.0.16299.0
    ```
    Note: The above win_vs2017_winkit value must be set to match your exact locally installed Winkit version. To find your Winkit version number, go to control panel > programs > programs and features. Scroll down to your Visual Studio Version, select it and then click the "Change" button above the list. Click "Modify" next to the VS version listed in the VS Installer window that pops up. Click the Workloads tab. Look under "Indvidual Components" on the right to see the Windows SDK version number.

1. Download this repository and place its contents in the following directory: `(Lumberyard)\dev\CaelumMagnaVR'.

1. Run the Lumberyard Project Configurator `(Lumberyard)\dev\Bin64vc141\ProjectConfigurator.exe` and set the CaelumMagnaVR project as the default start project.

1. Run the Lumberyard "compile all" process from command line. To do this, open a windows command line console, change directory to `(Lumberyard)\dev\` and type the following commands:
    ```
    lmbr_waf configure
    lmbr_waf build_win_x64_vs2017_profile -p all
    ```
    * Note: The build process may take some time.
    * Tip: To prevent the build process interrupting the normal use of your PC if you plan to use it while compiling, you can lower the compile process priority using the following build command instead: `%windir%\system32\cmd.exe /c start /low lmbr_waf build_win_x64_vs2017_profile -p all`.
    
1. Once the build process completes successfully, the project can now be viewed via the Lumberyard Editor by running `(Lumberyard)\dev\Bin64vc141\Editor.exe`.

## Creating a Release Build

To compile the project into a release build, follow the release build instructions (along with the following modifications to those instructions) at https://docs.aws.amazon.com/lumberyard/latest/userguide/game-build-release.html#game-build-release-vs

1. For each game engine level, open the level file in Editor and select `Game > Export to Engine` from the menu.

1. Modify the file BuildSamplesProject_Paks_PC.bat as per the instructions in the link above, so that it references the CaelumMagnaVR project.

1. Open a windows command line console, change directory to `(Lumberyard)\dev\` and run the following command: `.\Tools\CrySCompileServer\x64\profile\CrySCompileServer_vc141x64.exe`. Leave this process running.

1. In a NEW windows command line console, change directory to `(Lumberyard)\dev\` and run the command `lmbr_pak_shaders.bat CaelumMagnaVR D3D11 pc`.

1. Copy the newly generated shader pak files from:
    ```
    (Lumberyard)\dev\build\pc\CaelumMagnaVR
    ```
    to
    ```
    (Lumberyard)\dev\CaelumMagnaVR_pc_paks\CaelumMagnaVR
    ```
    
1. In console from the `(Lumberyard)\dev\` directory, run the command `lmbr_waf build_win_x64_vs2017_release -p game_and_engine`.

1. Copy the directory `(Lumberyard)\dev\Bin64vc141.Release` to `(Lumberyard)\dev\CaelumMagnaVR_pc_paks\Bin64vc141.Release` (you can also use the Bin64vc141.Profile build here instead, if you need to do debugging on the standalone EXE).

1. The standalone executable can now be run from `(Lumberyard)\dev\CaelumMagnaVR_pc_paks\Bin64vc141.Release\CaelumMagnaVR.exe`
    Note: To load a specific level file you need to use the `+map` option on the command line after the exe name, eg:
        * `CaelumMagnaVR.exe +map StarMapDemo`
	* `CaelumMagnaVR.exe +map SaveStateDemo`
	
## Credits

Created by Ashley Flynn https://ajflynn.io/ while studying a degree in software engineering at the Academy of Interactive Entertainment and the Canberra Institute of Technology in 2019.
	
Special thanks to Dr. Coryn Bailer-Jones for providing star survery distance estimate data http://www2.mpia-hd.mpg.de/homes/calj/gdr2_distances/main.html which was used in the Star Map demonstration project that implements the PLY Gem https://github.com/ash-j-f/CaelumMagnaVR-public.
	
And special thanks also go to Josh Beacham for assistance with Amazon Lumberyard code and setup.
	
## License

This project is licensed under the GNU General Public License v3.0. See the file "LICENSE" for more details.
