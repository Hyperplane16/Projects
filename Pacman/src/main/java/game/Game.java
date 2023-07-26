package game;

import processing.core.PFont;
import java.util.*;
import java.io.*;

/**
* General game manager class which stores all the states and attributes about the game session and
* directs all functions of the game.
*/
public class Game {
    // Constant Variables
    /**
    * The size of each cell within the gridspace.
    */
    public static final int GRIDSPACE = 16; // Size of each grid space
    /**
    * The offset value for Waka's drawing position.
    */
    public static final int WAKAOFFSET = 4; // Offset when drawing Waka's sprite
    /** 
    * The offset value for ghost drawing positions.
    */
    public static final int GHOSTOFFSET = 5; // Offset when drawing the ghost sprite
    /** 
    * The App object to be stored within game.
    */
    public App app;


    // Game Attributes (parsed from file)
    /**
    * The name of the map file.
    */
    public String mapfile;
    /**
    * The number of lives Waka has.
    */
    public int lives;
    /**
    * The speed that Waka and the ghosts move at.
    */
    public int speed;
    /**
    * The number of seconds that frightened mode lasts for. 
    */
    public int frightenedLength;
    /**
    * The number of seconds that invisible mode lasts for.
    */
    public int invisibleLength;
    /**
    * A list of numbers specifying the number of seconds each scatter and chase mode lasts. 
    * (starts at scatter)
    */
    public int[] modeLengths;
    /**
    * An index pointing to the current number to be used within modeLengths. 
    */
    public int modeIndex; // Index used to traverse through modeLengths


    // Object Containers
    /**
    * The PFont object for the text font of the end screens.
    */
    public PFont font;
    /**
    * The Waka object. 
    */
    public Waka waka;
    /**
    * A list storing all the individual ghost objects in the game.
    */
    public ArrayList<Ghost> ghostList;
    /**
    * A list storing all the consumable objects in the game.
    */
    public ArrayList<Consumable> consumableList;
    /**
    * A list storing all the wall objects in the game.
    */
    public ArrayList<Wall> wallList;


    // Miscellaneous
    /**
    * A timer used to animate Waka's mouth.
    */
    public int timerWaka;
    /**
    * A timer used to count the number of seconds of each ghost mode.
    */
    public int timerMode;
    /**
    * A timer saving the difference in time until the next ghost mode to help pause the current 
    * mode during a special mode.
    */
    public int timerModeDifference;
    /**
    * A timer used to count the number of seconds for each special mode.
    */
    public int timerSpecial;
    /**
    * A timer to count the number of seconds after the game ends until it is reset.
    */
    public int timerEnd;

    
    /**
    * Stores the state of the game as an integer.
    * <br>
    * (0 = in-progress, 1 = victory, 2 = defeat)
    */
    public int state; // State of the game: 0 = in-progress, 1 = victory, 2 = defeat
    /**
    * Key which stores the direction of the most recent keyboard move command.
    * <br>
    * (left = 37, up = 38, right = 39, down = 40)
    */
    public int moveKey; // Left - 37, Up - 38, Right - 39, Down - 40
    /**
    * Key which stores the most recent keyboard key to be compared with the debug mode's key.
    * <br>
    * (space-bar = 32) 
    */
    public int commandKey; // Space = 32
    /**
    * Boolean for whether the debug mode is currently active. 
    */
    public boolean debugMode; // Whether debug mode is active
    /**
    * String of the ghost mode currently in progress.
    * <br>
    * (scatter, chaser, frightened, invisible)
    */
    public String mode; // "scatter", "frightened", "invisible" or "chase" modes
    /**
    * String saving the previous standard ghost mode to revert back to from a special ghost mode.
    */
    public String previousMode; // The previous mode, used to revert back from specia;

    // ____________________________________________________________________________________________

    /**
    * Constructor that parses and initialises all game data.
    * @param app The App object
    * @param filename The name of the configuration file
    */
    public Game(App app, String filename) {
        this.app = app;
        // Parse the json, map and font files
        Map.parseData(this, filename);
        Map.parseObject(app, this);

        // Initialise game parameters
        this.state = 0; 
        this.moveKey = 37; // Movement starts to the left
        this.debugMode = false;
        this.mode = "scatter";
        this.previousMode = "scatter";

        // Setting the Timers
        this.timerWaka = app.millis();
        this.timerMode = app.millis();
        this.timerModeDifference = 0;
        this.timerSpecial = app.millis();
        this.timerEnd = app.millis();
    }

    /**
    * An overloaded constructor for testing purposes so game attributes can be individually
    * instantiated.
    * @param overloaded Any string (to simply signify that the overloaded constructor is to be used)
    */
    public Game(String overloaded) {} // OVERLOADED Constructor for testing purposes

    // ____________________________________________________________________________________________

    /**
    * Conduct all game functionalities and update all game attributes.
    * @param app The App object
    */
    public void tick(App app) {
        this.checkEnd(app); // Check if the game has been completed
        if (this.state != 0) { // If the game has ended suspend game
            return;
        }

        this.manageTimers(app); // Manage timers 
        // Tick Waka and ghost characteristics
        // Collision and targeting are conducted within these tick() methods
        this.waka.tick(app, this);
        for (Ghost g : this.ghostList) {
            g.tick(app, this);
        }

        // Toggle debug mode
        if (this.commandKey == 32) { // Space bar keypress
            if (this.debugMode == true) {
                this.debugMode = false;
                this.commandKey = 0; // Reset Key
            } else {
                this.debugMode = true;
                this.commandKey = 0; // Reset Key
            }
        }   
    }

