package game;

import processing.core.PImage;

/**
* Superfruit class.
*/
public class SuperFruit extends Fruit {
    /**
    * Initialises variables of the superfruit object.
    * @param x The x coordinate
    * @param y The y coordinate
    * @param sprite The object's sprite
    */
    public SuperFruit(int x, int y, PImage sprite) {
        super(x, y, sprite);
        this.offset = 4;
    }

    @Override
    public void draw(App app) {
        // Increase the size of the sprite
        this.sprite.resize(32, 32);
        app.image(this.sprite, this.x, this.y);
    }

    @Override
    public void collide(App app, Game game) {
        game.consumableList.remove(this);
        game.specialMode(app, "frightened");
    }
}