package game;

import processing.core.PImage;
import java.util.*;

/**
* Ghost superclass which manages all aspects of ghost functionality.
*/
public abstract class Ghost extends GameObject implements Character {
    /**
    * The current velocity in the x direction.
    */
    public int xVelocity;
    /**
    * The current velocity in the y direction.
    */
    public int yVelocity;
    /**
    * List of cardinal directions of which the ghost aims to follow.
    * <br>
    * (left, right, up, down)
    */
    public ArrayList<String> directionList;
    /**
    * The PImage sprite of the frightened ghost.
    */
    public PImage frightenedSprite;
    /**
    * The PImage sprite of the invisible ghost.
    */
    public PImage invisibleSprite;
    /**
    * The standard PImage sprite of the ghost.
    */
    public PImage standardSprite;
    /**
    * Whether the ghost is currently alive or not.
    */
    public boolean alive;
    /**
    * Stores the current mode of the ghost as an integer.
    * <br>
    * Mode of action (0 = normal, 1 = frightened, 2 = invisible)
    */
    public int ghostMode;

    /**
    * Initialises further variables of the ghost object.
    * @param x The x coordinate
    * @param y The y coordinate
    * @param sprite The object's sprite
    */
    public Ghost(int x, int y, PImage sprite) {
        super(x, y, sprite);

        this.xVelocity = 0;
        this.yVelocity = 0;
        this.alive = true;
        this.ghostMode = 0;
    
        this.directionList = new ArrayList<String>();
        this.standardSprite = this.sprite;
    }

    public void tick(App app, Game game) {
        if (this.alive == true) {
            this.x += this.xVelocity;
            this.y += this.yVelocity;

            this.setTargetDirection(game); // Sets the direction hierarchy
            this.followTarget(game); // Movement for ghost to follow the above hierarchy
            // Note that reverse movement of ghosts is checked by a function called within followTarget()
        }
    }

    public void draw(App app, Game game) {
        if (this.alive == true) {
            // Changing ghost opacity for invisible mode (processing: tint is a on/off switch)
            if (this.ghostMode == 2) { // If in invisible mode
                app.tint(0xff, 100); // Make ghosts transparent (value discretionary)
            }
            app.image(this.sprite, this.x, this.y);
            this.drawDebug(app, game); // Debug mode
            app.tint(0xff, 255); // Reset tint value
        }
    }

    public void setSpeed(int xVelocity, int yVelocity) {
        this.xVelocity = xVelocity;
        this.yVelocity = yVelocity;
    }

    public void changeSprite() { 
        // Change from the standard sprite to a special sprite and back
        if (this.ghostMode == 1) {
            this.sprite = this.frightenedSprite;
        } else if (this.ghostMode == 2) {
            this.sprite = this.invisibleSprite;
        } else {
            this.sprite = this.standardSprite;
        }
    }

    // Reset ghost characteristics (when Waka loses a life)
    public void reset() {
        this.x = this.xSpawn;
        this.y = this.ySpawn;
        this.xVelocity = 0;
        this.yVelocity = 0;
        this.alive = true;
        this.sprite = this.standardSprite;

        this.ghostMode = 0; // Revert to standard ghost mode
        this.changeSprite();
    }
 
    // Target method to be overriden in individual ghost classes
    /**
    * Overriden method which determines the distance from the ghost's current position to the target position.
    * @param game The Game object.
    * @return A list containing the x distance and y distance to the target.
    */
    public abstract ArrayList<Integer> target(Game game);

    // Conduct action if collision with ghost occurs
    /**
    * When collision with Waka occurs change the state of the game depending on the current attributes.
    * @param game The Game object
    */
    public void collide(Game game) {
        if (this.ghostMode == 1) { // Frightened mode
            this.alive = false;
        } else if (this.alive == true) {
            game.lives -= 1;
            game.resetSpawn();
        }
    }

    // ____________________________________________________________________________________________

