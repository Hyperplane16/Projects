from grid import grid_to_string, initialise_player
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

""" In some cases, testing has been done on unconventional or invalid grid
    elements such as the absence of start, end or unknown cells.

    The unit tests on grid aim to accurately convert the supplied grid into a
    string, even if invalidity came to pass, as the testing of parse() ensures
    that such states cannot reach grid.
"""

def test_grid_1():
    """ POSITIVE: Regular grid and player object with coordinates same as start
        location with no water buckets
    """
    grid = [[Wall(), Start(), Wall()], [Wall(), End(), Wall()]]
    player = Player(0, 1)
    assert grid_to_string(grid, player) == """*A*
*Y*

You have 0 water buckets.""", "test_grid_1 failed."


def test_grid_2():
    """ EDGE: If player coordinates were somehow negative, a runtime error
        should not arise.
    """
    grid = [[Wall(), Start(), Wall()], [Wall(), End(), Wall()]]
    player = Player(-1, -1)
    assert grid_to_string(grid, player) == """*X*
*Y*

You have 0 water buckets.""", "test_grid_2 failed."


def test_grid_3():
    """ EDGE: If grid is not rectangular or has walls missing, a board should
        still be able to be formed so the outer edge of air cells can be
        treated as walls (in the game engine)
    """
    grid = [[Wall(), Start()], [Wall(), End(), Wall()]]
    player = Player(0, 1)
    assert grid_to_string(grid, player) == """*A
*Y*

You have 0 water buckets.""", "test_grid_3 failed."


def test_grid_4():
    """ EDGE: If grid has length of 1 and somehow lacked a start or end cell,
        a runtime error should not arise.
    """
    grid = [[Wall()]]
    player = Player(0, 1)
    assert grid_to_string(grid, player) == """*

You have 0 water buckets.""", "test_grid_4 failed."


def test_grid_5():
    """ NEGATIVE: If input grid has an invalid cell, it should not try to
        form a board but raise an error.
    """
    try:
        expected = "name 'Jungle' is not defined"
        grid = [[Wall(), Jungle(), Wall()]]
    except NameError as e:
        assert str(e) == expected, "test_grid_5 failed"


def test_initialise_player_1():
    """ POSITIVE: Regular grid with one start to test normal functionality
    """
    grid = [[Wall(), Start(), Wall()]]
    output = initialise_player(grid)
    assert output.row == 0 and output.col == 1, \
    "test_initialise_player_1 failed"


def test_initialise_player_2():
    """ EDGE: If there is somehow more than one start, a runtime error should
        not arise.
    """
    grid = [[Wall(), Start(), Start()]]
    output = initialise_player(grid)
    assert output.row == 0 and output.col == 1, \
    "test_initialise_player_2 failed"


def run_tests():
    test_grid_1()
    test_grid_2()
    test_grid_3()
    test_grid_4()
    test_grid_5()

    test_initialise_player_1()
    test_initialise_player_2()
