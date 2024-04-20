
/////////////////////////////////// INCLUDE ////////////////////////////////////

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>

//////////////////////////////////// WINDOW ////////////////////////////////////

const int FRAME_RATE_LIMIT = 60;
sf::Vector2u SCREEN_SIZE(1280, 960);            //default
sf::RenderWindow window
        (sf::VideoMode(SCREEN_SIZE.x, SCREEN_SIZE.y), 
                       "PONG!", sf::Style::Close);

////////////////////////////////// BACKGROUND //////////////////////////////////

const sf::Color CANVAS_COLOR(0, 0, 0);
const int SCREEN_BORDER = 20;

//////////////////////////////////// PLAYER ////////////////////////////////////

const sf::Vector2f PLAYER_SIZE(50, 200);
sf::Vector2f PLAYER_SPEED(0, 500);
float maxDeviationDegree = 45;
#define LEFTUP W
#define LEFTDOWN S
#define RIGHTUP O
#define RIGHTDOWN L 

///////////////////////////////////// BALL /////////////////////////////////////

const sf::Vector2f BALL_SIZE(50, 50);
const float BALL_SPEED = 250;
const float BALL_SPEED_UP = 25;             
const float PI = 3.14159;
float START_DEGREE = 270;

//////////////////////////////////// SYSTEM ////////////////////////////////////

sf::Clock deltaClock;
float dt;
const std::string DATA_PATH = "res/";

////////////////////////////////////////////////////////////////////////////////

class Game{
        struct Ball
        {
            sf::Vector2f direction;
            sf::RectangleShape ball;
            sf::SoundBuffer ballSoundBuffer;
            sf::SoundBuffer scoreBuffer;
            sf::Sound wop;
            sf::Sound score;
            float ballDegree = START_DEGREE;
            Ball()
            {
                ball.setSize(BALL_SIZE);
                ball.setPosition(sf::Vector2f(SCREEN_SIZE.x/2-BALL_SIZE.x/2,
                 SCREEN_SIZE.y/2-BALL_SIZE.y/2));
                ball.setFillColor(sf::Color::White);
                direction.x = BALL_SPEED * sin((PI/180) * ballDegree);
                direction.y = BALL_SPEED * -cos((PI/180) * ballDegree);
                ballSoundBuffer.loadFromFile(DATA_PATH + "wall.wav");
                scoreBuffer.loadFromFile(DATA_PATH+ "score.wav");
                wop.setBuffer(ballSoundBuffer);
                score.setBuffer(scoreBuffer);
            }
            void checkBorder()
            {
                if(ball.getPosition().y+ball.getSize().y > SCREEN_SIZE.y || ball.getPosition().y < 0)
                {
                    direction.y*=-1;
                    wop.play();
                    speedUp();
                }
                if(ball.getPosition().x+ball.getSize().x > SCREEN_SIZE.x || ball.getPosition().x < 0)
                {
                    reset();
                    score.play();
                }
            }
            void update()
            {
                ball.move(direction * dt);
                checkBorder();
            }
            void draw()
            {
                window.draw(ball);
            }
            sf::FloatRect getBounds() 
            {
                return ball.getGlobalBounds();
            }
            void speedUp()
            {
                if(direction.x > 0)
                    direction.x += BALL_SPEED_UP;
                else 
                    direction.x -= BALL_SPEED_UP;
                if(direction.y > 0)
                    direction.y += BALL_SPEED_UP;
                else 
                    direction.y -= BALL_SPEED_UP;
            }
            void reset()
            {
                ball.setPosition(sf::Vector2f(SCREEN_SIZE.x/2-BALL_SIZE.x/2,
                                              SCREEN_SIZE.y/2-BALL_SIZE.y/2));
                
                do {
                    ballDegree = rand() % 360;
                } while(ballDegree == 0);
                

                direction.x = BALL_SPEED * sin((PI/180) * ballDegree);
                direction.y = BALL_SPEED * -cos((PI/180) * ballDegree);
            }
            
        };
        struct Player
        {
            bool moveingUp = 0;
            bool moveingDown = 0;
            sf::RectangleShape player;
            sf::RectangleShape redGluck;
            sf::RectangleShape blueGluck;
            
