package game;

import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

import java.util.*;
import processing.core.PImage;
import processing.core.PApplet;

class WakaTest {
    @Test 
    public void changeSpriteTest() {
        // Testing Waka's sprite changes according to current direction
        App appTest = new App();
        PApplet.runSketch(new String[]{""}, appTest);
        appTest.setup();
        Game gameTest = appTest.game;
        gameTest.waka = new Waka(100, 100);
        gameTest.waka.playerRight = appTest.loadImage("src/main/resources/playerRight.png");
        gameTest.waka.playerLeft = appTest.loadImage("src/main/resources/playerLeft.png");
        gameTest.waka.playerUp = appTest.loadImage("src/main/resources/playerUp.png");
        gameTest.waka.playerDown = appTest.loadImage("src/main/resources/playerDown.png");
        gameTest.waka.playerClosed = appTest.loadImage("src/main/resources/playerClosed.png");
        gameTest.waka.sprite = gameTest.waka.playerLeft;

        // Currently left, transition to closed
        gameTest.waka.mouthClosed = false;
        gameTest.waka.changeSprite();
        assertTrue(gameTest.waka.mouthClosed);
        assertEquals(gameTest.waka.sprite, gameTest.waka.playerClosed);

        // Transition from closed to left
        gameTest.waka.mouthClosed = true;
        gameTest.waka.mouthDirection = "left";
        gameTest.waka.changeSprite();
        assertFalse(gameTest.waka.mouthClosed);
        assertEquals(gameTest.waka.sprite, gameTest.waka.playerLeft);

        // Transition from closed to right
        gameTest.waka.mouthClosed = true;
        gameTest.waka.mouthDirection = "right";
        gameTest.waka.changeSprite();
        assertFalse(gameTest.waka.mouthClosed);
        assertEquals(gameTest.waka.sprite, gameTest.waka.playerRight);

        // Transition from closed to up
        gameTest.waka.mouthClosed = true;
        gameTest.waka.mouthDirection = "up";
        gameTest.waka.changeSprite();
        assertFalse(gameTest.waka.mouthClosed);
        assertEquals(gameTest.waka.sprite, gameTest.waka.playerUp);

        // Transition from closed to down
        gameTest.waka.mouthClosed = true;
        gameTest.waka.mouthDirection = "down";
        gameTest.waka.changeSprite();
        assertFalse(gameTest.waka.mouthClosed);
        assertEquals(gameTest.waka.sprite, gameTest.waka.playerDown);

        // If set mouthDirection is unknown, should not change any attributes
        gameTest.waka.mouthClosed = true;
        gameTest.waka.sprite = gameTest.waka.playerClosed;
        gameTest.waka.mouthDirection = "unknown";
        gameTest.waka.changeSprite();
        assertTrue(gameTest.waka.mouthClosed);
        assertEquals(gameTest.waka.sprite, gameTest.waka.playerClosed);
    }

    @Test
    public void resetTest() {
        // Testing that the reset will correctly reset Waka's attributes
        App appTest = new App();
        PApplet.runSketch(new String[]{""}, appTest);
        appTest.setup();
        Game gameTest = appTest.game;
        gameTest.waka = new Waka(100, 100);
        gameTest.waka.playerRight = appTest.loadImage("src/main/resources/playerRight.png");
        gameTest.waka.playerLeft = appTest.loadImage("src/main/resources/playerLeft.png");
        gameTest.waka.sprite = gameTest.waka.playerLeft;

        gameTest.waka.x += 10;
        gameTest.waka.y += 10;
        gameTest.waka.xVelocity = 5;
        gameTest.waka.yVelocity = -5;
        gameTest.waka.sprite = gameTest.waka.playerRight;
        gameTest.waka.reset();
        assertArrayEquals(new int[] {gameTest.waka.x, gameTest.waka.y, gameTest.waka.xVelocity, gameTest.waka.yVelocity}, 
            new int[] {100, 100, 0, 0});
        assertEquals(gameTest.waka.sprite, gameTest.waka.playerLeft);
    }

    // ____________________________________________________________________________________________

