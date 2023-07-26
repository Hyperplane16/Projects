package game;

import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

import java.util.*;
import processing.core.PFont;
import processing.core.PApplet;
import processing.core.PImage;

class GameTest {    
    @Test
    public void executionTest() {
        // Test whether the game object can correctly execute
        App appTest = new App();
        PApplet.runSketch(new String[]{""}, appTest);
        appTest.setup();
        // Actual constructor
        appTest.game = new Game(appTest, "config.json");
        appTest.game.draw(appTest);
        assertNotNull(appTest.game);
        // Testing constructor
        appTest.game = new Game("test");
        assertNotNull(appTest.game);
    }

    @Test 
    public void checkEndTest() {
        // Test whether data is correctly instantiated in the constructor
        App appTest = new App();
        PApplet.runSketch(new String[]{""}, appTest);
        appTest.setup();
        Game gameTest = appTest.game;
        // Not ended yet
        gameTest.state = 0;
        gameTest.consumableList = new ArrayList<Consumable>();
        gameTest.consumableList.add(new Fruit(0, 0, null));
        gameTest.lives = 3;
        gameTest.ghostList = new ArrayList<Ghost>();
        Chaser chaserTest = new Chaser(0, 0, null);
        gameTest.ghostList.add(chaserTest);
        gameTest.checkEnd(appTest);
        assertTrue(gameTest.state == 0);
        // All consumables have been eaten only
        gameTest.consumableList = new ArrayList<Consumable>();
        gameTest.checkEnd(appTest);
        assertTrue(gameTest.state == 1);
        // Waka loses all lives only
        gameTest.consumableList.add(new Fruit(0, 0, null));
        gameTest.state = 0;
        gameTest.lives = 0;
        gameTest.checkEnd(appTest);
        assertTrue(gameTest.state == 2);
        // All ghosts are dead only
        gameTest.consumableList.add(new Fruit(0, 0, null));
        gameTest.state = 0;
        gameTest.lives = 3;
        chaserTest.alive = false;
        gameTest.checkEnd(appTest);
        assertTrue(gameTest.state == 1);

        // TESTS WHERE STATE IS NOT 0
        // All ghosts are dead only
        gameTest.state = -1; // test value
        gameTest.checkEnd(appTest);
        assertTrue(gameTest.state == -1);
        // Waka loses all lives only
        chaserTest.alive = true;
        gameTest.lives = 0;
        gameTest.checkEnd(appTest);
        assertTrue(gameTest.state == -1);
        // All consumables are eaten only
        gameTest.lives = 3;
        gameTest.consumableList = new ArrayList<Consumable>();
        gameTest.checkEnd(appTest);
        assertTrue(gameTest.state == -1);

        // Timer for reset
        gameTest.timerEnd = -10001;
        gameTest.checkEnd(appTest);
    }

    @Test
    public void drawLivesTest() {
        // Test that the drawLives method activates, but no lives
        App appTest = new App();
        PApplet.runSketch(new String[]{""}, appTest);
        appTest.setup();
        Game gameTest = appTest.game; 
        gameTest.lives = 0;
        gameTest.drawLives(appTest);
    }

    @Test
    public void specialModeTest() {
        // Test that when the specialMode method has been called that the game and ghost attributes
        // for mode and sprite have been correctly modified
        App appTest = new App();
        PApplet.runSketch(new String[]{""}, appTest);
        appTest.setup();
        Game gameTest = appTest.game; 
        gameTest.ghostList = new ArrayList<Ghost>();
        Chaser chaserTest = new Chaser(0, 0, null);
        chaserTest.standardSprite = appTest.loadImage("src/main/resources/chaser.png");
        chaserTest.frightenedSprite = appTest.loadImage("src/main/resources/frightened.png");
        chaserTest.frightenedSprite = appTest.loadImage("src/main/resources/invisible.png");
        gameTest.ghostList.add(chaserTest);

        // Frightened Mode
        gameTest.mode = "scatter";
        gameTest.specialMode(appTest, "frightened");
        assertTrue(gameTest.mode == "frightened");
        assertTrue(chaserTest.ghostMode == 1);
        assertTrue(chaserTest.sprite == chaserTest.frightenedSprite);

        // Invisible Mode
        gameTest.mode = "chase";
        gameTest.specialMode(appTest, "invisible");
        assertTrue(gameTest.mode == "invisible");
        assertTrue(chaserTest.ghostMode == 2);
        assertTrue(chaserTest.sprite == chaserTest.invisibleSprite);

        // Unknown Mode
        gameTest.mode = "scatter";
        gameTest.specialMode(appTest, "unknown");
        assertTrue(gameTest.mode == "scatter");
    }

