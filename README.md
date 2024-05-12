MCPE Prune World Script v3.0 x64 (64Bit)
=========================================
Prune Chunks from Minecraft Bedrock Edition Worlds

Update 3.0 The GUI Update
=========================
+ Added a GUI for the Prune Tool
-> This removes command line functionality in favour of more flexibility
+ Added world chooser to pick from your installed Minecraft worlds
+ Added world browser to search for a specific Minecraft world
+ Added Backup System to make a full backup of the world before running
+ Added Options to run all dimensions at the same time
+ Added an option to optimize the world after pruning has finished

Update 2.5
==========
+ Fixed command line coordinates not converting to chunk coordinates

Update 2.4 [The really really fast update!]
===========================================
+ Note: Command Line & Script Changes in this version
+ New: runAll Bat now processes all dimensions at the same time!
+ Options File now affects other .bat files
+ Rewrite of Actor (entity) detection again
+ Fixed a few bugs
+ Made the system even faster

Update 2.3
==========
+ Rewrite of Actor (entity) detection again
+ Fixed the lost actors issue in version 2.2

Update 2.2
==========
+ Rewrite of Actor (entity) detection to make process much faster
+ Added Repair Database at the beginning of the process in case of any corruption
+ Added system to find lost entities / lost references to entities and remove them

Known Issues in 2.2
===================
Actor Key Deletion
- When lost actors (entities) are deleted, they show succesfully deleted, but then some reappear on the next scan.
- Sometimes it will remove a few, sometimes it won't remove any.

Fixes in 2.0
============
- Added DB Optimization (Optional)
- Updated to work with Minecraft Bedrock 1.18.31 and above.
This will make the process a little slower due to entities (actors) being stored separatley in the world database.

WARNING:
========
This app deletes data from your world database!
MAKE A BACKUP BEFORE RUNNING!!! - The app will automatically create a backup for you when you launch (if you want it to), however always make a backup yourself, just in case!
Once you've run the app check your world for any signs of problems.
Also check your command blocks if you have any as these may also freeze after running the script.

About:
======
This tool is for Minecraft Bedrock Edition and has been tested on 1.18.31 - 1.19 worlds.
Other versions may not work so make a backup first.
This tool is written in C++ and runs on Windows 64bit.


What this app does:
======================
This app deletes all chunk data outside of the coordinates that you enter.
It also deletes any entities as well as portal data found outside of those chunks as that data is stored separately from the chunk data.

You can choose to prune any or all dimensions and what coordinates you want to preserve the data for each dimension accordingly.
