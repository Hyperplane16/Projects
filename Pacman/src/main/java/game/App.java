package game;

import processing.core.PApplet;

/**
* PApplet manager class which provides the interface between the game and the screen.
*/
public class App extends PApplet {
    /**
    * Width of the screen.
    */
    public static final int WIDTH = 448;
    /**
    * Height of the screen.
    */
    public static final int HEIGHT = 576;
    /**
    * The Game object.
    */
    public Game game;

    /**
    * Constructor for the App class (empty)
    */
    public App() {} // Empty Constructor

    // ____________________________________________________________________________________________

    /**
    * PApplet settings method which initialises basic settings for the screen.
    */
    public void settings() {
        size(WIDTH, HEIGHT);
    }

    /**
    * PApplet setup method which initialises basic attributes for the app.
    */
    public void setup() {
        frameRate(60);
        this.game = new Game(this, "config.json");
    }

    /**
    * PApplet settings method which draws all elements to the screen.
    */
    public void draw() { 
        background(0, 0, 0); // Black Background
        this.game.tick(this);
        this.game.draw(this);
    }

    /**
    * PApplet settings method which detects a keypress. Then it is saved to the game object.
    */
    public void keyPressed() {
        if (keyCode == 37 || keyCode == 38 || keyCode == 39 || keyCode == 40) {
            this.game.moveKey = keyCode;
        }
        this.game.commandKey = keyCode;
    }

    /**
    * MAIN METHOD: This is the entry-point for the program.
    * @param args System variables
    */
    public static void main(String[] args) {
        PApplet.main("game.App");
    }
}

// export DISPLAY=:0