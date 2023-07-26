package game;

import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

import java.util.*;
import processing.core.PImage;
import processing.core.PApplet;

class WhimTest {
    @Test 
    public void chaseModeTest1() {
        // Testing default chase mode with one chaser
        App appTest = new App();
        PApplet.runSketch(new String[]{""}, appTest);
        appTest.setup();
        Game gameTest = appTest.game;
        gameTest.mode = "chase";
        gameTest.waka = new Waka(100, 100);
        gameTest.waka.setSpeed(5, 0);
        Chaser chaserTest = new Chaser(0, 0, null);
        gameTest.ghostList = new ArrayList<Ghost>();
        gameTest.ghostList.add(chaserTest);
        Whim whimTest = new Whim(200, 0, null);
        // chaserTest would be [256, 192]
        assertEquals(whimTest.target(gameTest), new ArrayList<Integer>(Arrays.asList(64, 192)));
    }

    @Test
    public void chaseModeTest2() {
        // Testing that target is Waka's position where there are no chasers and they are dead
        App appTest = new App();
        PApplet.runSketch(new String[]{""}, appTest);
        appTest.setup();
        Game gameTest = appTest.game;
        gameTest.mode = "chase";
        gameTest.waka = new Waka(100, 100);
        gameTest.waka.setSpeed(5, 0);
        gameTest.ghostList = new ArrayList<Ghost>();
        Ambusher ambusherTest = new Ambusher(0, 0, null);
        gameTest.ghostList.add(ambusherTest);
        Whim whimTest = new Whim(200, 0, null);
        // No chasers in game
        assertEquals(whimTest.target(gameTest), new ArrayList<Integer>(Arrays.asList(-96, 96)));

        // Chasers are dead
        Chaser chaserTest = new Chaser(0, 0, null);
        chaserTest.alive = false;
        gameTest.ghostList.add(chaserTest);
        assertEquals(whimTest.target(gameTest), new ArrayList<Integer>(Arrays.asList(-96, 96)));
    }

    @Test
    public void scatterModeTest() {
        // Testing whether scatter mode target is bottom right corner
        App appTest = new App();
        PApplet.runSketch(new String[]{""}, appTest);
        appTest.setup();
        Game gameTest = appTest.game;
        gameTest.mode = "scatter";
        Whim whimTest = new Whim(100, 100, null);
        // Bottom right corner
        assertEquals(whimTest.target(gameTest), new ArrayList<Integer>(Arrays.asList(336, 464)));
    }

    @Test
    public void unknownModeTest() {
        // Testing if the mode is unknown, then default target is current position
        App appTest = new App();
        PApplet.runSketch(new String[]{""}, appTest);
        appTest.setup();
        Game gameTest = appTest.game;
        gameTest.mode = "unknown";
        gameTest.waka = new Waka(0, 0);
        Whim whimTest = new Whim(200, 200, null);
        assertEquals(whimTest.target(gameTest), new ArrayList<Integer>(Arrays.asList(0, 0)));
    }
}