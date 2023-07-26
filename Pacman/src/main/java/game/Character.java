package game;

import processing.core.PImage;
import java.util.*;

// Interface from which Waka and Ghost implements
// In addition to these standard methods there are movement and collision methods
/**
* Interface defining standard methods for the Waka and ghosts characters of the game.
*/
public interface Character {
    // Method to change the attributes of the character
    /**
    * Conduct all character functionalities and update character attributes.
    * @param app The App object
    * @param game The Game object
    */
    public void tick(App app, Game game);

    // Method to render the sprites
    /**
    * Draws all aspects of the character to the screen.
    * @param app The App object.
    * @param game The game object.
    */
    public void draw(App app, Game game);

    // Compact method to change speed in one line
    /**
    * Compact method to change the current speed of the character.
    * @param xVelocity The new velocity in the x direction
    * @param yVelocity The new velocity in the y direction
    */
    public void setSpeed(int xVelocity, int yVelocity);

    // Method to change the sprite according to animation and modes
    /**
    * Change the character's sprite given its current attributes.
    */
    public void changeSprite();

    // Method to reset the positions when Waka loses a life
    /**
    * Reset the character's attributes to what they were at instantiation.
    */
    public void reset();
}