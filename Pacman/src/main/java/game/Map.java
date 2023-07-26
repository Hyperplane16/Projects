package game;

import processing.core.PImage;
import org.json.simple.*;
import org.json.simple.parser.*;
import java.util.*;
import java.io.*;

/**
* Stores static methods for parsing configuration and map files to be stored in the game object.
*/
public abstract class Map {
    /**
    * Parse the map file and instantiate all the resource objects into their respective lists of 
    * the game object.
    * <br>
    * <br>
    * NOTE: This is the only location in the main source code files where resource sprites/fonts are
    * loaded.
    * @param app The App object
    * @param game The Game object
    */
    public static void parseObject(App app, Game game) {
        game.wallList = new ArrayList<Wall>();
        game.ghostList = new ArrayList<Ghost>();
        game.consumableList = new ArrayList<Consumable>();
        // Create the font for the end screen given a .ttf file
        game.font = app.createFont("PressStart2P-Regular.ttf", 24);

        File f = new File(game.mapfile);
        try {
            Scanner scan = new Scanner(f);
            // Column and row are reversed to fit the inverted drawing coordinates
            int y = 0;
            while (scan.hasNextLine() == true) { // Initiate cell objects for each line
                String[] line = scan.nextLine().split("");
                int x = 0;
                for (String s : line) {
                    if (s.equals("p")) { // Waka's Starting Point
                        game.waka = new Waka(x * game.GRIDSPACE - game.WAKAOFFSET, y * game.GRIDSPACE
                                - game.WAKAOFFSET);
                        game.waka.playerRight = app.loadImage("playerRight.png");
                        game.waka.playerLeft = app.loadImage("playerLeft.png");
                        game.waka.playerUp = app.loadImage("playerUp.png");
                        game.waka.playerDown = app.loadImage("playerDown.png");
                        game.waka.playerClosed = app.loadImage("playerClosed.png");
                        game.waka.sprite = game.waka.playerLeft;
                    } else if (s.equals("a")) { // Ambusher's Starting Point
                        Ambusher g = new Ambusher(x * game.GRIDSPACE - game.GHOSTOFFSET, y * game.GRIDSPACE
                                - game.GHOSTOFFSET, app.loadImage("ambusher.png"));
                        g.setSpeed(-game.speed, 0);
                        g.frightenedSprite = app.loadImage("frightened.png");
                        g.invisibleSprite = app.loadImage("invisible.png");
                        game.ghostList.add(g);
                    } else if (s.equals("c")) { // Chaser's Starting Point
                        Chaser g = new Chaser(x * game.GRIDSPACE - game.GHOSTOFFSET, y * game.GRIDSPACE
                                - game.GHOSTOFFSET, app.loadImage("chaser.png"));
                        g.setSpeed(-game.speed, 0);
                        g.frightenedSprite = app.loadImage("frightened.png");
                        g.invisibleSprite = app.loadImage("invisible.png");
                        game.ghostList.add(g);
                    } else if (s.equals("i")) { // Ignorant's Starting Point
                        Ignorant g = new Ignorant(x * game.GRIDSPACE - game.GHOSTOFFSET, y * game.GRIDSPACE
                                - game.GHOSTOFFSET, app.loadImage("ignorant.png"));
                        g.setSpeed(-game.speed, 0);
                        g.frightenedSprite = app.loadImage("frightened.png");
                        g.invisibleSprite = app.loadImage("invisible.png");
                        game.ghostList.add(g);
                    } else if (s.equals("w")) { // Whim's Starting Point
                        Whim g = new Whim(x * game.GRIDSPACE - game.GHOSTOFFSET, y * game.GRIDSPACE
                                - game.GHOSTOFFSET, app.loadImage("whim.png"));
                        g.setSpeed(-game.speed, 0);
                        g.frightenedSprite = app.loadImage("frightened.png");
                        g.invisibleSprite = app.loadImage("invisible.png");
                        game.ghostList.add(g);
                    } else if (s.equals("1")) { // Horizontal Wall
                        game.wallList.add(new Wall(x * game.GRIDSPACE, y * game.GRIDSPACE, 
                                app.loadImage("horizontal.png")));
                    } else if (s.equals("2")) { // Vertical Wall
                        game.wallList.add(new Wall(x * game.GRIDSPACE, y * game.GRIDSPACE, 
                                app.loadImage("vertical.png")));
                    } else if (s.equals("3")) { // upLeft Wall
                        game.wallList.add(new Wall(x * game.GRIDSPACE, y * game.GRIDSPACE, 
                                app.loadImage("upLeft.png")));
                    } else if (s.equals("4")) { // upRight Wall
                        game.wallList.add(new Wall(x * game.GRIDSPACE, y * game.GRIDSPACE, 
                                app.loadImage("upRight.png")));
                    } else if (s.equals("5")) { // downLeft Wall
                        game.wallList.add(new Wall(x * game.GRIDSPACE, y * game.GRIDSPACE, 
                                app.loadImage("downLeft.png")));
                    } else if (s.equals("6")) { // downRight Wall
                        game.wallList.add(new Wall(x * game.GRIDSPACE, y * game.GRIDSPACE, 
                                app.loadImage("downRight.png")));
                    } else if (s.equals("7")) { // Fruit
                        game.consumableList.add(new Fruit(x * game.GRIDSPACE, y * game.GRIDSPACE, 
                                app.loadImage("fruit.png")));
                    } else if (s.equals("8")) { // SuperFruit
                        // Offset values to centre the super fruit cherry: 4
                        game.consumableList.add(new SuperFruit(x * game.GRIDSPACE - 4, y * game.GRIDSPACE - 4,
                                app.loadImage("superFruit.png")));
                    } else if (s.equals("9")) { // SodaCan
                        // Offset values to centre the soda can sprite: 4
                        game.consumableList.add(new SodaCan(x * game.GRIDSPACE - 4, y * game.GRIDSPACE - 4, 
                                app.loadImage("sodaCan.png")));
                    }
                    x += 1;
                }
                y += 1;
            }
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }
    } 