    // Draw a line from the ghost to the target is debug mode is active
    /**
    * When debug mode is active draw a white line from the centre of the ghost to its target position.
    * @param app The App object
    * @param game The Game object
    */
    public void drawDebug(App app, Game game) {
        // If debug mode is active and default ghost mode, draw debug target lines
        if (game.debugMode == false || this.ghostMode != 0) {
            return;
        }
        // Find the target position
        ArrayList<Integer> lengthDifferences = getTarget(game);
        int xTarget = lengthDifferences.get(0) + this.getCx(game.GHOSTOFFSET);
        int yTarget = lengthDifferences.get(1) + this.getCy(game.GHOSTOFFSET);

        app.stroke(255); // Draw a white line from ghost position to target position
        app.line(this.getCx(game.GHOSTOFFSET) + 14 - game.GHOSTOFFSET, this.getCy(game.GHOSTOFFSET) + 14 - game.GHOSTOFFSET, 
        xTarget + 8, yTarget + 8); 
    }

    // Find the difference in x and y lengths from the current ghost position to target
    // Returns a list of ints with x and y difference respectively
    /**
    * Retrieves a list containing the x and y distances to the target and manipulate it to fit
    * certain conditions. 
    * <br>
    * (if frightened or invisible use randomised target positions, if outside the grid bounds set them to
    * be within)
    * @param game The Game object
    * @return A list containing the modified x distance and y distance to the target.
    */
    public ArrayList<Integer> getTarget(Game game) {
        ArrayList<Integer> lengthDifferences = new ArrayList<Integer>();
        lengthDifferences = this.target(game);
        
        // Randomise positions if not standard movement
        if (this.ghostMode == 1 || this.ghostMode == 2) {
            Random random = new Random();
            // Random number between -27*16 and 27*16
            lengthDifferences.set(0, random.nextInt(2 * 27 * game.GRIDSPACE) - 27 * game.GRIDSPACE);
            // Random number between -35*16 and 35*16
            lengthDifferences.set(1, random.nextInt(2 * 35 * game.GRIDSPACE) - 35 * game.GRIDSPACE);
        }

        // Setting targets within the grid bounds if outside
        int xTarget = lengthDifferences.get(0) + this.getCx(game.GHOSTOFFSET);
        int yTarget = lengthDifferences.get(1) + this.getCy(game.GHOSTOFFSET);
        if (xTarget < 0) {
            lengthDifferences.set(0, 0 - this.getCx(game.GHOSTOFFSET));
        } 
        if (xTarget > 27 * game.GRIDSPACE) {
            lengthDifferences.set(0, 27 * game.GRIDSPACE - this.getCx(game.GHOSTOFFSET));
        }
        if (yTarget < 0) {
            lengthDifferences.set(1, 0 - this.getCy(game.GHOSTOFFSET));
        } 
        if (yTarget > 35 * game.GRIDSPACE) {
            lengthDifferences.set(1, 35 * game.GRIDSPACE - this.getCy(game.GHOSTOFFSET));
        }
        return lengthDifferences;
    }

    // ____________________________________________________________________________________________

    /**
    * Using the difference in lengths from the ghost's target, derive a hierarchy of directions for 
    * which a ghost should decide to move in at an intersection.
    * @param game The Game object
    */
    public void setTargetDirection(Game game) {
        // Consider movement direction only at the middle of a cell
        if (this.isCentre(game.GHOSTOFFSET) == false) {
            return;
        }
        ArrayList<Integer> lengthDifferences = this.getTarget(game);
        int xDifference = lengthDifferences.get(0);
        int yDifference = lengthDifferences.get(1);
        ArrayList<String> directionList = new ArrayList<String>();

        // First determine the magnitude of difference in terms of x and y
        // Then determine whether the target is left, right, above or below
        if (Math.abs(xDifference) >= Math.abs(yDifference)) { // Move left or right first
            if (xDifference >= 0) {
                directionList.add("right");
            } else {
                directionList.add("left");
            }
            if (yDifference >= 0) {
                directionList.add("down");
                directionList.add("up");
                if (directionList.contains("right")) {
                    directionList.add("left");
                } else {
                    directionList.add("right");
                }
            } else {
                directionList.add("up");
                directionList.add("down");
                if (directionList.contains("right")) {
                    directionList.add("left");
                } else {
                    directionList.add("right");
                }
            }
        } else { // Move up or down first
            if (yDifference >= 0) {
                directionList.add("down");
            } else {
                directionList.add("up");
            }
            if (xDifference >= 0) {
                directionList.add("right");
                directionList.add("left");
                if (directionList.contains("down")) {
                    directionList.add("up");
                } else {
                    directionList.add("down");
                }
            } else {
                directionList.add("left");
                directionList.add("right");
                if (directionList.contains("down")) {
                    directionList.add("up");
                } else {
                    directionList.add("down");
                }
            }
        }
        this.directionList = directionList;
    }

