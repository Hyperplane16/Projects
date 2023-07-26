from cells import (
    Start,
    End,
    Air,
    Wall,
    Fire,
    Water,
    Teleport
)
from player import Player

def grid_to_string(grid, player):
    """Turns a grid and player into a string

    Arguments:
        grid -- list of list of Cells
        player -- a Player with water buckets

    Returns:
        string: A string representation of the grid and player.
    """

    grid_display = ""
    
    i = 0
    while i < len(grid):
        n = 0
        while n < len(grid[i]):
            # Add each cell's corresponding display
            if i == player.row and n == player.col:
                grid_display += player.display
            else:
                grid_display += str(grid[i][n].display)
            if n == len(grid[i]) - 1:
                grid_display += "\n"            
            n += 1
        i += 1

    grid_display += "\n"

    if player.num_water_buckets == 1:
        grid_display += "You have 1 water bucket."
    else:
        grid_display += "You have {} water buckets.".format(player.num_water_buckets)
    
    return grid_display

def initialise_player(grid):
    # Finds the start object in the grid and initialises player object
    i = 0
    while i < len(grid):
        n = 0
        while n < len(grid[i]):
            if isinstance(grid[i][n], Start) == True:
                return Player(i, n)
            n += 1
        i += 1
