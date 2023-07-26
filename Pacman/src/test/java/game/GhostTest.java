package game;

import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

import java.util.*;
import processing.core.PImage;
import processing.core.PApplet;

class GhostTest {
    @Test 
    public void changeSpriteTest() {
        // Test that the ghost sprites correctly change when the method is called
        App appTest = new App();
        PApplet.runSketch(new String[]{""}, appTest);
        appTest.setup();
        Game gameTest = appTest.game;
        Chaser ghostTest = new Chaser(0, 0, null);
        ghostTest.standardSprite = appTest.loadImage("src/main/resources/chaser.png");
        ghostTest.frightenedSprite = appTest.loadImage("src/main/resources/frightened.png");
        ghostTest.invisibleSprite = appTest.loadImage("src/main/resources/invisible.png");
        ghostTest.sprite = ghostTest.standardSprite;
        // Frightened mode
        ghostTest.ghostMode = 1;
        ghostTest.changeSprite();
        assertEquals(ghostTest.sprite, ghostTest.frightenedSprite);
        // Invisible mode
        ghostTest.ghostMode = 2;
        ghostTest.changeSprite();
        assertEquals(ghostTest.sprite, ghostTest.invisibleSprite);
        // Normal mode
        ghostTest.ghostMode = 0;
        ghostTest.changeSprite();
        assertEquals(ghostTest.sprite, ghostTest.standardSprite);
    }

    @Test
    public void reset() {
        // Test that the ghosts correctly reset their attributes to their initial spawn
        App appTest = new App();
        PApplet.runSketch(new String[]{""}, appTest);
        appTest.setup();
        Game gameTest = appTest.game;
        Chaser ghostTest = new Chaser(100, 100, null);
        ghostTest.standardSprite = appTest.loadImage("src/main/resources/chaser.png");
        ghostTest.frightenedSprite = appTest.loadImage("src/main/resources/frightened.png");
        // Change parameters
        ghostTest.x = 150;
        ghostTest.y = 50;
        ghostTest.xVelocity = 5;
        ghostTest.yVelocity = -5;
        ghostTest.alive = false;
        ghostTest.sprite = ghostTest.frightenedSprite;
        ghostTest.reset();
        assertArrayEquals(new int[] {ghostTest.x, ghostTest.y, ghostTest.xVelocity, ghostTest.yVelocity}, 
            new int[] {100, 100, 0, 0});
        assertTrue(ghostTest.alive);
        assertEquals(ghostTest.sprite, ghostTest.standardSprite);
    }

    @Test
    public void collideTest() {
        // Test that when the collide() method is called upon collision with a ghost and Waka
        // that correct changes to the game state are made
        App appTest = new App();
        PApplet.runSketch(new String[]{""}, appTest);
        appTest.setup();
        Game gameTest = appTest.game;
        gameTest.lives = 3;
        gameTest.ghostList = new ArrayList<Ghost>();
        Chaser ghostTest = new Chaser(100, 100, null);
        gameTest.ghostList.add(ghostTest);
        gameTest.waka = new Waka(100, 100);

        gameTest.app = appTest;
        // Normal ghost collision
        ghostTest.collide(gameTest);
        assertEquals(gameTest.lives, 2);
        // Frightened ghost collision
        ghostTest.ghostMode = 1;
        ghostTest.collide(gameTest);
        assertFalse(ghostTest.alive);
        // Unalive ghost collision (nothing happens)
        ghostTest.ghostMode = 0;
        ghostTest.collide(gameTest);
        assertEquals(gameTest.lives, 2);
        assertFalse(ghostTest.alive);
    } 

    @Test
    public void getTargetTest() {
        // Test that getTarget() correctly obtains and edits the distance to the target
        App appTest = new App();
        PApplet.runSketch(new String[]{""}, appTest);
        appTest.setup();
        Game gameTest = appTest.game;
        gameTest.mode = "chase";
        // Standard test
        gameTest.waka = new Waka(200, 200);
        Chaser chaserTest = new Chaser(100, 100, null);
        assertEquals(chaserTest.getTarget(gameTest), new ArrayList<Integer>(Arrays.asList(96, 96)));
        // Outside Grid Bounds Test
        // x to the right, y up
        gameTest.waka = new Waka(1000, -1000);
        assertEquals(chaserTest.getTarget(gameTest), new ArrayList<Integer>(Arrays.asList(336, -96)));
        // x to the left, y down
        gameTest.waka = new Waka(-1000, 1000);
        assertEquals(chaserTest.getTarget(gameTest), new ArrayList<Integer>(Arrays.asList(-96, 464)));

        // Randomised target test (using a range)
        chaserTest.ghostMode = 1; // Frightened mode
        ArrayList<Integer> lengths = chaserTest.getTarget(gameTest);
        assertTrue(lengths.get(0) >= -432 && lengths.get(0) <= 432 && lengths.get(0) >= -560 && lengths.get(0) <= 560);
        chaserTest.ghostMode = 2; // Invisible mode
        lengths = chaserTest.getTarget(gameTest);
        assertTrue(lengths.get(0) >= -432 && lengths.get(0) <= 432 && lengths.get(0) >= -560 && lengths.get(0) <= 560);
    }

