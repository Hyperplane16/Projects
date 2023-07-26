from game import Game

def test_game_1():
    """ POSITIVE: Testing if the filename is initiated correctly so the drawn
        board is consistent with the provided file.
    """
    game = Game("board_super_hard.txt")
    assert game.filename == "board_super_hard.txt", "test_game_1 failed"

""" No further unit tests that test individuality of functions exist.
    The remaining functions and methods involve integration testing with more
    than one other module. Therefore they are much more suited to e2e testing.
"""

def run_tests():
    test_game_1()
