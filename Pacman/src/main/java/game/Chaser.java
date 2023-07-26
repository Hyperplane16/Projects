package game;

import processing.core.PImage;
import java.util.*;

/**
* The Chaser ghost type.
*/
public class Chaser extends Ghost {
    /**
    * Instantiates the ghost.
    * @param x The x coordinate
    * @param y The y coordinate
    * @param sprite The object's sprite
    */
    public Chaser(int x, int y, PImage sprite) {
        super(x, y, sprite);
    }

    @Override
    public ArrayList<Integer> target(Game game) {
        int xDifference = 0;
        int yDifference = 0;

        if (game.mode.equals("chase")) {
            // Target - Waka's Position
            xDifference = (game.waka.getCx(game.WAKAOFFSET) - this.getCx(game.GHOSTOFFSET));
            yDifference = (game.waka.getCy(game.WAKAOFFSET) - this.getCy(game.GHOSTOFFSET));
        } else if (game.mode.equals("scatter")) {
            // Target - Top left corner
            xDifference = (0 - this.getCx(game.GHOSTOFFSET));
            yDifference = (0 - this.getCy(game.GHOSTOFFSET));
        }

        ArrayList<Integer> lengthDifferences = new ArrayList<Integer>(Arrays.asList(xDifference, yDifference));
        return lengthDifferences;
    }
}