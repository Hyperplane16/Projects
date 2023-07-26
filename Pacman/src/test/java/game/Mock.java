package game;

import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

import processing.core.PApplet;

// File to put mocked App in to be shared across all test files
class mock {}

// Mocked App Class
class App extends PApplet {
    public Game game;
    public App() {}

    public void setup() {
        // Call the empty constructor of game so manual assignment of attributes
        // can be done for testing
        this.game = new Game("test");
    }
}