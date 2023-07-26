package game;

import processing.core.PImage;

/**
* Soda can class.
*/
public class SodaCan extends Consumable {
    /**
    * Initialises variables of the soda can object.
    * @param x The x coordinate
    * @param y The y coordinate
    * @param sprite The object's sprite
    */
    public SodaCan(int x, int y, PImage sprite) {
        super(x, y, sprite);
        this.offset = 4;
    }

    @Override
    public void collide(App app, Game game) {
        game.consumableList.remove(this);
        game.specialMode(app, "invisible");
    }
}