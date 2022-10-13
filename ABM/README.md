# ABM

## Project

A simulation of a smart electrical grid using agents. Agents are components of the grid that either generate or consume power. Each agent runs on its own thread. Agents are defined in the SQLite database to be loaded in by the program. The model keeps time using a tick system. Upon the program's initialisation, it queries the user to input a time for the simulation to run in ticks. The program then runs the simulation and displays to the user how much power each agent is consuming or producing, how much the batteries have charged, and the time left in the simulation. After the simulation completes, the user may open a graph to visualise the data of how each agent behaved during each tick. This data can be exported to a CSV file for further analysis.

Demonstration video: https://www.youtube.com/watch?v=_uv7WGVuGm8