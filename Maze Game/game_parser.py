from cells import (
    Start,
    End,
    Air,
    Wall,
    Fire,
    Water,
    Teleport
)

# Form a list of strings, with each string representing a line from the file
def read_lines(filename):
    configuration = open(filename, "r")
    lines = configuration.readlines()
    return parse(lines)

def parse(lines):
    """ Transform the input into a grid.

    Arguments:
        lines -- list of strings representing the grid

    Returns:
        list -- contains list of lists of Cells
    """

    grid = []
    
    number_of_start_cells = 0
    number_of_end_cells = 0
    teleport_pad_bank = ["1","2","3","4","5","6","7","8","9"]
    teleport_pad_list = []

    """ Assuming the maze is a rectangle, open areas may exist in the config file,
        where some columns or rows have different lengths. 
        
        Therefore the goal is to find the maximum width of the maze so air
        blocks can be added to the previously undefined areas.

        DOCSTRING Continued.
    """

    # Strip newlines from each string
    i = 0
    while i < len(lines):
        lines[i] = lines[i].rstrip("\n")
        i += 1
    # Max width of the maze 
    maze_width = 0
    for s in lines:
        if len(s) > maze_width:
            maze_width = len(s)

    # Iterates through each string in the list of strings (lines)
    i = 0
    while i < len(lines):
        grid.append([])
        
        """ Continued.

            If a line is not same width as the longest line, spaces will be added 
            until their length reaches that width so air-blocks can be filled in
            later to form a rectangular maze
        """

        if len(lines[i]) < maze_width:
            t = 0
            difference = maze_width - len(lines[i])
            while t < difference:
                lines[i] += " "
                t += 1

        # Instantiate the respective objects to the list within grid
        n = 0
        while n < maze_width:
            if lines[i][n] == "X": # Start
                grid[i].append(Start())
                number_of_start_cells += 1
            elif lines[i][n] == "Y": # End
                grid[i].append(End())
                number_of_end_cells += 1
            elif lines[i][n] == " ": # Air
                grid[i].append(Air())
            elif lines[i][n] == "*": # Wall
                grid[i].append(Wall())
            elif lines[i][n] == "F": # Fire
                grid[i].append(Fire())
            elif lines[i][n] == "W": # Water
                grid[i].append(Water())
            elif lines[i][n] in teleport_pad_bank: # Teleport
                for t in teleport_pad_bank:
                    if lines[i][n] == t:
                        grid[i].append(Teleport(t))
                        teleport_pad_list.append(t)
                        break
            else: # Unknown Cell
                raise ValueError("Bad letter in configuration file: {}.". \
                      format(lines[i][n]))
            
            n += 1
        i += 1

    # Number of start or end cells
    if number_of_start_cells != 1:
        raise ValueError("Expected 1 starting position, got {}.". \
              format(number_of_start_cells))
    if number_of_end_cells != 1:
        raise ValueError("Expected 1 ending position, got {}.". \
              format(number_of_end_cells))

    # Exclusive pairings of teleport pads
    # For each teleport pad in the list, scan through all the teleport pads in
    # the list to see if exclusive pair exists
    for teleport_pad in teleport_pad_list:
        number_of_teleport_pads = 0
        for t in teleport_pad_list:
            if teleport_pad == t:
                number_of_teleport_pads += 1
        if number_of_teleport_pads != 2:
            raise ValueError("Teleport pad {} does not have an exclusively" \
                             " matching pad.".format(teleport_pad))

    return grid
