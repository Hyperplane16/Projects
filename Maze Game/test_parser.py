from game_parser import parse, read_lines
from cells import ( # Added for output testing
    Start,
    End,
    Air,
    Wall,
    Fire,
    Water,
    Teleport
)

def test_parse_1():
    """ POSITIVE: Testing whether parse() can take an input and return a list
        of list of cells correctly
    """
    lines = ["*XY*\n"]
    assert type(parse(lines)) == list and \
           type(parse(lines)[0]) == list and \
           type(parse(lines)[0][0]) == Wall and \
           type(parse(lines)[0][1]) == Start and \
           type(parse(lines)[0][2]) == End and \
           type(parse(lines)[0][3]) == Wall, "test_parse_1 failed."

def test_parse_2():
    """ NEGATIVE: Testing whether a ValueError arises when invalidities
        exist, such as no X
    """
    lines = ["*****\n"]
    try:
        expected = "Expected 1 starting position, got 0."
        actual = parse(lines) 
    except ValueError as e:
        assert str(e) == expected, "test_parse_2 failed."

def test_parse_3():
    """ EDGE: Testing an string of size 1 with no newline and bad letter to
        test if parse() can handle both bad letters and the corner case of
        a string size of 1
    """
    try:
        expected = "Bad letter in configuration file: Q."
        actual = lines = ["Q"] 
    except ValueError as e:
        assert str(e) == expected, "test_parse_3 failed."

def test_parse_4():
    """ EDGE: Ensuring that a maze with incomplete outer walls can still be
        constructed with Air cells filling the spaces. (so it can treated as a
        wall in the game engine)
    """
    lines = ["*X\n", "*Y \n"]
    assert type(parse(lines)[0][0]) == Wall and \
           type(parse(lines)[0][1]) == Start and \
           type(parse(lines)[0][2]) == Air and \
           type(parse(lines)[1][0]) == Wall and \
           type(parse(lines)[1][1]) == End and \
           type(parse(lines)[1][2]) == Air, "test_parse_4 failed."
    
def test_read_lines_1():
    """ EDGE: Ensure no unexpected result occurs from inputting an empty string
        as the configuration file.
    """
    try:
        expected = "[Errno 2] No such file or directory: ''"
        read_lines("")
    except FileNotFoundError as e:
        assert str(e) == expected, "test_read_lines_1 failed."

def run_tests():
    test_parse_1()
    test_parse_2()
    test_parse_3()
    test_parse_4()
    test_read_lines_1()
