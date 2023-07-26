package game;

import processing.core.PImage;
import java.lang.Math;
import java.util.*;

/**
* The Whim ghost type.
*/
public class Whim extends Ghost {
    /**
    * Instantiates the ghost.
    * @param x The x coordinate
    * @param y The y coordinate
    * @param sprite The object's sprite
    */
    public Whim(int x, int y, PImage sprite) {
        super(x, y, sprite);
    }

    @Override
    public ArrayList<Integer> target(Game game) {
        int xDifference = 0;
        int yDifference = 0;

        if (game.mode.equals("chase")) {
            // Target - Consider the vector from any single Chaser to 2 cells ahead of Waka in the 
            // direction of his movement and double it. Then consider from the position of Whim
            // If all Chasers are dead, target Waka's position instead.
            xDifference = (game.waka.getCx(game.WAKAOFFSET) - this.getCx(game.GHOSTOFFSET));
            yDifference = (game.waka.getCy(game.WAKAOFFSET) - this.getCy(game.GHOSTOFFSET));
            for (Ghost g : game.ghostList) {
                if (g instanceof Chaser && g.alive == true) {
                    // Consider the difference between current Chaser position and 2 cells ahead of Waka, then multiply by 2
                    int xChaserDifference = 2 * (game.waka.getCx(game.WAKAOFFSET) + 
                        2 * game.GRIDSPACE * Integer.signum(game.waka.xVelocity) - g.getCx(game.GHOSTOFFSET));
                    int yChaserDifference = 2 * (game.waka.getCy(game.WAKAOFFSET) + 
                        2 * game.GRIDSPACE * Integer.signum(game.waka.yVelocity) - g.getCy(game.GHOSTOFFSET));
                    // Using the chaser's position to consider the target in relation to Whim
                    xDifference = (g.getCx(game.GHOSTOFFSET) + xChaserDifference - this.getCx(game.GHOSTOFFSET));
                    yDifference = (g.getCy(game.GHOSTOFFSET) + yChaserDifference - this.getCy(game.GHOSTOFFSET));
                    break;
                }
            }
        } else if (game.mode.equals("scatter")) {
            // Target - Bottom right corner
            xDifference = (27*game.GRIDSPACE - this.getCx(game.GHOSTOFFSET));
            yDifference = (35*game.GRIDSPACE - this.getCy(game.GHOSTOFFSET));
        }

        ArrayList<Integer> lengthDifferences = new ArrayList<Integer>(Arrays.asList(xDifference, yDifference));
        return lengthDifferences;
    }
}