    @Test
    public void setTargetDirectionTest() {
        // Test that setTargetDirection() will correctly parse the distance to the target
        // Into a list of directions
        App appTest = new App();
        PApplet.runSketch(new String[]{""}, appTest);
        appTest.setup();
        Game gameTest = appTest.game;
        gameTest.mode = "chase";
        Chaser chaserTest = new Chaser(100, 100, null);
        chaserTest.directionList = new ArrayList<String>();

        // Not in the middle of a cell
        chaserTest.x = 127;
        chaserTest.y = 127;
        chaserTest.setTargetDirection(gameTest);
        assertTrue(chaserTest.directionList.size() == 0);

        chaserTest.x = 187; // Centre the ghost in a cell
        chaserTest.y = 187;
        // DIRECTION LIST TESTING
        // MORE LEFT, UP
        gameTest.waka = new Waka(12, 156);
        chaserTest.setTargetDirection(gameTest);
        assertEquals(chaserTest.directionList, new ArrayList<String>(Arrays.asList("left", "up", "down", "right")));
        
        // MORE LEFT, DOWN
        gameTest.waka = new Waka(12, 226);
        chaserTest.setTargetDirection(gameTest);
        assertEquals(chaserTest.directionList, new ArrayList<String>(Arrays.asList("left", "down", "up", "right")));

        // MORE RIGHT, UP
        gameTest.waka = new Waka(288, 156);
        chaserTest.setTargetDirection(gameTest);
        assertEquals(chaserTest.directionList, new ArrayList<String>(Arrays.asList("right", "up", "down", "left")));

        // MORE RIGHT, DOWN
        gameTest.waka = new Waka(288, 226);
        chaserTest.setTargetDirection(gameTest);
        assertEquals(chaserTest.directionList, new ArrayList<String>(Arrays.asList("right", "down", "up", "left")));

        // MORE UP, LEFT
        gameTest.waka = new Waka(151, 28);
        chaserTest.setTargetDirection(gameTest);
        assertEquals(chaserTest.directionList, new ArrayList<String>(Arrays.asList("up", "left", "right", "down")));

        // MORE UP, Right
        gameTest.waka = new Waka(220, 28);
        chaserTest.setTargetDirection(gameTest);
        assertEquals(chaserTest.directionList, new ArrayList<String>(Arrays.asList("up", "right", "left", "down")));

        // MORE DOWN, LEFT
        gameTest.waka = new Waka(151, 288);
        chaserTest.setTargetDirection(gameTest);
        assertEquals(chaserTest.directionList, new ArrayList<String>(Arrays.asList("down", "left", "right", "up")));

        // MORE DOWN, RIGHT
        gameTest.waka = new Waka(220, 288);
        chaserTest.setTargetDirection(gameTest);
        assertEquals(chaserTest.directionList, new ArrayList<String>(Arrays.asList("down", "right", "left", "up")));
    }

