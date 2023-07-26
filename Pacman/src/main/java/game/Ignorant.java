package game;

import processing.core.PImage;
import java.lang.Math;
import java.util.*;

/**
* The Ignorant ghost type.
*/
public class Ignorant extends Ghost {
    /**
    * Instantiates the ghost.
    * @param x The x coordinate
    * @param y The y coordinate
    * @param sprite The object's sprite
    */
    public Ignorant(int x, int y, PImage sprite) {
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
        }
        // If scatter mode or less than 8 euclidean cells away, target is buttom left corner
        if (game.mode.equals("scatter") ||
            Math.sqrt(Math.pow(xDifference, 2) + Math.pow(yDifference, 2))/game.GRIDSPACE < 8) {
            // Target - Bottom left corner
            xDifference = (0 - this.getCx(game.GHOSTOFFSET));
            yDifference = (35*game.GRIDSPACE - this.getCy(game.GHOSTOFFSET));
        }

        ArrayList<Integer> lengthDifferences = new ArrayList<Integer>(Arrays.asList(xDifference, yDifference));
        return lengthDifferences;
    }
}