            Player(bool side)
            {
                player.setSize(PLAYER_SIZE);
                if(side)
                    player.setPosition(sf::Vector2f(SCREEN_BORDER,
                     SCREEN_SIZE.y/2-PLAYER_SIZE.y/2));
                else
                    player.setPosition(sf::Vector2f(SCREEN_SIZE.x-SCREEN_BORDER-PLAYER_SIZE.x,
                     SCREEN_SIZE.y/2-PLAYER_SIZE.y/2));

                redGluck.setFillColor(sf::Color::Red);
                redGluck.setSize(PLAYER_SIZE);
                if(side)
                    redGluck.setPosition(sf::Vector2f(SCREEN_BORDER-1,
                     SCREEN_SIZE.y/2-PLAYER_SIZE.y/2));
                else
                    redGluck.setPosition(sf::Vector2f(SCREEN_SIZE.x-SCREEN_BORDER-PLAYER_SIZE.x-1,
                     SCREEN_SIZE.y/2-PLAYER_SIZE.y/2));

                blueGluck.setFillColor(sf::Color::Blue);
                blueGluck.setSize(PLAYER_SIZE);
                if(side)
                    blueGluck.setPosition(sf::Vector2f(SCREEN_BORDER+1,
                     SCREEN_SIZE.y/2-PLAYER_SIZE.y/2));
                else
                    blueGluck.setPosition(sf::Vector2f(SCREEN_SIZE.x-SCREEN_BORDER-PLAYER_SIZE.x+1,
                     SCREEN_SIZE.y/2-PLAYER_SIZE.y/2));                    
            }
            void draw()
            {
                window.draw(blueGluck);
                window.draw(redGluck);
                window.draw(player);
            }
            void moveUp()
            {
                if(moveingUp && player.getPosition().y - SCREEN_BORDER > 0)
                {
                    player.move(-PLAYER_SPEED * dt);
                    redGluck.move(-PLAYER_SPEED * dt);
                    blueGluck.move(-PLAYER_SPEED * dt);
                }
                    
            }
            void moveDown()
            {
                if(moveingDown && player.getPosition().y + player.getSize().y + SCREEN_BORDER < SCREEN_SIZE.y)
                {
                    player.move(PLAYER_SPEED * dt);
                    redGluck.move(PLAYER_SPEED * dt);
                    blueGluck.move(PLAYER_SPEED * dt);
                }
            }
            void update()
            {
                moveUp();
                moveDown();
            }

        };

    Player leftPlayer;
    Player rightPlayer;
    Ball ball;
    sf::SoundBuffer platformBuffer;
    sf::Sound platform;
    void events();
    void update();
    void render();
    void resize();
    void touching();
    void degreegen();
public:
    Game();
    void run();

};

Game::Game(): 
leftPlayer(1),
rightPlayer(0)
{
    window.setFramerateLimit(FRAME_RATE_LIMIT);
    platformBuffer.loadFromFile(DATA_PATH + "platform.wav");
    platform.setBuffer(platformBuffer);

    // resize();
    
}

void Game::run()
{
    while (window.isOpen())
    {
        events();
        update();
        render();
        dt = deltaClock.restart().asSeconds();
    }
}

void Game::events()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window.close();

///////////////////////////////// LEFT-PLAYER /////////////////////////////////

        if (event.type == sf::Event::EventType::KeyPressed && 
            event.key.code == sf::Keyboard::LEFTUP)
            leftPlayer.moveingUp = true;
        if (event.type == sf::Event::EventType::KeyReleased && 
            event.key.code == sf::Keyboard::LEFTUP)
            leftPlayer.moveingUp = false;
        if (event.type == sf::Event::EventType::KeyPressed && 
            event.key.code == sf::Keyboard::LEFTDOWN)
            leftPlayer.moveingDown = true;
        if (event.type == sf::Event::EventType::KeyReleased && 
            event.key.code == sf::Keyboard::LEFTDOWN)
            leftPlayer.moveingDown = false;
            
