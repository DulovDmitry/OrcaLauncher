[![License: GPL](https://img.shields.io/badge/Licence-GNU%20GPL-blue)](https://www.gnu.org/licenses/gpl-3.0.txt)
![version](https://img.shields.io/badge/version-v1.4.0-ff8800)
[![author](https://img.shields.io/badge/Author-Dmitry%20Dulov-brightgreen)](mailto:dulov.dmitry@gmail.com)

# OrcaLauncher

OrcaLauncher is the program that simplifies your communication with ORCA quantum chemistry package. It is available **only for Windows platform** to date!
OrcaLauncher is an open-source project. It distributes under the GNU GPL license.

- [Features](#features)
- [User guide](#userguide)
- [Installation and start](#installation)
- [Screenshots](#screenshots)
- [Contacts](#contacts)

<a id="features"></a>
### Features

- No more need to run ORCA from the command line
- Easy interaction with input and output files
- Controllable queue for executing tasks
- Connectability with third-party programs (Chemcraft, Avogadro)
- It is convenient for small research groups as well as for single researchers

<a id="userguide"></a>
### User guide

Quick start tutorial and detailed guide with program screenshots are avaliable in the <a href="https://github.com/DulovDmitry/OrcaLauncher/blob/master/OrcaLauncher%20User%20Guide.pdf" target="_blank" rel="noopener noreferrer">OrcaLauncher User Guide</a>.

<a id="installation"></a>
### Installation and start

Download the OrcaLauncher installer ([install_orcalauncher_x64.exe](https://github.com/DulovDmitry/OrcaLauncher/blob/master/install_orcalauncher_x64.exe)) or archive ([OrcaLauncher.7z](https://github.com/DulovDmitry/OrcaLauncher/blob/master/OrcaLauncher.7z)). Install or extract it to your local disc.
Once you run *OrcaLauncher.exe*, you should specify the path to *orca.exe* on your computer (Settings > Set path to orca.exe). Do it only once at first OrcaLauncher run.
Add your ORCA input files by clicking “Add files”. Loaded files appear on the left-hand side of the program (*Input files queue preview*). If you want to correct some input file or just take a look at it, you should left-click on the filename in the table. The content of the input file appears on the right-hand side of the program. *If you change an input file, don’t forget to save the updates (“Save” button)!*

![F3.png](https://github.com/DulovDmitry/OrcaLauncher/blob/master/screenshots/F3.png "OrcaLauncher main window")

OrcaLauncher is ready to take off! Press “Run orca” button. The main window is hidden and the status window appears in the corner of the screen. Queued tasks will be started one by one following the initial order. Outfiles appear in the same directory as input files located and with the same base names.

![F4.png](https://github.com/DulovDmitry/OrcaLauncher/blob/master/screenshots/F4.png "Evolution of executing queue")

A task may become either “Completed” or “Aborted” depending on the result of ORCA calculations. You can open your outfile in the file explorer by right-click > “Show outfile in explorer”.

<a id="screenshots"></a>
### Screenshots

<p align="center">
<img src="https://github.com/DulovDmitry/OrcaLauncher/blob/master/screenshots/F3.png" alt="F3.png" />
</p>
<p align="center">
Main window
</p>

***

<p align="center">
<img src="https://github.com/DulovDmitry/OrcaLauncher/blob/master/screenshots/F9.png" alt="F9.png" width="350"/>
</p>
<p align="center">
Executing queue in the status window
</p>

***

<p align="center">
<img src="https://github.com/DulovDmitry/OrcaLauncher/blob/master/screenshots/F5.png" alt="F5.png" width="350"/>
</p>
<p align="center">
Context menu of the status window
</p>

***

<p align="center">
<img src="https://github.com/DulovDmitry/OrcaLauncher/blob/master/screenshots/F6.png" alt="F6.png" width="500"/>
</p>
<p align="center">
Templates manager
</p>

***

<a id="contacts"></a>
### Contacts

If you find a bug, please email me at [dulov.dmitry@gmail.com](mailto:dulov.dmitry@gmail.com). Feel free to submit your suggestions concerning new features and improvements.
