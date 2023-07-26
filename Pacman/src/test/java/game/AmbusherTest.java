package game;

import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

import java.util.*;
import processing.core.PImage;
import processing.core.PApplet;

class AmbusherTest {
    @Test 
    public void chaseModeTest() {
        // Testing Ambusher's chase mode target is 4 cells ahead of horiztonal and vertical Waka movement 
        App appTest = new App();
        PApplet.runSketch(new String[]{""}, appTest);
        appTest.setup();
        Game gameTest = appTest.game;
        gameTest.mode = "chase";
        gameTest.waka = new Waka(100, 100);
        gameTest.waka.setSpeed(-5, 0);
        Ambusher ambusherTest = new Ambusher(0, 0, null);
        // Horizontal
        assertEquals(ambusherTest.target(gameTest), new ArrayList<Integer>(Arrays.asList(32, 96)));
        // Vertical
        gameTest.waka.setSpeed(0, 5);
        assertEquals(ambusherTest.target(gameTest), new ArrayList<Integer>(Arrays.asList(96, 160)));
    }

    @Test
    public void scatterModeTest() {
        // Testing whether scatter mode target is top right corner
        App appTest = new App();
        PApplet.runSketch(new String[]{""}, appTest);
        appTest.setup();
        Game gameTest = appTest.game;
        gameTest.mode = "scatter";
        Ambusher ambusherTest = new Ambusher(100, 100, null);
        // Top right corner
        assertEquals(ambusherTest.target(gameTest), new ArrayList<Integer>(Arrays.asList(336, -96)));
    }

    @Test
    public void unknownModeTest() {
        // Testing if the mode is unknown, then default target is the current position
        App appTest = new App();
        PApplet.runSketch(new String[]{""}, appTest);
        appTest.setup();
        Game gameTest = appTest.game;
        gameTest.mode = "unknown";
        Ambusher ambusherTest = new Ambusher(100, 100, null);
        assertEquals(ambusherTest.target(gameTest), new ArrayList<Integer>(Arrays.asList(0, 0)));
    }
}