    @Test
    public void followTargetTest() {
        // Test that the ghost will correctly follow the list of directions in scenarios
        // where there are different arrangements of walls around it
        App appTest = new App();
        PApplet.runSketch(new String[]{""}, appTest);
        appTest.setup();
        Game gameTest = appTest.game;
        gameTest.mode = "chase";
        gameTest.speed = 5;
        gameTest.wallList = new ArrayList<Wall>();
        Chaser chaserTest = new Chaser(100, 100, null);

        // Not in the middle of a cell (but can move)
        chaserTest.x = 127;
        chaserTest.y = 127;
        chaserTest.setSpeed(0, gameTest.speed);
        chaserTest.followTarget(gameTest);
        assertArrayEquals(new int[] {chaserTest.xVelocity, chaserTest.yVelocity}, 
            new int[] {0, gameTest.speed});
        
        // SPEED CHANGE DETECTION IN RELATION TO DIRECTION LIST
        chaserTest.x = 187; // Set to centre of cell
        chaserTest.y = 187;
        chaserTest.directionList = new ArrayList<String>(Arrays.asList("left", "up", "down", "right"));

        gameTest.wallList.add(new Wall(0, 0, null));
        // NO WALLS, Moving UP - should move left
        chaserTest.setSpeed(0, -gameTest.speed);
        chaserTest.followTarget(gameTest); 
        assertTrue(chaserTest.xVelocity == -gameTest.speed && chaserTest.yVelocity == 0);

        // NO WALLS, Moving RIGHT - should move up
        chaserTest.setSpeed(gameTest.speed, 0);
        chaserTest.followTarget(gameTest); 
        assertTrue(chaserTest.xVelocity == 0 && chaserTest.yVelocity == -gameTest.speed);

        // NO WALLS, Moving LEFT - should move left
        chaserTest.setSpeed(-gameTest.speed, 0);
        chaserTest.followTarget(gameTest); 
        assertTrue(chaserTest.xVelocity == -gameTest.speed && chaserTest.yVelocity == 0);

        // NO WALL, Moving DOWN - should move left
        chaserTest.setSpeed(0, gameTest.speed);
        chaserTest.followTarget(gameTest); 
        assertTrue(chaserTest.xVelocity == -gameTest.speed && chaserTest.yVelocity == 0);
        
        // ______________________________________

        gameTest.wallList.add(new Wall(176, 192, null));
        // WALL ON LEFT, Moving UP - should move up
        chaserTest.setSpeed(0, -gameTest.speed);
        chaserTest.followTarget(gameTest); 
        assertTrue(chaserTest.xVelocity == 0 && chaserTest.yVelocity == -gameTest.speed);

        // WALL ON LEFT, Moving DOWN - should move down
        chaserTest.setSpeed(0, gameTest.speed);
        chaserTest.followTarget(gameTest); 
        assertTrue(chaserTest.xVelocity == 0 && chaserTest.yVelocity == gameTest.speed);

        // WALL ON LEFT, MOVING RIGHT - should move up
        chaserTest.setSpeed(gameTest.speed, 0);
        chaserTest.followTarget(gameTest); 
        assertTrue(chaserTest.xVelocity == 0 && chaserTest.yVelocity == -gameTest.speed);

        // WALL ON LEFT, MOVING LEFT - should move up
        chaserTest.setSpeed(-gameTest.speed, 0);
        chaserTest.followTarget(gameTest); 
        assertTrue(chaserTest.xVelocity == 0 && chaserTest.yVelocity == -gameTest.speed);

        // ______________________________________
        
        gameTest.wallList.add(new Wall(192, 176, null));
        // WALL ON LEFT+UP, Moving UP - should move right
        chaserTest.setSpeed(0, -gameTest.speed);
        chaserTest.followTarget(gameTest); 
        assertTrue(chaserTest.xVelocity == gameTest.speed && chaserTest.yVelocity == 0);

        // WALL ON LEFT+UP, Moving DOWN - should move down
        chaserTest.setSpeed(0, gameTest.speed);
        chaserTest.followTarget(gameTest); 
        assertTrue(chaserTest.xVelocity == 0 && chaserTest.yVelocity == gameTest.speed);

        // WALL ON LEFT+UP, Moving RIGHT - should move down
        chaserTest.setSpeed(gameTest.speed, 0);
        chaserTest.followTarget(gameTest); 
        assertTrue(chaserTest.xVelocity == 0 && chaserTest.yVelocity == gameTest.speed);

        // WALL ON LEFT+UP, Moving LEFT - should move down
        chaserTest.setSpeed(-gameTest.speed, 0);
        chaserTest.followTarget(gameTest); 
        assertTrue(chaserTest.xVelocity == 0 && chaserTest.yVelocity == gameTest.speed);

        // ______________________________________

        gameTest.wallList.add(new Wall(192, 208, null));
        // WALL ON LEFT+UP+DOWN, Moving UP - should move right
        chaserTest.setSpeed(0, -gameTest.speed);
        chaserTest.followTarget(gameTest); 
        assertTrue(chaserTest.xVelocity == gameTest.speed && chaserTest.yVelocity == 0);

        // WALL ON LEFT+UP+DOWN, Moving DOWN - should move right
        chaserTest.setSpeed(0, gameTest.speed);
        chaserTest.followTarget(gameTest); 
        assertTrue(chaserTest.xVelocity == gameTest.speed && chaserTest.yVelocity == 0);

        // WALL ON LEFT+UP+DOWN, Moving RIGHT - should move right
        chaserTest.setSpeed(gameTest.speed, 0);
        chaserTest.followTarget(gameTest); 
        assertTrue(chaserTest.xVelocity == gameTest.speed && chaserTest.yVelocity == 0);

        // WALL ON LEFT+UP+DOWN, Moving LEFT - should move RIGHT (only way out)
        chaserTest.setSpeed(-gameTest.speed, 0);
        chaserTest.followTarget(gameTest); 
        assertTrue(chaserTest.xVelocity == gameTest.speed && chaserTest.yVelocity == 0);

        // ______________________________________

        chaserTest.directionList = new ArrayList<String>(Arrays.asList("right", "up", "down", "left"));
        gameTest.wallList = new ArrayList<Wall>();
        gameTest.wallList.add(new Wall(208, 192, null));

        // WALL ON THE RIGHT, Moving RIGHT - should move UP
        chaserTest.setSpeed(gameTest.speed, 0);
        chaserTest.followTarget(gameTest); 
        assertTrue(chaserTest.xVelocity == 0 && chaserTest.yVelocity == -gameTest.speed);

        gameTest.wallList.add(new Wall(192, 176, null));
        gameTest.wallList.add(new Wall(192, 208, null));
        // WALL ON RIGHT+UP+DOWN, Moving RIGHT - should move left (only way out)
        chaserTest.setSpeed(gameTest.speed, 0);
        chaserTest.followTarget(gameTest); 
        assertTrue(chaserTest.xVelocity == -gameTest.speed && chaserTest.yVelocity == 0);

        // ______________________________________

        chaserTest.directionList = new ArrayList<String>(Arrays.asList("up", "left", "right", "down"));

        gameTest.wallList = new ArrayList<Wall>();
        gameTest.wallList.add(new Wall(208, 192, null));
        gameTest.wallList.add(new Wall(176, 192, null));
        gameTest.wallList.add(new Wall(192, 176, null));
        // WALL ON UP+LEFT+RIGHT, Moving UP - should move down (only way out)
        chaserTest.setSpeed(0, -gameTest.speed);
        chaserTest.followTarget(gameTest); 
        assertTrue(chaserTest.xVelocity == 0 && chaserTest.yVelocity == gameTest.speed);

        // ______________________________________

        chaserTest.directionList = new ArrayList<String>(Arrays.asList("down", "left", "right", "up"));

        gameTest.wallList = new ArrayList<Wall>();
        gameTest.wallList.add(new Wall(208, 192, null));
        gameTest.wallList.add(new Wall(176, 192, null));
        gameTest.wallList.add(new Wall(192, 208, null));
        // WALL ON DOWN+LEFT+RIGHT, Moving UP - should move down (only way out)
        chaserTest.setSpeed(0, gameTest.speed);
        chaserTest.followTarget(gameTest); 
        assertTrue(chaserTest.xVelocity == 0 && chaserTest.yVelocity == -gameTest.speed);
    }

