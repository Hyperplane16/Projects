from player import Player 

def test_player_1():
    """ POSITIVE: Regular movement command testing if update works correctly
    """
    player = Player(0,0)
    player.move("S")
    assert player.row == 1 and player.col == 0, "test_player_1 failed."

def test_player_2():
    """ NEGATIVE: Testing the correct behaviour in response to an invalid
        movement keypress, T
    """
    player = Player(0,0)
    assert player.move("T") == "\nPlease enter a valid move" \
           " (w, a, s, d, e, q).\n", "test_player_2 failed."

def test_player_3():
    """ EDGE: Ensuring input of more than one valid key at the same time still
        returns the correct error message
    """
    player = Player(0,0)
    assert player.move("W A") == "\nPlease enter a valid move" \
           " (w, a, s, d, e, q).\n", "test_player_3 failed."   

def run_tests():
    test_player_1()
    test_player_2()
    test_player_3()