    // ____________________________________________________________________________________________

    /**
    * Iterate through the ghost's direction hierarchy and attempt to follow it in order, if it is possible to
    * turn in that direction. 
    * <br>
    * Additionally allows ghosts to move backwards from where they came from only if it's a dead-end.
    * <br>
    * <br>
    * NOTE: Calls the method conductReverseMovement() within this method.
    * @param game The Game object
    */
    public void followTarget(Game game) {
        // Consider movement direction only at the middle of a cell
        if (this.isCentre(game.GHOSTOFFSET) == false) {
            return;
        }

        for (String direction : this.directionList) {
            int counter = 0;
            for (Wall wall : game.wallList) {
                // LEFT
                if (direction == "left") {
                    // Consider the cell to the left of the current
                    if (wall.x == this.getCx(game.GHOSTOFFSET) - 16 && wall.y == this.getCy(game.GHOSTOFFSET)) {
                        break;
                    }
                    // If a wall in this direction is not found and not moving in opposite direction, turn
                    if (counter == game.wallList.size()-1 && this.xVelocity <= 0) {
                        this.setSpeed(-game.speed, 0);
                        return;
                    }
                }
                // RIGHT
                else if (direction == "right") {
                    // Consider the cell to the right of the current
                    if (wall.x == this.getCx(game.GHOSTOFFSET) + 16 && wall.y == this.getCy(game.GHOSTOFFSET)) {
                        break;
                    }
                    // If a wall in this direction is not found and not moving in opposite direction, turn
                    if (counter == game.wallList.size()-1 && this.xVelocity >= 0) {
                        this.setSpeed(game.speed, 0);
                        return;
                    }
                }
                // UP
                else if (direction == "up") {
                    // Consider the cell above the current
                    if (wall.x == this.getCx(game.GHOSTOFFSET) && wall.y == this.getCy(game.GHOSTOFFSET) - 16) {
                        break;
                    }
                    // If a wall in this direction is not found, turn
                    if (counter == game.wallList.size()-1 && this.yVelocity <= 0) {
                        this.setSpeed(0, -game.speed);
                        return;
                    }
                }
                // DOWN
                else if (direction == "down") {
                    // Consider the cell to the right of the current
                    if (wall.x == this.getCx(game.GHOSTOFFSET) && wall.y == this.getCy(game.GHOSTOFFSET) + 16) {
                        break;
                    }
                    // If a wall in this direction is not found and not moving in opposite direction, turn
                    if (counter == game.wallList.size()-1 && this.yVelocity >= 0) {
                        this.setSpeed(0, game.speed);
                        return;
                    }
                }
                counter += 1;
            }
        }

        // ________________________________________________________________________________________

        this.conductReverseMovement(game);
    }

    /**
    * Allow ghosts to rebound in the direction they came from if and only if they reach a dead-end.
    * <br>
    * <br>
    * NOTE: This method is called within followTarget()
    * @param game The Game object
    */
    public void conductReverseMovement(Game game) {
        if (xVelocity > 0) { // Currently moving RIGHT
            this.setSpeed(-game.speed, 0);
        } else if (xVelocity < 0) { // Currently moving LEFT
            this.setSpeed(game.speed, 0);
        } else if (yVelocity > 0) { // Currently moving DOWN
            this.setSpeed(0, -game.speed);
        } else if (yVelocity < 0) { // Currently moving UP
            this.setSpeed(0, game.speed);
        } 
    }
}