package game;

import processing.core.PImage;

/**
* General purpose object superclass of all objects in the game.
*/
public abstract class GameObject {
    /**
    * The x coordinate of the object.
    */
    public int x;
    /**
    * The y coordinate of the object.
    */
    public int y;
    /**
    * The PImage sprite of the object.
    */
    public PImage sprite;
    /**
    * The initial x coordinate of the object at instantiation.
    */
    public int xSpawn;
    /**
    * The initial y coordinate of the object at instantiation. 
    */
    public int ySpawn;

    /**
    * General superclass constructor which is called by all game objects to instantiate its most basic
    * attributes.
    * @param x The x coordinate
    * @param y The y coordinate
    * @param sprite The sprite object
    */
    public GameObject(int x, int y, PImage sprite) {
        this.x = x;
        this.y = y;
        this.sprite = sprite;

        this.xSpawn = this.x;
        this.ySpawn = this.y;
    }

    /**
    * Draw the object using its sprite.
    * @param app The App object
    */
    public void draw(App app) {
        app.image(this.sprite, this.x, this.y);
    }

    // Getter methods to retrieve the coordinates of the cell the object is currently occupying.
    // For Waka and Ghosts their models are offset, so it needs to be re-added to compare to the cell coordinate
    // Where cells are 16x16 pixels.
    /**
    * Finds the x coordinate of the closest 16x16 cell by rounding down the current x position to the nearest 
    * multiple of 16.
    * @param offset The offset value of the object to when drawing the sprite (applies to ghost and Waka)
    * @return The x coordinate of the closest cell
    */
    public int getCx(int offset) {
        // Round the current position to the nearest top left corner.
        // double cell = (this.x + offset)/16;
        // int x = (int)cell * 16; // x of current cell by rounding down the decimal
        int x = (this.x + offset)/16 * 16;
        return x;
    } 

    /**
    * Finds the y coordinate of the closest 16x16 cell by rounding down the current y position to the nearest 
    * multiple of 16.
    * @param offset The offset value of the object to when drawing the sprite (applies to ghost and Waka)
    * @return The y coordinate of the closest cell
    */
    public int getCy(int offset) {
        // Round the current position to the nearest top left corner.
        // double cell = (this.y + offset)/16;
        // int y = (int)cell * 16; // y of current cell by rounding down the decimal
        int y = (this.y + offset)/16 * 16;
        return y;
    }

    // Method to determine whether any given object is in the centre of a cell, given a possible offset
    /**
    * Determines whether the given object is in the centre of a 16x16 cell.
    * @param offset The offset value of the object to when drawing the sprite (applies to ghost and Waka)
    * @return Whether the cell is at the centre or not
    */
    public boolean isCentre(int offset) {
        if (this.x + offset != this.getCx(offset) || this.y + offset != this.getCy(offset)) {
            return false;
        }
        return true;
    }
}