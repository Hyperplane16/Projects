package game;

import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

import processing.core.PImage;

// Have a class extend the abstract GameObject to test
class GOTest extends GameObject {
    public GOTest(int x, int y, PImage sprite) {
        super(x, y, sprite);
    }
}

class GameObjectTest {
    @Test
    public void getCoordinates() {
        // Test whether getCx() and getCy() correctly obtains coordinates
        GOTest goTest = new GOTest(100, 100, null);
        assertEquals(goTest.getCx(0), 96);
        assertEquals(goTest.getCy(0), 96);
    }

    @Test
    public void getCoordinatesOffset() {
        // Test whether methods work with an offset
        GOTest goTest = new GOTest(100, 100, null);
        assertEquals(goTest.getCx(5), 96);
        assertEquals(goTest.getCy(-5), 80);
    }

    @Test
    public void isCentreTest() {
        // Test whether isCentre() correctly detects when object is in the centre of a 16x16 cell
        GOTest goTest = new GOTest(16, 100, null); // x true, y false
        assertFalse(goTest.isCentre(0)); 
        goTest = new GOTest(96, 96, null); // Both true
        assertTrue(goTest.isCentre(0));
        goTest = new GOTest(96, 96, null); // Both false
        assertFalse(goTest.isCentre(5));
        goTest = new GOTest(11, 11, null); // Both True
        assertTrue(goTest.isCentre(5));
        goTest = new GOTest(96, 11, null); // x false, y true
        assertFalse(goTest.isCentre(5));
    }
}