package game;

import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

import java.util.*;
import processing.core.PImage;
import processing.core.PApplet;

class ChaserTest {
    @Test 
    public void chaseModeTest() {
        // Testing chase mode Waka's current position
        App appTest = new App();
        PApplet.runSketch(new String[]{""}, appTest);
        appTest.setup();
        Game gameTest = appTest.game;
        gameTest.mode = "chase";
        gameTest.waka = new Waka(100, 100);
        gameTest.waka.setSpeed(-5, 0);
        Chaser chaserTest = new Chaser(0, 0, null);
        // Waka's position
        assertEquals(chaserTest.target(gameTest), new ArrayList<Integer>(Arrays.asList(96, 96)));
    }

    @Test
    public void scatterModeTest() {
        // Testing whether scatter mode target is top left corner
        App appTest = new App();
        PApplet.runSketch(new String[]{""}, appTest);
        appTest.setup();
        Game gameTest = appTest.game;
        gameTest.mode = "scatter";
        Chaser chaserTest = new Chaser(100, 100, null);
        // Top left corner
        assertEquals(chaserTest.target(gameTest), new ArrayList<Integer>(Arrays.asList(-96, -96)));
    }

    @Test
    public void unknownModeTest() {
        // Testing if the mode is unknown, then default target is the current position
        App appTest = new App();
        PApplet.runSketch(new String[]{""}, appTest);
        appTest.setup();
        Game gameTest = appTest.game;
        gameTest.mode = "unknown";
        Chaser chaserTest = new Chaser(100, 100, null);
        assertEquals(chaserTest.target(gameTest), new ArrayList<Integer>(Arrays.asList(0, 0)));
    }
}