from cells import ( 
    Start,
    End,
    Air,
    Wall,
    Fire,
    Water,
    Teleport
)

def test_cells_1():
    """ POSITIVE: Testing whether teleport displays are set correctly so their
        displays can be correctly called and drawn.
    """
    teleport = Teleport("1")
    assert teleport.display == "1", "test_cells 1 failed"
    
""" All further step functions involve integration testing with multiple modules
    combined, therefore they are not suited to unit testing but e2e.
"""

def run_tests():
    test_cells_1()
    