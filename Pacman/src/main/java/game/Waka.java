package game;

import processing.core.PImage;
import java.util.*;

/**
* Waka class which manages all aspects of Waka's functionality.
*/
public class Waka extends GameObject implements Character {
    /**
    * The current velocity in the x direction.
    */
    public int xVelocity;
    /**
    * The current velocity in the y direction.
    */
    public int yVelocity;
    /**
    * The PImage sprite of the right facing Waka.
    */
    public PImage playerRight;
    /**
    * The PImage sprite of the left facing Waka.
    */
    public PImage playerLeft;
    /**
    * The PImage sprite of the up facing Waka.
    */
    public PImage playerUp;
    /**
    * The PImage sprite of the down facing Waka.
    */
    public PImage playerDown;
    /**
    * The PImage sprite of the closed Waka.
    */
    public PImage playerClosed;
    /**
    * Whether Waka's mouth is currently closed.
    */
    public boolean mouthClosed;
    /**
    * The current direction that the mouth is pointing at.
    * <br>
    * (left, right, up, down)
    */
    public String mouthDirection; // Direction of Waka for mouth animation

    /**
    * Initialises variables of the Waka object.
    * @param x The x coordinate
    * @param y The y coordinate
    */
    public Waka(int x, int y) {
        super(x, y, null);

        this.xVelocity = 0;
        this.yVelocity = 0;

        this.mouthClosed = true;
        this.mouthDirection = "left";
    }

    public void tick(App app, Game game) {
        this.x += this.xVelocity;
        this.y += this.yVelocity;
        
        // Saves the current direction of Waka's movement for animation
        if (this.xVelocity < 0) {
            this.mouthDirection = "left";
        } else if (this.xVelocity > 0) {
            this.mouthDirection = "right";
        } else if (this.yVelocity < 0) {
            this.mouthDirection = "up";
        } else if (this.yVelocity > 0) {
            this.mouthDirection = "down";
        }

        this.conductReverseDirection(game); // Allows Waka to turn around
        this.conductQueuedMovement(game); // Movement method for Waka
        this.checkWallCollision(game); // Method for bouncing off walls
        this.checkFruitCollision(app, game); // Method to check consumable collision
        this.checkGhostCollision(game); // Check ghost collision
    }

    public void draw(App app, Game game) {
        app.image(this.sprite, this.x, this.y);
    }

    public void setSpeed(int xVelocity, int yVelocity) {
        this.xVelocity = xVelocity;
        this.yVelocity = yVelocity;
    }

    // Change Waka's sprite in the direction of movement (to permit mouth animation)
    public void changeSprite() {
        if (this.mouthClosed == true) {
            if (this.mouthDirection == "left") {
                this.sprite = this.playerLeft;
                this.mouthClosed = false;
            } else if (this.mouthDirection ==  "right") {
                this.sprite = this.playerRight;
                this.mouthClosed = false;
            } else if (this.mouthDirection == "up") {
                this.sprite = this.playerUp;
                this.mouthClosed = false;
            } else if (this.mouthDirection == "down") {
                this.sprite = this.playerDown;
                this.mouthClosed = false;
            }
        } else {
            this.sprite = this.playerClosed;
            this.mouthClosed = true;
        }
    }

    // Reset Waka's characteristics (once he collides with a ghost)
    public void reset() {
        this.x = this.xSpawn;
        this.y = this.ySpawn;
        this.xVelocity = 0;
        this.yVelocity = 0;
        this.sprite = this.playerLeft;
    }
    
    // ____________________________________________________________________________________________

    // Allows for Waka to turn in the opposite direction instantly
    /**
    * Allows Waka to instantly turn in the opposite direction if the opposite keyboard keypress
    * is made.
    * @param game The Game object
    */
    public void conductReverseDirection(Game game) {
        // If Waka's direction is currently opposite to the keypress,
        // reset waka's speed to the new keypress direction
        if (game.moveKey == 37 && this.xVelocity > 0) { // LEFT
            this.setSpeed(-game.speed, 0);
        } else if (game.moveKey == 39 && this.xVelocity < 0) { // RIGHT
            this.setSpeed(game.speed, 0);
        } else if (game.moveKey == 38 && this.yVelocity > 0) { // UP
            this.setSpeed(0, -game.speed);
        } else if (game.moveKey == 40 && this.yVelocity < 0) { // DOWN
            this.setSpeed(0, game.speed);
        }
    }

