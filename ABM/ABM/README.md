# ABM

## File purposes

main.cpp kicks off Qt GUI and starts a number of threads based on how many were defined in Agents.db. Threads are all sent to Bucket.cpp.

Bucket.cpp contains every agent (thread) process, and therefore is the backbone of the entire simulation.

MainWindow.cpp is the master Qt window. It displays all agent power production and consumption, battery charge, as well as simulation time to completion. All other windows can be opened from it.

Graph.cpp displays a graph at the end of the simulation of all power consumptions and productions for each agent type with respect to time. Legend may be clicked to hide/show graphs.

Instructions.cpp contains the window that displays instructions for the user to use the program. Does not include build instructions, as optimally the user wouldn't need to build the project.

sqlite3.c and sqlite3.h were used to interact with the SQLite database. I did not create them, they are publically available libraries. They were just in this repo so I could easily set up this project on a new machine, as I was moving between machines a lot in final year.
