package game;

import processing.core.PImage;

/**
* Superclass for consumable objects from which fruit, superfruit and sodacan extend from.
*/
public abstract class Consumable extends GameObject {
    /**
    * Offset value when drawing.
    */
    int offset;

    /**
    * Initialises variables of the consumable object.
    * @param x The x coordinate
    * @param y The y coordinate
    * @param sprite The object's sprite
    */
    public Consumable(int x, int y, PImage sprite) {
        super(x, y, sprite);
    }

    // Method to be overriden that conducts actions based on a detected collision
    /**
    * When collision with Waka occurs modify the game state depending on the consumable.
    * @param app The App object
    * @param game The Game object
    */
    public abstract void collide(App app, Game game);
}