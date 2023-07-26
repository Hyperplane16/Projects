from game_parser import read_lines
from grid import grid_to_string, initialise_player # Added Second
from cells import Wall # Added, for special detection of leaving the maze

# -----------------------------------------------------------------------------

class Game:
    def __init__(self, filename):
        self.filename = filename
        # Initialise Grid and Player
        self.grid = read_lines(filename)
        self.player = initialise_player(self.grid)

        # Move Statistics
        self.number_of_moves = 0
        self.moveslist = []
        
        # State of the Game
        # [running (does nothing), lost (due to fire), won (reached end)]
        self.state = "running"

    def game_display(self):
        return grid_to_string(self.grid, self.player)

    def game_move(self, move, game):
        """ Takes a keypress input and the game object to update the
            player position, then activates any cell related activity.

            If messages exist, only one will be returned in the following order:
            - If the keypress input is not valid
            - Cell related messages
        """

        # Apply movement to the player object
        move_action = self.player.move(move)
        # Return error in movement commands if exists (defined in Player.py)
        if move_action != None:
            return move_action
        
        # If the movement command is valid, add move statistics 
        self.number_of_moves += 1
        self.moveslist.append(move.lower())
        # If we walk into a wall or out of bounds, then the latest
        # statistic will be removed within cells.py, by cell_action)
        
        current_player_cell = self.grid[self.player.row][self.player.col]
        # If the player leaves the confines of the maze entirely
        if self.player.row < 0 or self.player.col < 0:
            current_player_cell = Wall()

        # Apply action of the cell currently occupied by the player
        cell_action = current_player_cell.step(game, move)
        # Return cell message if exists (defined in cells.py)
        if cell_action != None:
            return cell_action
        
        return ""
