package game;

import processing.core.PImage;
import java.util.*;

/**
* The Ambusher ghost type.
*/
public class Ambusher extends Ghost {
    /**
    * Instantiates the ghost.
    * @param x The x coordinate
    * @param y The y coordinate
    * @param sprite The object's sprite
    */
    public Ambusher(int x, int y, PImage sprite) {
        super(x, y, sprite);
    }

    @Override
    public ArrayList<Integer> target(Game game) {
        int xDifference = 0;
        int yDifference = 0;

        if (game.mode.equals("chase")) {
            // Target - 4 grids ahead of Waka's movement direction
            // If Waka is not moving, Waka's position
            xDifference = (game.waka.getCx(game.WAKAOFFSET) + 4 * game.GRIDSPACE * Integer.signum(game.waka.xVelocity)
                - this.getCx(game.GHOSTOFFSET));
            yDifference = (game.waka.getCy(game.WAKAOFFSET) + 4 * game.GRIDSPACE * Integer.signum(game.waka.yVelocity)
                - this.getCy(game.GHOSTOFFSET));
        } else if (game.mode.equals("scatter")) {
            // Target - Top right corner
            xDifference = (27*game.GRIDSPACE - this.getCx(game.GHOSTOFFSET));
            yDifference = (0 - this.getCy(game.GHOSTOFFSET));
        }

        ArrayList<Integer> lengthDifferences = new ArrayList<Integer>(Arrays.asList(xDifference, yDifference));
        return lengthDifferences;
    }

}