    @Test
    public void manageTimersTest() {
        // Test that when the timer code activates it makes the correct changes
        // This assumes that the current time app.millis() is larger than the required time
        // - hence the use of large negative test values below
        App appTest = new App();
        PApplet.runSketch(new String[]{""}, appTest);
        appTest.setup();
        Game gameTest = appTest.game; 

        gameTest.timerWaka = 0;
        gameTest.timerMode = 0;
        gameTest.timerModeDifference = 0;
        gameTest.timerSpecial = 0;
        gameTest.mode = "scatter";
        gameTest.modeLengths = new int[] {1000, 2000, 3000};
        gameTest.modeIndex = 0;

        gameTest.waka = new Waka(100, 100);
        gameTest.ghostList = new ArrayList<Ghost>();
        Chaser chaserTest = new Chaser(0, 0, null);
        gameTest.ghostList.add(chaserTest);

        // MODE TIMER TESTS
        // Changing Waka animation
        gameTest.timerWaka = -10000000; // test value
        gameTest.manageTimers(appTest);

        // Changing from scatter to chase modes
        gameTest.timerMode = -10000000; // test value
        gameTest.manageTimers(appTest);
        assertEquals(gameTest.mode, "chase");
        assertTrue(gameTest.modeIndex == 1);
        assertEquals(gameTest.previousMode, "chase");

        // Change from chase to scatter mode
        gameTest.timerMode = -10000000; // test value
        gameTest.manageTimers(appTest);
        assertEquals(gameTest.mode, "scatter");
        assertTrue(gameTest.modeIndex == 2);
        assertEquals(gameTest.previousMode, "scatter");

        // Restart at scatter mode when fully traversed through the array
        gameTest.timerMode = -10000000; // test value
        gameTest.modeIndex = 2; // test value (1 less than array length)
        gameTest.manageTimers(appTest);
        assertEquals(gameTest.mode, "scatter");
        assertTrue(gameTest.modeIndex == 0);

        // SPECIAL MODE TESTS
        // Change from frightened Mode
        chaserTest.ghostMode = 1;
        gameTest.mode = "frightened";
        gameTest.previousMode = "scatter";
        gameTest.timerSpecial = -10000000; // test value
        gameTest.manageTimers(appTest);
        assertEquals(gameTest.mode, "scatter");
        assertTrue(chaserTest.ghostMode == 0);

        // from invisible Mode
        chaserTest.ghostMode = 2;
        gameTest.mode = "invisible";
        gameTest.previousMode = "scatter";
        gameTest.timerSpecial = -10000000; // test value
        gameTest.manageTimers(appTest);
        assertEquals(gameTest.mode, "scatter");
        assertTrue(chaserTest.ghostMode == 0);

        // Unknown Mode
        chaserTest.ghostMode = -1;
        gameTest.mode = "unknown";
        gameTest.previousMode = "scatter";
        gameTest.timerSpecial = -10000000; // test value
        gameTest.manageTimers(appTest);
        assertEquals(gameTest.mode, "unknown");
        assertTrue(chaserTest.ghostMode == -1);
    }

    @Test
    public void tickTest() {
        // Test that the game tick() method correctly alters the game elements
        // (where individiual object tick() have already been tested so are omitted)
        App appTest = new App();
        PApplet.runSketch(new String[]{""}, appTest);
        appTest.setup();
        Game gameTest = appTest.game; 

        gameTest.state = 0;
        gameTest.mode = "scatter";
        gameTest.consumableList = new ArrayList<Consumable>();
        gameTest.consumableList.add(new Fruit(0, 0, null));
        gameTest.lives = 3;
        gameTest.ghostList = new ArrayList<Ghost>();
        Chaser chaserTest = new Chaser(0, 0, null);
        gameTest.ghostList.add(chaserTest);
        gameTest.waka = new Waka(100, 100);
        gameTest.timerWaka = 0;
        gameTest.timerMode = 0;
        gameTest.timerModeDifference = 0;
        gameTest.timerSpecial = 0;
        Map.parseData(gameTest, "config.json");
        gameTest.tick(appTest);

        // When game has been ended, test that tick() has been suspended
        gameTest.waka.xVelocity = 5;
        gameTest.waka.yVelocity = 5;
        gameTest.state = -1; // test value
        gameTest.tick(appTest);
        assertTrue(gameTest.waka.x == 100 & gameTest.waka.y == 100);

        // DEBUG MODE TEST
        gameTest.state = 0;
        // Currently not debug mode
        gameTest.commandKey = 32;
        gameTest.debugMode = true;
        gameTest.tick(appTest);
        assertFalse(gameTest.debugMode);
        assertTrue(gameTest.commandKey == 0);
        
        // Current debug mode
        gameTest.commandKey = 32;
        gameTest.debugMode = false;
        gameTest.tick(appTest);
        assertTrue(gameTest.debugMode);
        assertTrue(gameTest.commandKey == 0);
    }
}