    @Test
    public void conductReverseDirectionTest() {
        // Testing that Waka will correctly turn around in the opposite direction
        // When the required keypress is made
        App appTest = new App();
        PApplet.runSketch(new String[]{""}, appTest);
        appTest.setup();
        Game gameTest = appTest.game;
        gameTest.speed = 5;
        gameTest.waka = new Waka(100, 100);
        // Left keypress when moving right
        gameTest.waka.xVelocity = gameTest.speed;
        gameTest.moveKey = 37;
        gameTest.waka.conductReverseDirection(gameTest);
        assertEquals(gameTest.waka.xVelocity, -gameTest.speed);

        // Left keypress when not moving right
        gameTest.waka.xVelocity = -gameTest.speed;
        gameTest.moveKey = 37;
        gameTest.waka.conductReverseDirection(gameTest);
        assertEquals(gameTest.waka.xVelocity, -gameTest.speed);

        // Right keypress when moving left
        gameTest.waka.xVelocity = -gameTest.speed;
        gameTest.moveKey = 39;
        gameTest.waka.conductReverseDirection(gameTest);
        assertEquals(gameTest.waka.xVelocity, gameTest.speed);

        // Right keypress when not moving left
        gameTest.waka.xVelocity = gameTest.speed;
        gameTest.moveKey = 39;
        gameTest.waka.conductReverseDirection(gameTest);
        assertEquals(gameTest.waka.xVelocity, gameTest.speed);

        gameTest.waka.xVelocity = 0;
        // Up keypress when moving down
        gameTest.waka.yVelocity = gameTest.speed;
        gameTest.moveKey = 38;
        gameTest.waka.conductReverseDirection(gameTest);
        assertEquals(gameTest.waka.yVelocity, -gameTest.speed);

        // Up keypress when not moving down
        gameTest.waka.yVelocity = -gameTest.speed;
        gameTest.moveKey = 38;
        gameTest.waka.conductReverseDirection(gameTest);
        assertEquals(gameTest.waka.yVelocity, -gameTest.speed);

        // Down keypress when moving up
        gameTest.waka.yVelocity = -gameTest.speed;
        gameTest.moveKey = 40;
        gameTest.waka.conductReverseDirection(gameTest);
        assertEquals(gameTest.waka.yVelocity, gameTest.speed);

        // Down keypress when not moving up
        gameTest.waka.yVelocity = gameTest.speed;
        gameTest.moveKey = 40;
        gameTest.waka.conductReverseDirection(gameTest);
        assertEquals(gameTest.waka.yVelocity, gameTest.speed);
    }