//////////////////////////////// RIGHT-PLAYER /////////////////////////////////

        if (event.type == sf::Event::EventType::KeyPressed && 
            event.key.code == sf::Keyboard::RIGHTUP)
            rightPlayer.moveingUp = true;
        if (event.type == sf::Event::EventType::KeyReleased && 
            event.key.code == sf::Keyboard::RIGHTUP)
            rightPlayer.moveingUp = false;
        if (event.type == sf::Event::EventType::KeyPressed && 
            event.key.code == sf::Keyboard::RIGHTDOWN)
            rightPlayer.moveingDown = true;
        if (event.type == sf::Event::EventType::KeyReleased && 
            event.key.code == sf::Keyboard::RIGHTDOWN)
            rightPlayer.moveingDown = false;

    }

}

void Game::update()
{
    leftPlayer.update();
    rightPlayer.update();
    ball.update();
    touching();
    
}

void Game::render()
{
    
    window.clear(CANVAS_COLOR);
    ball.draw();
    leftPlayer.draw();
    rightPlayer.draw();
    window.display();
    
}

void Game::touching()
{
    if(ball.ball.getGlobalBounds().intersects(leftPlayer.player.getGlobalBounds()))
    {
        if(ball.ball.getPosition().y + ball.ball.getSize().y >= leftPlayer.player.getPosition().y ||
           ball.ball.getPosition().y >= leftPlayer.player.getPosition().y + leftPlayer.player.getSize().y)
            {
                // left and right side hit 
                // varibal can be delete
                
                float DeviationDegree = 
                (((ball.ball.getPosition().y + ball.ball.getSize().y/2) -
                (leftPlayer.player.getPosition().y + leftPlayer.player.getSize().y/2)) / 
                (leftPlayer.player.getSize().y / 2)) * maxDeviationDegree;

                ball.direction.y += 180 * sin((PI/180) * DeviationDegree);
                if(ball.direction.x <= 0)
                    ball.direction.x *=-1;

                ball.direction.x += BALL_SPEED_UP;
                platform.play();
            }   
        //up and down hit
        /*
        
        nothing, but still work 
        
        */
        
    }

    
    if(ball.ball.getGlobalBounds().intersects(rightPlayer.player.getGlobalBounds()))
    {
        if(ball.ball.getPosition().y + ball.ball.getSize().y >= rightPlayer.player.getPosition().y ||
           ball.ball.getPosition().y >= rightPlayer.player.getPosition().y + rightPlayer.player.getSize().y)
            {
                // left and right side  hit
                // varibal can be delete
                
                float DeviationDegree = 
                (((ball.ball.getPosition().y + ball.ball.getSize().y/2) -
                (rightPlayer.player.getPosition().y + rightPlayer.player.getSize().y/2)) / 
                (rightPlayer.player.getSize().y / 2)) * maxDeviationDegree;

                ball.direction.y += 180 * sin((PI/180) * DeviationDegree);
                if(ball.direction.x >= 0)
                    ball.direction.x *=-1;
                ball.direction.x -= BALL_SPEED_UP;
                platform.play();
            }   
            //up and down hit
            /*
            
            nothing, but still work 
            
            */
    }
}


int main()
{

/////////////// from fork of this game ///////////////
    srand(time(NULL));
    do {
        START_DEGREE = rand() % 360; 
    } while(START_DEGREE == 0); 

//////////////////////////////////////////////////////

    Game game;
    game.run();
    return 0;
}

// resize feature was added in the fork of this game

// void Game::resize()
// {
//             SCREEN_SIZE = window.getSize();
//             sf::View view(sf::FloatRect(0, 0, SCREEN_SIZE.x, SCREEN_SIZE.y));
//             window.setView(view);
// }    //no resize


        ////////////////////////////////////////////////////////
        // if(event.type == sf::Event::Resized)
        //     resize(); // no resize


