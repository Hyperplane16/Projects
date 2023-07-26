from game import Game
import os
import sys

# -----------------------------------------------------------------------------

# If the command line argument is not valid
if len(sys.argv) == 1:
    print("Usage: python3 run.py <filename> [play]")
    sys.exit()

try: # If file is not found, print an error message, otherwise initiate Game object
    game = Game(sys.argv[1])
except FileNotFoundError:
    print("{} does not exist!".format(sys.argv[1]))
    sys.exit()

message = ""

# ----------------------------------------------------------------------------

while True:
    # If "play" argument is given, refresh the screen every move
    if len(sys.argv) == 3 and sys.argv[2] == "play":
        os.system('clear')

    # Print the board and any messages (defined below)
    print(game.game_display())
    print(message)

    # If the game has been won or lost
    if game.state == "won" or game.state == "lost":
        # Correct punctuation of ending message
        comma_delimited = ", ".join(game.moveslist)
        if game.number_of_moves == 1:
            print("You made {} move.".format(game.number_of_moves))
            print("Your move: {}".format(comma_delimited))
        else:
            print("You made {} moves.".format(game.number_of_moves))
            print("Your moves: {}".format(comma_delimited))

        if game.state == "won":
            print("""
=====================
====== YOU WIN! =====
=====================""")
        elif game.state == "lost":
            print("""
=====================
===== GAME OVER =====
=====================""")
        sys.exit()

    # Player Input
    command = input("Input a move: ")
    message = game.game_move(command, game)

    # Special Exit
    if command.upper() == "Q":
        print("\nBye!")
        sys.exit()