    @Test
    public void conductQueuedMovementTest() {
        // Test that Waka will correctly turn according to the player's move key
        // When it has the opportunity to do so (different arrangements of walls)
        App appTest = new App();
        PApplet.runSketch(new String[]{""}, appTest);
        appTest.setup();
        Game gameTest = appTest.game;
        gameTest.speed = 5;
        gameTest.wallList = new ArrayList<Wall>();

        // If not middle of cell (but can move)
        gameTest.wallList.add(new Wall(0, 0, null));
        gameTest.moveKey = 37; // Suppose moving left
        gameTest.waka = new Waka(97, 97);
        gameTest.waka.xVelocity = 0;
        gameTest.waka.yVelocity = gameTest.speed;
        gameTest.waka.conductQueuedMovement(gameTest);
        assertArrayEquals(new int[] {gameTest.waka.xVelocity, gameTest.waka.yVelocity}, 
            new int[] {0, gameTest.speed});
        
        // The remaining test cases involve Waka being in the middle of a cell
        // The program assumes there is at least one wall in the wallList
        gameTest.waka = new Waka(92, 92);

        // Turning LEFT but there's a wall (while moving down)
        gameTest.moveKey = 37;
        gameTest.waka.xVelocity = 0;
        gameTest.waka.yVelocity = gameTest.speed;
        gameTest.wallList.add(new Wall(80, 96, null));
        gameTest.waka.conductQueuedMovement(gameTest);
        assertArrayEquals(new int[] {gameTest.waka.xVelocity, gameTest.waka.yVelocity}, 
            new int[] {0, gameTest.speed});

        // Turning LEFT but there's no wall (while moving down)
        gameTest.wallList = new ArrayList<Wall>();
        gameTest.wallList.add(new Wall(0, 0, null));
        gameTest.waka.conductQueuedMovement(gameTest);
        assertArrayEquals(new int[] {gameTest.waka.xVelocity, gameTest.waka.yVelocity}, 
            new int[] {-gameTest.speed, 0});
        
        // Turning RIGHT but there's a wall (while moving down)
        gameTest.moveKey = 39;
        gameTest.waka.xVelocity = 0;
        gameTest.waka.yVelocity = gameTest.speed;
        gameTest.wallList.add(new Wall(112, 96, null));
        gameTest.waka.conductQueuedMovement(gameTest);
        assertArrayEquals(new int[] {gameTest.waka.xVelocity, gameTest.waka.yVelocity}, 
            new int[] {0, gameTest.speed});
        
        // Turning RIGHT but there's no wall (while moving down)
        gameTest.wallList = new ArrayList<Wall>();
        gameTest.wallList.add(new Wall(0, 0, null));
        gameTest.waka.conductQueuedMovement(gameTest);
        assertArrayEquals(new int[] {gameTest.waka.xVelocity, gameTest.waka.yVelocity}, 
            new int[] {gameTest.speed, 0});
        
        // Turning UP but there's a wall (while moving right)
        gameTest.moveKey = 38;
        gameTest.waka.xVelocity = gameTest.speed;
        gameTest.waka.yVelocity = 0;
        gameTest.wallList.add(new Wall(96, 80, null));
        gameTest.waka.conductQueuedMovement(gameTest);
        assertArrayEquals(new int[] {gameTest.waka.xVelocity, gameTest.waka.yVelocity}, 
            new int[] {gameTest.speed, 0});

        // Turning UP but there's no wall (while moving right)
        gameTest.wallList = new ArrayList<Wall>();
        gameTest.wallList.add(new Wall(0, 0, null));
        gameTest.waka.conductQueuedMovement(gameTest);
        assertArrayEquals(new int[] {gameTest.waka.xVelocity, gameTest.waka.yVelocity}, 
            new int[] {0, -gameTest.speed});
        
        // Turning DOWN but there's a wall (while moving right)
        gameTest.moveKey = 40;
        gameTest.waka.xVelocity = gameTest.speed;
        gameTest.waka.yVelocity = 0;
        gameTest.wallList.add(new Wall(96, 112, null));
        gameTest.waka.conductQueuedMovement(gameTest);
        assertArrayEquals(new int[] {gameTest.waka.xVelocity, gameTest.waka.yVelocity}, 
            new int[] {gameTest.speed, 0});

        // Turning DOWN but there's no wall (while moving right)
        gameTest.wallList = new ArrayList<Wall>();
        gameTest.wallList.add(new Wall(0, 0, null));
        gameTest.waka.conductQueuedMovement(gameTest);
        assertArrayEquals(new int[] {gameTest.waka.xVelocity, gameTest.waka.yVelocity}, 
            new int[] {0, gameTest.speed});
    }

