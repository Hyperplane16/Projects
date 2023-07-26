# sys.setrecursionlimit(100000)

""" Current Issues
    - Solver can successfully find paths of all valid, rectangular boards.
    - However, there is an overemphasis on water collection in that if water is collected,
      the path taken till that point will always be stored regardless if water is necessary
      to win the game.
    - Though this is an optimisation and efficiency issue.
"""

from game import Game
from cells import (
    Start,
    End,
    Air,
    Wall,
    Fire,
    Water,
    Teleport
)
import sys

# -----------------------------------------------------------------------------

# Function that performs a deepcopy by creating a new game object and performing 
# the moves up to the current state of the game object automatically.
def clone_game(obj, filename):
    new_game = Game(filename)
    i = 0
    while i < len(obj.moveslist):
        new_game.game_move(obj.moveslist[i], new_game)
        i += 1
    return new_game

def solve(mode, node):    
    mode = mode.upper()

    """ The technical difference between DFS and BFS is that in BFS,
        - The current node is taken as the first and not the last
        - The first node is then removed from the list as opposed to the last
        - The priority of nodes follows the actual pattern and not inverse like DFS
    """
    
    # Initiate 
    visited = [[node.player.row, node.player.col]]
    nodes_list = [node]
    wait_count = 0

    while True:
        # If the ending is reached
        if node.state == "won":
            return [True, node.number_of_moves, node.moveslist]
        # If there are no entries in nodes_list
        if len(nodes_list) == 0:
            return [False]

        """ Changing queuing and node adding order for BFS and DFS """
        if mode == "DFS":
            # node = last node in nodes_list
            node = nodes_list[-1]
            nodes_list.pop(-1)
            
            ordering = [1, 2, 3, 4, 5]

        elif mode == "BFS":
            # node = first node in node_list
            node = nodes_list[0]
            nodes_list.pop(0)
            
            ordering = [5, 4, 3, 2, 1]
    
        """ The game object will be tested for the cells around it. If they are
            not a wall, the corresponding keypress move will be made and a new
            game instance will be appended.

            The adding of nodes is done within a for loop. The order of execution
            is defined in the docstring above inside "ordering"

            A special method is applied if fire is encountered. DOC-STRING continued.
        """

        for order in ordering:
            if order == 1:
                """ Only if the cell encountered is a teleport pad can the wait ability "E"
                    be applied.

                    There is a set limit of three sequential E moves for the solver.
                """

                # Wait ability for teleport pad nodes
                if type(node.grid[node.player.row][node.player.col]) == Teleport:
                    if wait_count <= 3:
                        wait_count += 1
                        new = clone_game(node, sys.argv[1])
                
                        new.game_move("E", new)
                        visited.append([new.player.row, new.player.col])
                        nodes_list.append(new)
                    else:
                        wait_count = 0

            if order == 2:
                # If node has a left node
                if node.player.col != 0: # Prevent out of bounds

                    """ Continued.
                    
                        If the node to a direction is NOT a wall or fire, it will always be added.

                        If it is a fire, it will only be added if the number 
                        of water buckets is higher than 0.

                        DOC-STRING continued for water.
                    """
                    
                    if type(node.grid[node.player.row][node.player.col - 1]) != Wall \
                       and (type(node.grid[node.player.row][node.player.col - 1]) !=  Fire \
                       or node.player.num_water_buckets > 0):
                        
                        if [node.player.row, node.player.col - 1] not in visited:
                
                            new = clone_game(node, sys.argv[1])
                
                            new.game_move("A", new)
                            visited.append([new.player.row, new.player.col])
                            nodes_list.append(new)

                            """ Continued.
                                
                                If the movement occurs over a water bucket, remove
                                all entries in nodes_list and visited except for the most
                                recent game object (the one where the player reaches the water)

                                Then break to avoid a pass by reference logic error as the
                                current "node" would be pointed toward the new added game
                                object.

                                Reason is that if the surrounding cell testing continued 
                                without a break, further moves may miss the water bucket.
                            """

                            # If a water bucket is picked up, remove all entires except for
                            # recently added game instance and break.
                            if type(node.grid[node.player.row][node.player.col - 1]) == Water:
                                visited = visited[-1:]
                                nodes_list = nodes_list[-1:]
                                wait_count = 0
                                break

            """ Note: The code for testing surrounding cells is identical except for the
                side that is tested. (Therefore explanation texts is omitted below)
            """

            if order == 3:    
                # If node has a up node
                if node.player.row != 0: # Prevent out of bounds
                    if type(node.grid[node.player.row - 1][node.player.col]) != Wall \
                       and (type(node.grid[node.player.row - 1][node.player.col]) !=  Fire \
                       or node.player.num_water_buckets > 0):
                        
                        if [node.player.row - 1, node.player.col] not in visited:
                
                            new = clone_game(node, sys.argv[1])
                
                            new.game_move("W", new)
                            visited.append([new.player.row, new.player.col])
                            nodes_list.append(new)

                            if type(node.grid[node.player.row - 1][node.player.col]) == Water:
                                visited = visited[-1:]
                                nodes_list = nodes_list[-1:]
                                wait_count = 0
                                break

            if order == 4:
                # If node has a right node
                if node.player.col != len(game.grid[0])-1: # Prevent out of bounds
                    if type(node.grid[node.player.row][node.player.col + 1]) != Wall \
                       and (type(node.grid[node.player.row][node.player.col + 1]) !=  Fire \
                       or node.player.num_water_buckets > 0):

                        if [node.player.row, node.player.col + 1] not in visited:

                            new = clone_game(node, sys.argv[1])
                
                            new.game_move("D", new)
                            visited.append([new.player.row, new.player.col])
                            nodes_list.append(new)

                            if type(node.grid[node.player.row][node.player.col + 1]) == Water:
                                visited = visited[-1:]
                                nodes_list = nodes_list[-1:]
                                wait_count = 0
                                break

            if order == 5:
                # If node has a down node
                if node.player.row != len(node.grid)-1: # Prevent out of bounds
                    if type(node.grid[node.player.row + 1][node.player.col]) != Wall \
                    and (type(node.grid[node.player.row + 1][node.player.col]) !=  Fire \
                       or node.player.num_water_buckets > 0):

                        if [node.player.row + 1, node.player.col] not in visited:

                            new = clone_game(node, sys.argv[1])
                            
                            new.game_move("S", new)
                            visited.append([new.player.row, new.player.col])
                            nodes_list.append(new)

                            if type(node.grid[node.player.row + 1][node.player.col]) == Water:
                                visited = visited[-1:]
                                nodes_list = nodes_list[-1:]
                                wait_count = 0
                                break
                            
# -----------------------------------------------------------------------------

if __name__ == "__main__":
    solution_found = False

    # If command-line argument is not valid
    if len(sys.argv) == 1 or len(sys.argv) == 2:
        print("Usage: python3 solver.py <filename> <mode>")
        sys.exit()

    # If file is not found, print an error message
    try:
        game = Game(sys.argv[1])
    except FileNotFoundError:
        print("{} does not exist!".format(sys.argv[1]))
        sys.exit()

    # Returns a list in the order: [True/False, List of Moves, Number of Moves]
    solution_found = solve(sys.argv[2], game)
    
    if solution_found[0] == True:
        comma_delimited = ", ".join(solution_found[2])
        print("Path has {} moves.".format(solution_found[1]))
        print("Path: {}".format(comma_delimited))   

    else:
        print("There is no possible path.")