    /**
    * A method for queued movement, allowing Waka to turn in the direction of a keypress only
    * if Waka is in the centre of the current cell and there is no wall blocking the path.
    * <br>
    * This allows Waka to turn at the next possible opportunity.
    * @param game The Game object
    */
    public void conductQueuedMovement(Game game) {
        // Consider movement direction only at the middle of a cell
        if (this.isCentre(game.WAKAOFFSET) == false) {
            return;
        }

        // Iterate through the list of walls to attempt to find a wall cell in the direction of
        // keypress (desired movement). If not found, then Waka can turn in that direction
        int counter = 0;
        for (Wall wall : game.wallList) {
            if (game.moveKey == 37) { // MOVEMENT LEFT
                // Consider the cell to to he left of the current
                if (wall.x == this.getCx(game.WAKAOFFSET) - game.GRIDSPACE && wall.y == this.getCy(game.WAKAOFFSET)) {
                    return;
                } else if (counter == game.wallList.size()-1) {
                    // If no wall is found in the direction of keypress, turn
                    this.setSpeed(-game.speed, 0);
                }
            } else if (game.moveKey == 38) { // MOVEMENT UP
                // Consider the cell to the top of the current
                if (wall.x == this.getCx(game.WAKAOFFSET) && wall.y == this.getCy(game.WAKAOFFSET) - 16) {
                    return;
                } else if (counter == game.wallList.size()-1) {
                     // If no wall is found in the direction of keypress, turn
                    this.setSpeed(0, -game.speed);
                }
            } else if (game.moveKey == 39) { // MOVEMENT RIGHT
                // Consider the cell to the left of the current
                if (wall.x == this.getCx(game.WAKAOFFSET) + 16 && wall.y == this.getCy(game.WAKAOFFSET)) {
                    return;                          
                } else if (counter == game.wallList.size()-1) {
                    // If no wall is found in the direction of keypress, turn
                    this.setSpeed(game.speed, 0);
                }
            } else if (game.moveKey == 40) { // MOVEMENT DOWN
                // Consider the cell to the top of the current
                if (wall.x == this.getCx(game.WAKAOFFSET) && wall.y == this.getCy(game.WAKAOFFSET) + 16) {
                    return;  
                } else if (counter == game.wallList.size()-1) {
                    // If no wall is found in the direction of keypress, turn
                    this.setSpeed(0, game.speed);
                }
            }
            counter += 1;
        }
    }

    /**
    * Halt Waka's movement if he runs into a wall in the direction of movement.
    * @param game The Game object
    */
    public void checkWallCollision(Game game) {
        // Consider movement direction only at the middle of a cell
        if (this.isCentre(game.WAKAOFFSET) == false) {
            return;
        }

        for (Wall wall : game.wallList) {
            if (this.xVelocity < 0) { // MOVEMENT LEFT
                // Consider the cell to the left of the current
                if (wall.x == this.getCx(game.WAKAOFFSET) - game.GRIDSPACE && wall.y == this.getCy(game.WAKAOFFSET)) {
                    this.setSpeed(0, 0); 
                    return;
                }
            } else if (this.xVelocity > 0) { // MOVEMENT RIGHT
                // Consider the cell to the right of the current
                if (wall.x == this.getCx(game.WAKAOFFSET) + game.GRIDSPACE && wall.y == this.getCy(game.WAKAOFFSET)) {
                    this.setSpeed(0, 0);
                    return;
                }
            } else if (this.yVelocity < 0) { // MOVEMENT UP
                // Consider the cell above the current
                if (wall.x == this.getCx(game.WAKAOFFSET) && wall.y == this.getCy(game.WAKAOFFSET) - game.GRIDSPACE) {
                    this.setSpeed(0, 0);
                    return;
                }
            } else if (this.yVelocity > 0) { // MOVEMENT DOWN
                // Consider the cell above the current
                if (wall.x == this.getCx(game.WAKAOFFSET) && wall.y == this.getCy(game.WAKAOFFSET) + game.GRIDSPACE) {
                    this.setSpeed(0, 0);
                    return;
                }
            }
        }
    }

    /**
    * Detect if Waka is colliding with a consumable object. If so, activate their individual collide methods.
    * @param app The App object
    * @param game The Game object
    */
    public void checkFruitCollision(App app, Game game) {
        for (int i = 0; i < game.consumableList.size(); i++) {
            Consumable cell = game.consumableList.get(i);     
            int hitBoxOffset = 0;
            hitBoxOffset = cell.offset; // Retreive drawing offset value
            // x-coordinates or y-coordinates overlap
            if ((cell.x + 8 + hitBoxOffset >= this.x + hitBoxOffset && cell.x <= this.x + 8) && 
                (cell.y + 8 + hitBoxOffset >= this.y + hitBoxOffset && cell.y <= this.y + 8 + hitBoxOffset)) {
                cell.collide(app, game); 
            }
        }
    }

    /**
    * Check whether Waka is colliding with a ghost and call ghost's collide method if collision has occured.
    * @param game The Game object
    */
    public void checkGhostCollision(Game game) {
        for (Ghost ghost : game.ghostList) {
            // x-coordinates or y-coordinates overlap
            if ((ghost.x + 23 >= this.x && ghost.x <= this.x + 23) && (ghost.y + 23 >= this.y && ghost.y <= this.y + 23)) {
                ghost.collide(game);
            }
        }
    }
}