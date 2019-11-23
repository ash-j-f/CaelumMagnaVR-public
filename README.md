# Caelum Magna VR
Caelum Magna VR Star Map made with Amazon Lumberyard and the PLY PostgreSQL Connector Gem

## Introduction

This repository serves as a reference project for using the PLY PostgreSQL Database connector Gem https://github.com/ash-j-f/PLY-public

For now, the intersting bit is the source code found in https://github.com/ash-j-f/CaelumMagnaVR-public/tree/master/Gem/Code which shows how to use various aspects of the PLY Gem.

**Until the star database creation and import process is documented, and the customised star data (over 80GB of data) is uploaded to a public location, the compiled project is not currently in a publicly usable state. This will be completed in the near future.**

The project will build and run, but without a star database to connect to, it will not display any stars.

For a preview of the star map software running using my locally hosted star database, check out a video of the prototype at https://youtu.be/VHa8dVGE2vs

Check out my portfolio at https://ajflynn.io/

## Supported Hardware

The Star Map runs on 64-bit Windows platforms (Windows 7 or above), and is compatible with Oculus VR headsets and controllers. Experimental support is also provided for OpenVR compatible headsets such as the HTC Vive family of VR headsets.

Minimum system specification (or equivalent): Intel i7 processor with at least 4 cores, 4GB RAM, Nvidia GTX1060 GPU.

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

## Installation (Star Map PostgreSQL Database)

### A Note on Security

**This Star Map database is intended to be accessible only at the address "localhost", on the same PC that the Star Map applictaion is running.** As such, the database, table and user setup instructions that follow use "easy" but low-security methods. You should ensure that your Windows firewall and other firewall devices prevent anyone uninvited from accessing the database server on your PC. Alternatively, you can configure the database with stronger security options (beyond the scope of this document), and choose your own CaelumMagnaVR database user password (rather than using the default). You will need to alter the username entry on the PLY Configuration Component in each of the project levels via the Lumberyard Editor. You will then need to re-compile the application.

### Database Installation Process

1. Install PostgreSQL on Windows using EnterpriseDB installer: https://www.enterprisedb.com/downloads/postgres-postgresql-downloads
    * Note: You may need to run the installer using option "--install_runtimes 0" if encountering a C++ runtime install error. See https://stackoverflow.com/questions/4288303/cant-install-postgresql-an-error-occurred-executing-the-microsoft-vc-runtime

1. Launch Stackbuilder when asked at the end of the PostgreSQL install. Find and install the PostGIS packacge listed under "Spatial Extensuions" in Stackbuilder.

1. Access the PostgreSQL server via pgAdmin (go to Windows > Run > and type "pgadmin" to find the app).
    This will open the pgAdmin tool at a localhost address in your browser.
    The server should be visible in the list on the left, and will require the password and username you chose in the PostgreSQL setup phase to access it.
    
1. Use pgadmin to create a new user in the database called "CaelumMagnaUser" with the password "CaelumMagnaUser".
    
1. Use pgadmin to create a new database on the PostgreSQL server and name it "CaelumMagnaVR". Set "CaelumMagnaUser" as the database owner.

1. Right click the "Extensions" subtree item undet the CaelumMagnaVR database name and choose "Create > Extension".

1. Repeat the above step to install each of the following PostGIS extensions:
    * plpgsql (may already be installed by default)
    * dblink
    * tsm_system_rows
    * postgis
    
1. Install Cygwin, which provides Linux-like commands on the command line for Windows such as Gzip https://www.cygwin.com/
    
1. Click the windows Start button and type "psql". Run the psql shell.

1. When prompted, enter "localhost" for the server name, "CaelumMagnaVR" for the database name, "5432" for the port, "CaelumMagnaVRUser" for the username, and "CaelumMagnaVRUser" as the password (if asked).

1. When logged into the psql shell, create the star map database table with the following SQL commands:
    ```
    CREATE TABLE public.gaia_main
    (
        source_id bigint NOT NULL,
        geom geometry,
        random_index integer,
        CONSTRAINT gaia_main_pkey PRIMARY KEY (source_id)
    ) PARTITION BY HASH (source_id) 
    WITH (
        OIDS = FALSE
    )
    TABLESPACE pg_default;
    
    CREATE INDEX gaia_main_geom_idx
        ON public.gaia_main USING gist
        (geom gist_geometry_ops_nd)
        TABLESPACE pg_default;

    CREATE TABLE public.gaia_main_0 PARTITION OF public.gaia_main
        FOR VALUES WITH (modulus 12, remainder 0);

    CREATE TABLE public.gaia_main_1 PARTITION OF public.gaia_main
        FOR VALUES WITH (modulus 12, remainder 1);

    CREATE TABLE public.gaia_main_10 PARTITION OF public.gaia_main
        FOR VALUES WITH (modulus 12, remainder 10);

    CREATE TABLE public.gaia_main_11 PARTITION OF public.gaia_main
        FOR VALUES WITH (modulus 12, remainder 11);

    CREATE TABLE public.gaia_main_2 PARTITION OF public.gaia_main
        FOR VALUES WITH (modulus 12, remainder 2);

    CREATE TABLE public.gaia_main_3 PARTITION OF public.gaia_main
        FOR VALUES WITH (modulus 12, remainder 3);

    CREATE TABLE public.gaia_main_4 PARTITION OF public.gaia_main
        FOR VALUES WITH (modulus 12, remainder 4);

    CREATE TABLE public.gaia_main_5 PARTITION OF public.gaia_main
        FOR VALUES WITH (modulus 12, remainder 5);

    CREATE TABLE public.gaia_main_6 PARTITION OF public.gaia_main
        FOR VALUES WITH (modulus 12, remainder 6);

    CREATE TABLE public.gaia_main_7 PARTITION OF public.gaia_main
        FOR VALUES WITH (modulus 12, remainder 7);

    CREATE TABLE public.gaia_main_8 PARTITION OF public.gaia_main
        FOR VALUES WITH (modulus 12, remainder 8);

    CREATE TABLE public.gaia_main_9 PARTITION OF public.gaia_main
        FOR VALUES WITH (modulus 12, remainder 9);
    ```

1. **TBC** Download the Caelum Magna Gaia Star Database file from XXXXXXXX. Note that this file is very large, at over 80GB.

1. Run the following command to import the star data (change the file paths to match the location of the Cygwin gzip command and the Caelum Magna Gaia star database file on your system). This process will take a long time.
    ```
    copy gaia_main (source_id, geom, random_index) from program 'C:\cygwin64\bin\gzip -dcq V:\gaia\caelum_magna_gaia_main.csv.gz' WITH (FORMAT CSV, DELIMITER ',', HEADER);
    ````

## Installation (Lumberyard and Star Map Project Files)

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
    * Note: To load a specific level file you need to use the `+map` option on the command line after the exe name, eg:
        * `CaelumMagnaVR.exe +map StarMapDemo`
        * `CaelumMagnaVR.exe +map SaveStateDemo`
	
## Credits

Created by Ashley Flynn https://ajflynn.io/ while studying a degree in software engineering at the Academy of Interactive Entertainment and the Canberra Institute of Technology in 2019.
	
Special thanks to Dr. Coryn Bailer-Jones for providing star survery distance estimate data http://www2.mpia-hd.mpg.de/homes/calj/gdr2_distances/main.html which was used in the Star Map demonstration project that implements the PLY Gem https://github.com/ash-j-f/CaelumMagnaVR-public.
	
And special thanks also go to Josh Beacham for assistance with Amazon Lumberyard code and setup.
	
## License

This project is licensed under the GNU General Public License v3.0. See the file "LICENSE" for more details.
