class Start:
    def __init__(self):
        self.display = "X"

    def step(self, game, move):
        pass

class End:
    def __init__(self):
        self.display = "Y"

    def step(self, game, move):
        game.state = "won"
        return "\n\nYou conquer the treacherous maze set up by the Fire Nation" \
               " and reclaim the Honourable Furious Forest Throne, restoring" \
               " your hometown back to its former glory of rainbow and sunshine!" \
               " Peace reigns over the lands.\n"

class Air:
    def __init__(self):
        self.display = " "

    def step(self, game, move):
        # If a hole exists in the outer wall, assuming the maze is a rectangle
        if game.player.row == 0 or game.player.col == 0 \
           or game.player.row == len(game.grid)-1 \
           or game.player.col == len(game.grid[0])-1:
               # Treat air as wall object            
               return Wall().step(game, move)
            
class Wall:
    def __init__(self):
        self.display = "*"

    def step(self, game, move):
        # Reverse all the movements if player steps onto a wall
        if move.upper() == "W":
            game.player.row += 1
        elif move.upper() == "A":
            game.player.col += 1
        elif move.upper() == "S":
            game.player.row -= 1
        elif move.upper() == "D":
            game.player.col -= 1

        # Remove latest move statistics
        game.number_of_moves -= 1
        game.moveslist.pop(-1)
               
        return "\nYou walked into a wall. Oof!\n"

class Fire:
    def __init__(self):
        self.display = "F"

    def step(self, game, move):
        if game.player.num_water_buckets >= 1:
            game.player.num_water_buckets -= 1
            game.grid[game.player.row][game.player.col] = Air()
            return "\nWith your strong acorn arms, you throw a water bucket at the" \
                   " fire. You acorn roll your way through the extinguished flames!\n" 
        else:
            game.state = "lost"
            return "\n\nYou step into the fires and watch your dreams disappear :(." \
"\n\nThe Fire Nation triumphs! The Honourable Furious Forest is reduced to a pile of" \
" ash and is scattered to the winds by the next storm... You have been roasted.\n"
        
class Water:
    def __init__(self):
        self.display = "W"

    def step(self, game, move):
        game.player.num_water_buckets += 1
        game.grid[game.player.row][game.player.col] = Air()
        return "\nThank the Honourable Furious Forest, you've found a bucket of water!\n" 

class Teleport:
    def __init__(self, display):
        self.display = display  

    def step(self, game, move):
        teleport_number = game.grid[game.player.row][game.player.col].display
        i = 0
        # Find the location of the matching teleport pad
        while i < len(game.grid):
            n = 0
            while n < len(game.grid[i]):
                if game.grid[i][n].display == teleport_number and \
                    (i != game.player.row or n!= game.player.col):
                    game.player.row = i
                    game.player.col = n
                    return "\nWhoosh! The magical gates break Physics as we know it" \
                           " and opens a wormhole through space and time.\n"
                n += 1
            i += 1