    /**
    * Draw all the elements of the game to the screen.
    * @param app The App object
    */
    public void draw(App app) {
        // Draw all objects (walls, fruit, Waka and all ghosts)
        Map.draw(app, this);
        for (Consumable c : this.consumableList) {
            c.draw(app);
        }
        this.waka.draw(app, this);
        for (Ghost g : this.ghostList) {
            g.draw(app, this);
        }

        this.drawLives(app); // Draw the number of Wakas at the bottom on the screen as lives
        this.displayEnd(app); // Draw the ending screen
    }

    // ____________________________________________________________________________________________

    /**
    * Check if the game has ended by fruit collection, loss of lives or all ghosts dead.
    * <br>
    * If so start the timer counting the number of seconds until the game is to be reset.
    * @param app The app object
    */
    public void checkEnd(App app) {
        // Eating all the fruits
        if (this.consumableList.size() == 0 && this.state == 0) {
            this.state = 1;
            this.timerEnd = app.millis();
        }
        // Losing all Waka's lives
        else if (this.lives == 0 && this.state == 0) {
            this.state = 2;
            this.timerEnd = app.millis();
        }
        // Eating all the ghosts
        boolean allDead = true;
        for (Ghost g : this.ghostList) {
            if (g.alive == true) {
                allDead = false;
                break;
            }
        }
        if (allDead == true && this.state == 0) {
            this.state = 1;
            this.timerEnd = app.millis();
        }
        // If the game has ended and 10 seconds has passed, restart the game entirely
        if (this.state != 0 && app.millis() > this.timerEnd + 10000) {
            app.game = new Game(app, "config.json");
            return;            
        }
    }

    /**
    * Display the ending screen including text.
    * @param app The App object
    */
    public void displayEnd(App app) {
        if (this.state == 1) {
            app.background(0, 0, 0); // Black background
            app.textFont(this.font); // Setting text font
            app.text("YOU WIN", 130, 240);
        } else if (this.state == 2) {
            app.background(0, 0, 0); // Black background
            app.textFont(this.font); // Setting text font
            app.text("GAME OVER", 120, 240);
        }
    } 

    /**
    * Manage all timers within the game for modes and animations.
    * @param app The App object
    */
    public void manageTimers(App app) {
        // Timer for Waka's Mouth (8 frames, 133 milliseconds)
        if (app.millis() > this.timerWaka + 133) {
            this.waka.changeSprite();
            this.timerWaka = app.millis();
        }
        // Timer for Mode (modeLengths seconds)
        if (app.millis() > this.timerMode + 1000 * this.modeLengths[modeIndex]) {
            if (this.mode.equals("scatter")) {
                this.mode = "chase";
            } else {
                this.mode = "scatter";
            }
            this.modeIndex += 1;
            if (this.modeIndex >= this.modeLengths.length) { // Start from the beginning if reach the end
                this.modeIndex = 0;
                this.mode = "scatter";
            }
            this.previousMode = this.mode;
            this.timerMode = app.millis();
        }
        // Timer for Special mode
        if (!(this.mode.equals("scatter") || this.mode.equals("chase"))) {
            // Once the timer has been reached, revert to previous mode and ghost sprites
            int timeLength = 0;
            if (this.mode.equals("frightened")) {
                timeLength = this.frightenedLength;
            } else if (this.mode.equals("invisible")) {
                timeLength = this.invisibleLength;
            } else {
                return;
            }
            // While in special mode, standard mode changing is suspended
            this.timerMode = app.millis() - this.timerModeDifference;
            if (app.millis() > this.timerSpecial + 1000 * timeLength) {
                this.mode = this.previousMode; // Revert to previous mode
                for (Ghost g : this.ghostList) {
                    g.ghostMode = 0; // Revert to standard ghost mode
                    g.changeSprite();
                }
            }
        }        
    }

    // Lives counter
    /**
    * Draw the number of lives remaining as right-facing Waka sprites at the bottom of the screen.
    * @param app The App object
    */
    public void drawLives(App app) {
        for (int i = 0; i < this.lives; i++) {
            // NOTE: The screen width can only accomodate 17 Waka sprites across the width of the grid
            app.image(this.waka.playerRight, 6 + i * (this.GRIDSPACE + 10), app.HEIGHT - 2 * this.GRIDSPACE);
        }
    }  

    // Change special modes (frightened, invisible)
    /**
    * Change the current ghost mode to the specified special mode.
    * @param app The App object
    * @param specialMode A string specifying the name of the special mode to be changed to
    * (frightened, invisible)
    */
    public void specialMode(App app, String specialMode) {
        // If a special object is collected while their mode is active, don't overwrite previous mode
        if (this.mode.equals("scatter") || this.mode.equals("chase")) {
            this.previousMode = this.mode; // Saves the current mode so it can correctly revert back
        }
        this.timerSpecial = app.millis();
        // Saves the time until next periodic mode 
        this.timerModeDifference = app.millis() - this.timerMode;
        int ghostMode = 0; 
        if (specialMode.equals("frightened")) { // FRIGHTENED MODE
            this.mode = "frightened";
            ghostMode = 1; 
        } else if (specialMode.equals("invisible")) { // INVISIBLE MODE
            this.mode = "invisible";
            ghostMode = 2;
        }
        for (Ghost g : this.ghostList) { // Change all ghost sprites
            g.ghostMode = ghostMode;
            g.changeSprite();
        }
    }

    /**
    * Reset Waka position, all ghost positions and the current mode to what they were at the start of the game.
    */
    // After collision with ghost, reset positions
    public void resetSpawn() {
        this.waka.reset();
        for (Ghost g : this.ghostList) {
            g.reset();
            g.xVelocity = -this.speed;
        }
        this.moveKey = 37;

        // Reset mode progress of the game to the beginning
        this.mode = "scatter";
        this.previousMode = "scatter";
        this.modeIndex = 0;
        this.timerMode = this.app.millis();
    }
}