package game;

import processing.core.PImage;

/**
* Standard fruit class.
*/
public class Fruit extends Consumable {
    /**
    * Initialises variables of the fruit object.
    * @param x The x coordinate
    * @param y The y coordinate
    * @param sprite The object's sprite
    */
    public Fruit(int x, int y, PImage sprite) {
        super(x, y, sprite);
        this.offset = 0;
    }

    @Override
    public void collide(App app, Game game) {
        game.consumableList.remove(this); 
    }
}