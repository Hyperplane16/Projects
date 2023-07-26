package game;

import processing.core.PImage;

/**
* Wall object class.
*/
public class Wall extends GameObject {
    /**
    * Initialises variables of the soda can object.
    * @param x The x coordinate
    * @param y The y coordinate
    * @param sprite The object's sprite
    */
    public Wall(int x, int y, PImage sprite) {
        super(x, y, sprite);
    }
}