    @Test
    public void checkWallCollisionTest() {
        // Check that if Waka hits a wall head-on it will stop moving
        App appTest = new App();
        PApplet.runSketch(new String[]{""}, appTest);
        appTest.setup();
        Game gameTest = appTest.game;
        gameTest.speed = 5;
        gameTest.wallList = new ArrayList<Wall>();

        // If not middle of cell but moving toward a wall
        gameTest.wallList.add(new Wall(96, 112, null));
        gameTest.waka = new Waka(97, 97);
        gameTest.waka.xVelocity = 0;
        gameTest.waka.yVelocity = gameTest.speed;
        gameTest.waka.checkWallCollision(gameTest);
        assertArrayEquals(new int[] {gameTest.waka.xVelocity, gameTest.waka.yVelocity}, 
            new int[] {0, gameTest.speed});
        
        // The remaining test cases involve Waka being in the middle of a cell
        // The program assumes there is at least one wall in the wallList
        gameTest.waka = new Waka(92, 92);

        // Moving LEFT but there's no wall
        gameTest.waka.xVelocity = -gameTest.speed;
        gameTest.waka.yVelocity = 0;
        gameTest.wallList = new ArrayList<Wall>();
        gameTest.wallList.add(new Wall(0, 0, null));
        gameTest.waka.checkWallCollision(gameTest);
        assertArrayEquals(new int[] {gameTest.waka.xVelocity, gameTest.waka.yVelocity}, 
            new int[] {-gameTest.speed, 0});

        // Moving LEFT but there's a wall
        gameTest.wallList.add(new Wall(80, 96, null));
        gameTest.waka.checkWallCollision(gameTest);
        assertArrayEquals(new int[] {gameTest.waka.xVelocity, gameTest.waka.yVelocity}, 
            new int[] {0, 0});
        
        // Moving RIGHT but there's no wall
        gameTest.waka.xVelocity = gameTest.speed;
        gameTest.waka.yVelocity = 0;
        gameTest.wallList = new ArrayList<Wall>();
        gameTest.wallList.add(new Wall(0, 0, null));
        gameTest.waka.checkWallCollision(gameTest);
        assertArrayEquals(new int[] {gameTest.waka.xVelocity, gameTest.waka.yVelocity}, 
            new int[] {gameTest.speed, 0});

        // Moving RIGHT but there's a wall
        gameTest.wallList.add(new Wall(112, 96, null));
        gameTest.waka.checkWallCollision(gameTest);
        assertArrayEquals(new int[] {gameTest.waka.xVelocity, gameTest.waka.yVelocity}, 
            new int[] {0, 0});
        
        // Moving UP but there's no wall
        gameTest.waka.xVelocity = 0;
        gameTest.waka.yVelocity = -gameTest.speed;
        gameTest.wallList = new ArrayList<Wall>();
        gameTest.wallList.add(new Wall(0, 0, null));
        gameTest.waka.checkWallCollision(gameTest);
        assertArrayEquals(new int[] {gameTest.waka.xVelocity, gameTest.waka.yVelocity}, 
            new int[] {0, -gameTest.speed});

        // Moving UP but there's a wall
        gameTest.wallList.add(new Wall(96, 80, null));
        gameTest.waka.checkWallCollision(gameTest);
        assertArrayEquals(new int[] {gameTest.waka.xVelocity, gameTest.waka.yVelocity}, 
            new int[] {0, 0});

        // Moving DOWN but there's no wall
        gameTest.waka.xVelocity = 0;
        gameTest.waka.yVelocity = gameTest.speed;
        gameTest.wallList = new ArrayList<Wall>();
        gameTest.wallList.add(new Wall(0, 0, null));
        gameTest.waka.checkWallCollision(gameTest);
        assertArrayEquals(new int[] {gameTest.waka.xVelocity, gameTest.waka.yVelocity}, 
            new int[] {0, gameTest.speed});

        // Moving DOWN but there's a wall
        gameTest.wallList.add(new Wall(96, 112, null));
        gameTest.waka.checkWallCollision(gameTest);
        assertArrayEquals(new int[] {gameTest.waka.xVelocity, gameTest.waka.yVelocity}, 
            new int[] {0, 0});
    }

