class Player:
    def __init__(self, row, col):
        self.display = "A"
        self.num_water_buckets = 0
        self.row = row
        self.col = col

    def move(self, move):
        # Update player object positions depending on the move
        if move.upper() == "W":
            self.row -= 1
        elif move.upper() == "A":
            self.col -= 1
        elif move.upper() == "S":
            self.row += 1
        elif move.upper() == "D":
            self.col += 1
        elif move.upper() == "E":
            pass
    
        # If no valid move is given
        else:
            return "\nPlease enter a valid move (w, a, s, d, e, q).\n"