    /**
    * Parse the configuration file and set its data as the game's attributes.
    * @param game The Game object
    * @param filename The name of the configuration file
    */
    @SuppressWarnings("unchecked") // Passing from JSON object always produces an unchecked exception
    public static void parseData(Game game, String filename) {
        JSONParser parser = new JSONParser();
        try {
            // Use JSON.simple's parsing methods
            Reader read = new FileReader(filename);
            JSONObject obj = (JSONObject)parser.parse(read);

            // Retreive all data sections of the file
            String mapfile = (String)obj.get("map");
            Long livesLong = (Long)obj.get("lives");
            int lives = livesLong.intValue();
            Long speedLong = (Long)obj.get("speed");
            int speed = speedLong.intValue();
            Long frightenedLong = (Long)obj.get("frightenedLength");
            int frightenedLength = frightenedLong.intValue();
            Long invisibleLong = (Long)obj.get("invisibleLength");
            int invisibleLength = invisibleLong.intValue();

            JSONArray array = (JSONArray)obj.get("modeLengths");
            Iterator<Long> iterator = array.iterator();
            int[] modeLengths = new int[array.size()];
            int i = 0;
            while (iterator.hasNext()) { // Iterate through the modeLengths and add to array
                Long lengthLong = (Long)iterator.next();
                int length = lengthLong.intValue();
                modeLengths[i] = length;
                i += 1;
            }

            game.mapfile = mapfile;
            game.lives = lives;
            game.speed = speed;
            game.frightenedLength = frightenedLength;
            game.invisibleLength = invisibleLength;
            game.modeLengths = modeLengths;
            game.modeIndex = 0;
        } catch (IOException e) {
            e.printStackTrace();
        } catch (ParseException e) {
            e.printStackTrace();
        }
    }

    // ____________________________________________________________________________________________

    /**
    * Draw the map by iterating through each wall in the list of walls in the game and drawing the sprite.
    * @param app The App object
    * @param game The game object
    */
    public static void draw(App app, Game game) {
        for (Wall wall : game.wallList) {
            app.image(wall.sprite, wall.x, wall.y);
        }
    }
}