    @Test
    public void checkFruitCollisionTest() {
        // Check that when Waka collides with a consumable object it will
        // Correctly call the collision methods of that object
        App appTest = new App();
        PApplet.runSketch(new String[]{""}, appTest);
        appTest.setup();
        Game gameTest = appTest.game;
        gameTest.consumableList = new ArrayList<Consumable>();
        // Test if specialMode activates
        gameTest.ghostList = new ArrayList<Ghost>();
        gameTest.ghostList.add(new Chaser(0, 0, null)); // Special mode assumes at least 1 ghost exists
        gameTest.mode = "test"; // Test value for attribute

        // When not colliding with anything
        gameTest.consumableList.add(new Fruit(96, 96, null)); 
        gameTest.consumableList.add(new SuperFruit(108, 92, null));    
        gameTest.consumableList.add(new SodaCan(124, 92, null));
        // No collision LEFT
        gameTest.waka = new Waka(0, 92);
        gameTest.waka.checkFruitCollision(appTest, gameTest);
        assertEquals(gameTest.consumableList.size(), 3);
        assertEquals(gameTest.mode, "test");
        // No collision RIGHT
        gameTest.waka = new Waka(300, 92);
        gameTest.waka.checkFruitCollision(appTest, gameTest);
        assertEquals(gameTest.consumableList.size(), 3);
        assertEquals(gameTest.mode, "test");
        // No collision UP
        gameTest.waka = new Waka(112, 0);
        gameTest.waka.checkFruitCollision(appTest, gameTest);
        assertEquals(gameTest.consumableList.size(), 3);
        assertEquals(gameTest.mode, "test");
        // No collision DOWN
        gameTest.waka = new Waka(112, 300);
        gameTest.waka.checkFruitCollision(appTest, gameTest);
        assertEquals(gameTest.consumableList.size(), 3);
        assertEquals(gameTest.mode, "test");
        
        // FRUIT COLLISION
        gameTest.waka = new Waka(92, 92);
        // LEFT edge overlap
        gameTest.consumableList = new ArrayList<Consumable>();
        gameTest.consumableList.add(new Fruit(85, 96, null)); 
        gameTest.waka.checkFruitCollision(appTest, gameTest);
        assertEquals(gameTest.consumableList.size(), 0);
        assertEquals(gameTest.mode, "test");

        // RIGHT edge overlap
        gameTest.consumableList = new ArrayList<Consumable>();
        gameTest.consumableList.add(new Fruit(99, 96, null)); 
        gameTest.waka.checkFruitCollision(appTest, gameTest);
        assertEquals(gameTest.consumableList.size(), 0);
        assertEquals(gameTest.mode, "test");

        // UP edge overlap
        gameTest.consumableList = new ArrayList<Consumable>();
        gameTest.consumableList.add(new Fruit(96, 85, null)); 
        gameTest.waka.checkFruitCollision(appTest, gameTest);
        assertEquals(gameTest.consumableList.size(), 0);
        assertEquals(gameTest.mode, "test");

         // DOWN edge overlap
        gameTest.consumableList = new ArrayList<Consumable>();
        gameTest.consumableList.add(new Fruit(96, 99, null)); 
        gameTest.waka.checkFruitCollision(appTest, gameTest);
        assertEquals(gameTest.consumableList.size(), 0);
        assertEquals(gameTest.mode, "test");

        // SUPERFRUIT COLLISION
        // Overlap
        gameTest.consumableList = new ArrayList<Consumable>();
        gameTest.consumableList.add(new SuperFruit(92, 92, null)); 
        gameTest.waka.checkFruitCollision(appTest, gameTest);
        assertEquals(gameTest.consumableList.size(), 0);
        assertEquals(gameTest.mode, "frightened");

        // SODACAN COLLISION
        // Overlap
        gameTest.mode = "test";
        gameTest.consumableList = new ArrayList<Consumable>();
        gameTest.consumableList.add(new SodaCan(92, 92, null)); 
        gameTest.waka.checkFruitCollision(appTest, gameTest);
        assertEquals(gameTest.consumableList.size(), 0);
        assertEquals(gameTest.mode, "invisible");      
    }

