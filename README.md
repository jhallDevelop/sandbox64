# sandbox64
Sandbox area to run experiments using libdragon for n64 game development  
FEATURES:  
 - 3D rendering
 - Basic Rigidbody Collisions and Physics
 - Sound
 - Font
[![Project Screenshot](./video/gfs_gamejam_2024/10_oldGods64_010.png)](./video/gfs_gamejam_2024/10_oldGods64_830am.mov)  


## Installation Instructions  
1\. Installing libdragon and dependencies
=========================================

First step is to install some dependencies:

-   Docker desktop

-   Node

-   Git

(1) Install Docker Desktop via winget:

winget install docker.dockerdesktop

or get from <https://www.docker.com/products/docker-desktop> (direct link to Windows/ARM64: <https://desktop.docker.com/win/main/amd64/Docker%20Desktop%20Installer.exe>). Launch Docker Desktop and sign in/click through the initial dialogs

(2) Install fnm (Fast Node Manager) from Powershell:

winget install Schniz.fnm

(3) Create or edit $profile in powershell

To get path to $profile file, run:

$profile

# or if already exists, to edit:
notepad $profile

Add the following to it:

fnm env --use-on-cd | Out-String | Invoke-Expression

(4) Install node:

# download and install Node.js
fnm use --install-if-missing 22

# use these to verify node is installed correctly (will print version numbers)
node -v
npm -v

(5) Install git:

winget install --id Git.Git -e --source winget

(6) Install libdragon:

npm i -g libdragon@latest

2\. Setting up a project
========================

(1) Create a folder for project (e.g. in $HOME\Documents create libdragon-src or similar). *cd* into it and run *libdragon init, libdragon install*:

# create the project folder and cd into it
cd $HOME\Documents
mkdir libdragon-src
cd libdragon-src

# this creates a new docker container, initialises it and mounts the libdragon directory from the docker image to the current folder
libdragon init

# then, to switch to preview branch (or to update)
git -C ./libdragon checkout preview
libdragon install

N.B. I use the preview branch because this contains OpenGL, while the regular branch is 2D only.

(2) To start / stop the container/environment from the project folder:

libdragon start
libdragon stop

To access the Linux shell inside your docker container, either use Docker Desktop, click on 'Containers', select the running container and choose the 'Exec' tab, or from powershell, run:

docker exec -it [container-id] bash

3\. Set up vscode
=================

(1) Install vscode:

winget install -e --id Microsoft.VisualStudioCode

or the old-fashioned way, from <https://code.visualstudio.com/>.

(2) Open vscode and install the extensions:

-   C/C++

-   C/C++ Extension Pack

-   Dev Containers

(others might be desirable, but these should be the basics)

(3) Make sure that the container/environment is running (e.g. via *libdragon start* from project folder - can check running state from Docker Desktop), then in vscode, press F1 and type:

>Dev Containers: Attach to Running Container...

Press Enter. Then select the name of the container (should match the container name shown in Docker Desktop)

(4) The C/C++ extension now needs to be configured for libdragon includes to be recognised properly. Press F1 and type:

>C/C++: Edit Configurations (UI)

There should be a 'Linux' configuration already set up but it will be missing a couple of things:

-   Under 'Compiler path' type:

    /n64_toolchain/bin/mips64-elf-g++

-   Under 'Include path' add:

    ${workspaceFolder}/libdragon/include/

Includes should now work - test by opening an example c file in the libdragon/examples folder and right-clicking one of the includes and clicking 'Go to Definition'. If there are red squiggles all over the place, it is probably not set up correctly.

4\. Setting up local documentation (Doxygen)
============================================

(1) From the running Docker container in Docker Desktop, click on Exec to get to a Linux prompt in the container and install doxygen and graphviz:

apt-get install doxygen graphviz

*(2) cd* to the top level of the libdragon environment/cloned repo where there should be a doxygen-public.conf and run doxygen to generate the docs:

cd /libdragon/libdragon
doxygen doxygen-public.conf

Some warnings might appear, but it should ultimately generate the docs. By default it should create the docs in the ./website/ref subdirectory - this should also be accessible from Windows. Open index.html when it's done to browse the offline documentation.

5\. 'Hello world' test program
==============================

(1) Get hold of an appropriate emulator to test roms, [ares](https://ares-emu.net/download) (multiplatform) is recommended - main requirement is that it's sufficiently low-level as HLE-based emulators will probably fall over on code compiled with libdragon

(2) In the root folder of the libdragon project (/libdragon/, one level up from the folder where the doxygen-public.conf is), create a new folder for your hello world ROM (e.g. hello-world)

(3) In the libdragon root folder, there is a Makefile, and a subfolder called 'src' - copy both of these to the hello-world folder

(4) Inside hello-world, make an additional subfolder called build (both build and src are referenced in the Makefile)

(5) From a Docker terminal, inside the hello-world folder, run:

make

or from a Windows terminal, inside the hello-world folder, run:

libdragon make

Some compiler output should be shown and then a hello.z64 file (the ROM) should appear in the same folder. The build folder will also contain a number of files produced during compilation.

Open this in your emulator of choice - in Ares, you can drag the .z64 file into the main window, where you should see 'Hello, world!' printed white-on-black at the top-left of the window.

6\. Debug output via the ares terminal
======================================

Current versions of ares (v134+) support simple debugging (debug output to console) via the ISViewer debug channel. ISViewer here refers to the 'IS-Viewer 64', a development cartridge that was used for production N64 development and connected to a host system via SCSI which could then read the debug output through a console. Several emulators including ares support the ISViewer debug channel, and it can be enabled in libdragon for this purpose.

(1) Open cmd or Powershell. cd to the path where Ares is located and launch ares with the --terminal flag:

ares.exe --terminal

Ares should open as normal but an additional terminal window should also appear

(2) Click Settings > Options

(3) Tick to enable Homebrew Development Mode (might not be necessary on the latest versions of Ares but originally this was supposed to be used to enable the ISViewer channel

(4) Tweak your Libdragon code as follows to enable the ISViewer debug channel and add some test output so we can check if this is working - here is an example that modifies the hello-world main.c file with a couple of additional lines to enable the debug output:

#include <stdio.h>

#include <libdragon.h>

int main(void)
{
    debug_init_isviewer(); // this enables the ISViewer debug channel

    console_init();

    debug_init_usblog();
    console_set_debug(true);

    printf("Hello world!\n");
    debugf("Test\n"); // this macro can be used like printf to write to the debug channel

    while(1) {}
}

(5) Compile the code using make / libdragon make

(6) Open the ROM (drag the .z64 file into the Ares window) - if all goes well, the line "Test" should appear at the bottom of the Ares terminal window, indicating that the debug output is working correctly

(7) The ISViewer feature can be switched on and off from menu item Tools > Tracer in Ares - there should be a 'Log to Terminal' column ticked against the item called 'Cartridge ISViewer'