    @Test
    public void tickTest() {
        // Test that tick() correctly updates ghost attributes
        App appTest = new App();
        PApplet.runSketch(new String[]{""}, appTest);
        appTest.setup();
        Game gameTest = appTest.game;
        gameTest.mode = "chase";
        gameTest.speed = 5;
        Chaser chaserTest = new Chaser(100, 100, null);

        // Alive
        chaserTest.setSpeed(gameTest.speed, -gameTest.speed);
        chaserTest.tick(appTest, gameTest);
        assertTrue(chaserTest.x == 105 && chaserTest.y == 95);

        // Dead
        chaserTest.alive = false;
        chaserTest.tick(appTest, gameTest);
        assertTrue(chaserTest.x == 105 && chaserTest.y == 95);
    }   

    @Test
    public void drawDebugTest() {     
        // Testing drawDebug() when it shouldn't draw   
        App appTest = new App();
        PApplet.runSketch(new String[]{""}, appTest);
        appTest.setup();
        Game gameTest = appTest.game;
        gameTest.mode = "chase";
        gameTest.waka = new Waka(100, 100);
        Chaser chaserTest = new Chaser(100, 100, null);
        // Debug mode false and standard ghost mode
        gameTest.debugMode = false;
        chaserTest.drawDebug(appTest, gameTest);
        // Debug mode true but ghost mode not standard
        gameTest.debugMode = true;
        chaserTest.ghostMode = -1; // test value
        chaserTest.drawDebug(appTest, gameTest);
    }
}