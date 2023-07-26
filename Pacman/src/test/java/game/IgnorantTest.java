package game;

import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

import java.util.*;
import processing.core.PImage;
import processing.core.PApplet;

class IgnorantTest {
    @Test 
    public void chaseModeTest() {
        // Testing default chase mode target of Waka's position as well as if Waka gets within 8 units
        App appTest = new App();
        PApplet.runSketch(new String[]{""}, appTest);
        appTest.setup();
        Game gameTest = appTest.game;
        gameTest.mode = "chase";
        gameTest.waka = new Waka(100, 100);
        gameTest.waka.setSpeed(-5, 0);
        Ignorant ignorantTest = new Ignorant(0, 0, null);
        // Default
        assertEquals(ignorantTest.target(gameTest), new ArrayList<Integer>(Arrays.asList(96, 96)));
        // Less than 8 units - target is bottom left
        ignorantTest = new Ignorant(80, 80, null);
        assertEquals(ignorantTest.target(gameTest), new ArrayList<Integer>(Arrays.asList(-80, 480)));
    }

    @Test
    public void scatterModeTest() {
        // Testing whether scatter mode target is bottom left corner
        App appTest = new App();
        PApplet.runSketch(new String[]{""}, appTest);
        appTest.setup();
        Game gameTest = appTest.game;
        gameTest.mode = "scatter";
        Ignorant ignorantTest = new Ignorant(100, 100, null);
        // Bottom left corner
        assertEquals(ignorantTest.target(gameTest), new ArrayList<Integer>(Arrays.asList(-96, 464)));
    }

    @Test
    public void unknownModeTest() {
        // Testing if the mode is unknown and Waka not within 8 units, then default target is scatter
        App appTest = new App();
        PApplet.runSketch(new String[]{""}, appTest);
        appTest.setup();
        Game gameTest = appTest.game;
        gameTest.mode = "unknown";
        gameTest.waka = new Waka(0, 0);
        Ignorant ignorantTest = new Ignorant(200, 200, null);
        // Bottom left corner
        assertEquals(ignorantTest.target(gameTest), new ArrayList<Integer>(Arrays.asList(-192, 368)));
    }
}