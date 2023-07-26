package game;

import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

import processing.core.PApplet;

class AppTest {
    @Test 
    public void constructorNull() {
        // Test whether App's constructor works correctly
        App appTest = new App();
        assertNotNull(appTest);
    }

    @Test
    public void gameObjectNull() {
        // Test whether the game object is correctly instantiated
        App appTest = new App();
        PApplet.runSketch(new String[]{""}, appTest);
        appTest.setup();
        appTest.game = new Game(appTest, "config.json");
        assertNotNull(appTest.game);
    }
}
