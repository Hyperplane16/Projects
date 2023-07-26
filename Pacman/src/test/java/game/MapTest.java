package game;

import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

import java.util.*;
import processing.core.PApplet;
import processing.core.PImage;

class MapTest {
    @Test 
    public void parseJsonFile() {
        // Test whether parseData() correctly sets the game object's attributes
        App appTest = new App();
        PApplet.runSketch(new String[]{""}, appTest);
        appTest.setup();
        Game gameTest = appTest.game;
        Map.parseData(gameTest, "src/test/resources/configTest.json");
        
        assertEquals(gameTest.mapfile, "mapTest.txt");
        assertEquals(gameTest.lives, 3);
        assertEquals(gameTest.speed, 1);
        assertEquals(gameTest.frightenedLength, 3);
        assertEquals(gameTest.invisibleLength, 7);
        assertArrayEquals(gameTest.modeLengths, new int[] {9, 20, 7, 20, 5, 20, 5, 1000});
        assertEquals(gameTest.modeIndex, 0);
    }

    @Test
    public void parseObjectsValid() {
        // Test whether parseObjects() correctly initalises game objects from the map file
        // Consider their object type, x and y coordinates
        App appTest = new App();
        PApplet.runSketch(new String[]{""}, appTest);
        appTest.setup();
        Game gameTest = appTest.game;
        gameTest.mapfile = "src/test/resources/mapTest.txt"; // Manually input test file
        Map.parseObject(appTest, gameTest);

        assertTrue(gameTest.waka instanceof Waka);
        assertEquals(gameTest.waka.x, 12);
        assertEquals(gameTest.waka.y, -4);
        assertTrue(gameTest.ghostList.get(0) instanceof Ambusher);
        assertEquals(gameTest.ghostList.get(0).x, 27);
        assertEquals(gameTest.ghostList.get(0).y, -5);
        assertTrue(gameTest.ghostList.get(1) instanceof Chaser);
        assertEquals(gameTest.ghostList.get(1).x, 43);
        assertEquals(gameTest.ghostList.get(1).y, -5);
        assertTrue(gameTest.ghostList.get(2) instanceof Ignorant);
        assertEquals(gameTest.ghostList.get(2).x, 59);
        assertEquals(gameTest.ghostList.get(2).y, -5);
        assertTrue(gameTest.ghostList.get(3) instanceof Whim);
        assertEquals(gameTest.ghostList.get(3).x, 75);
        assertEquals(gameTest.ghostList.get(3).y, -5);
        assertTrue(gameTest.wallList.get(0) instanceof Wall);
        assertEquals(gameTest.wallList.get(0).x, 96);
        assertEquals(gameTest.wallList.get(0).y, 0);
        assertTrue(gameTest.wallList.get(1) instanceof Wall);
        assertEquals(gameTest.wallList.get(1).x, 112);
        assertEquals(gameTest.wallList.get(1).y, 0);
        assertTrue(gameTest.wallList.get(2) instanceof Wall);
        assertEquals(gameTest.wallList.get(2).x, 128);
        assertEquals(gameTest.wallList.get(2).y, 0);
        assertTrue(gameTest.wallList.get(3) instanceof Wall);
        assertEquals(gameTest.wallList.get(3).x, 144);
        assertEquals(gameTest.wallList.get(3).y, 0);
        assertTrue(gameTest.wallList.get(4) instanceof Wall);
        assertEquals(gameTest.wallList.get(4).x, 160);
        assertEquals(gameTest.wallList.get(4).y, 0);
        assertTrue(gameTest.wallList.get(5) instanceof Wall);
        assertEquals(gameTest.wallList.get(5).x, 176);
        assertEquals(gameTest.wallList.get(5).y, 0);
        assertTrue(gameTest.consumableList.get(0) instanceof Fruit);
        assertEquals(gameTest.consumableList.get(0).x, 192);
        assertEquals(gameTest.consumableList.get(0).y, 0);
        assertTrue(gameTest.consumableList.get(1) instanceof SuperFruit);
        assertEquals(gameTest.consumableList.get(1).x, 204);
        assertEquals(gameTest.consumableList.get(1).y, -4);
        assertTrue(gameTest.consumableList.get(2) instanceof SodaCan);
        assertEquals(gameTest.consumableList.get(2).x, 220);
        assertEquals(gameTest.consumableList.get(2).y, -4);
    }
}