    @Test
    public void checkGhostCollisionTest() {
        // Check that after collision with a ghost the correct changes
        // to the ghost and game state are made
        App appTest = new App();
        PApplet.runSketch(new String[]{""}, appTest);
        appTest.setup();
        Game gameTest = appTest.game;
        gameTest.waka = new Waka(92, 92);

        // FRIGHTENED MODE TESTING
        Chaser chaserTest = new Chaser(0, 91, null);

        // No collision with ghost LEFT
        gameTest.ghostList = new ArrayList<Ghost>();
        chaserTest.ghostMode = 1; // Frightened mode
        gameTest.ghostList.add(chaserTest);
        gameTest.waka.checkGhostCollision(gameTest);
        assertTrue(chaserTest.alive);

        // No collision with ghost RIGHT
        gameTest.ghostList = new ArrayList<Ghost>();
        chaserTest = new Chaser(300, 91, null);
        chaserTest.ghostMode = 1; // Frightened mode
        gameTest.ghostList.add(chaserTest);
        gameTest.waka.checkGhostCollision(gameTest);
        assertTrue(chaserTest.alive);

        // No collision with ghost UP
        gameTest.ghostList = new ArrayList<Ghost>();
        chaserTest = new Chaser(91, 0, null);
        chaserTest.ghostMode = 1; // Frightened mode
        gameTest.ghostList.add(chaserTest);
        gameTest.waka.checkGhostCollision(gameTest);
        assertTrue(chaserTest.alive);

        // No collision with ghost DOWN
        gameTest.ghostList = new ArrayList<Ghost>();
        chaserTest = new Chaser(91, 300, null);
        chaserTest.ghostMode = 1; // Frightened mode
        gameTest.ghostList.add(chaserTest);
        gameTest.waka.checkGhostCollision(gameTest);
        assertTrue(chaserTest.alive);

        // EDGE COLLISION
        // Ghost LEFT edge collision
        gameTest.ghostList = new ArrayList<Ghost>();
        chaserTest = new Chaser(91-22, 91, null);
        chaserTest.ghostMode = 1; // Frightened mode
        gameTest.ghostList.add(chaserTest);
        gameTest.waka.checkGhostCollision(gameTest);
        assertFalse(chaserTest.alive);

        // Ghost RIGHT edge collision
        gameTest.ghostList = new ArrayList<Ghost>();
        chaserTest = new Chaser(91+22, 91, null);
        chaserTest.ghostMode = 1; // Frightened mode
        gameTest.ghostList.add(chaserTest);
        gameTest.waka.checkGhostCollision(gameTest);
        assertFalse(chaserTest.alive);

        // GHOST UP edge collision
        gameTest.ghostList = new ArrayList<Ghost>();
        chaserTest = new Chaser(91, 91-22, null);
        chaserTest.ghostMode = 1; // Frightened mode
        gameTest.ghostList.add(chaserTest);
        gameTest.waka.checkGhostCollision(gameTest);
        assertFalse(chaserTest.alive);

        // GHOST DOWN edge collision
        gameTest.ghostList = new ArrayList<Ghost>();
        chaserTest = new Chaser(91, 91+22, null);
        chaserTest.ghostMode = 1; // Frightened mode
        gameTest.ghostList.add(chaserTest);
        gameTest.waka.checkGhostCollision(gameTest);
        assertFalse(chaserTest.alive);
    }

    @Test
    public void tickTest() {
        // Check that the tick() method correct updates Waka's attributes
        App appTest = new App();
        PApplet.runSketch(new String[]{""}, appTest);
        appTest.setup();
        Game gameTest = appTest.game;
        gameTest.waka = new Waka(100, 100);
        gameTest.waka.xVelocity = -5;
        gameTest.waka.yVelocity = 5;
        // Requires the lists to at least exist
        gameTest.consumableList = new ArrayList<Consumable>();
        gameTest.ghostList = new ArrayList<Ghost>();
        gameTest.wallList = new ArrayList<Wall>();

        gameTest.waka.tick(appTest, gameTest);
        assertArrayEquals(new int[] {gameTest.waka.x, gameTest.waka.y}, 
            new int[] {95, 105});
        
        // DIRECTION Tests
        // LEFT
        gameTest.waka.xVelocity = -5;
        gameTest.waka.yVelocity = 0;
        gameTest.waka.tick(appTest, gameTest);
        assertEquals(gameTest.waka.mouthDirection, "left");

        // RIGHT
        gameTest.waka.xVelocity = 5;
        gameTest.waka.yVelocity = 0;
        gameTest.waka.tick(appTest, gameTest);
        assertEquals(gameTest.waka.mouthDirection, "right");

        // UP
        gameTest.waka.xVelocity = 0;
        gameTest.waka.yVelocity = -5;
        gameTest.waka.tick(appTest, gameTest);
        assertEquals(gameTest.waka.mouthDirection, "up");

        // DOWN
        gameTest.waka.xVelocity = 0;
        gameTest.waka.yVelocity = 5;
        gameTest.waka.tick(appTest, gameTest);
        assertEquals(gameTest.waka.mouthDirection, "down");

        // NO VELOCITY (should retain direction)
        gameTest.waka.mouthDirection = "test";
        gameTest.waka.xVelocity = 0;
        gameTest.waka.yVelocity = 0;
        gameTest.waka.tick(appTest, gameTest);
        assertEquals(gameTest.waka.mouthDirection, "test");
    }
}