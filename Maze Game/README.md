# Maze Game

A maze game written in Python with gameplay elements involving water buckets, 
fire obstacles and teleport pads given by a cell configuration grid. The player, 
an Acorn character, can use movement keys to navigate the map with the aim of 
victory by reaching an ending cell. 

The code operates a Game object to store the current game state and to call
upon cell specific actions. There is also a solving functionality, solver.py,
that uses DFS or BFS to automatically search and output a valid path to victory.

A set of functional unit and end-to-end (E2E) tests have been created and can be run
using test_e2e.sh.