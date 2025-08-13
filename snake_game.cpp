#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <sstream>

enum class GameState {
    MENU,
    PLAYING,
    PAUSED,
    GAME_OVER
};

enum class Direction {
    UP, DOWN, LEFT, RIGHT
};

struct Position {
    int x, y;
    Position(int x = 0, int y = 0) : x(x), y(y) {}
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};

class SnakeGame {
private:
    static const int WINDOW_WIDTH = 800;
    static const int WINDOW_HEIGHT = 600;
    static const int CELL_SIZE = 20;
    static const int GRID_WIDTH = WINDOW_WIDTH / CELL_SIZE;
    static const int GRID_HEIGHT = WINDOW_HEIGHT / CELL_SIZE;

    sf::RenderWindow window;
    sf::Font font;
    GameState currentState;
    
    // Snake properties
    std::vector<Position> snake;
    Direction direction;
    Direction nextDirection;
    
    // Food properties
    Position food;
    Position specialFood;
    bool hasSpecialFood;
    sf::Clock specialFoodTimer;
    
    // Game mechanics
    int score;
    int highScore;
    float gameSpeed;
    sf::Clock gameClock;
    
    // Random number generation
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<> xDist;
    std::uniform_int_distribution<> yDist;

public:
    SnakeGame() : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Snake Game"),
                  currentState(GameState::MENU),
                  direction(Direction::RIGHT),
                  nextDirection(Direction::RIGHT),
                  score(0),
                  highScore(0),
                  gameSpeed(200.0f),
                  hasSpecialFood(false),
                  gen(rd()),
                  xDist(1, GRID_WIDTH - 2),
                  yDist(1, GRID_HEIGHT - 2) {
        
        window.setFramerateLimit(60);
        loadFont();
        loadHighScore();
        initializeGame();
    }

    void loadFont() {
        // Try to load a system font, fallback to default if not available
        if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf") &&
            !font.loadFromFile("/System/Library/Fonts/Arial.ttf") &&
            !font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf")) {
            std::cout << "Warning: Could not load font, using default font" << std::endl;
        }
    }

    void loadHighScore() {
        std::ifstream file("highscore.txt");
        if (file.is_open()) {
            file >> highScore;
            file.close();
        }
    }

    void saveHighScore() {
        std::ofstream file("highscore.txt");
        if (file.is_open()) {
            file << highScore;
            file.close();
        }
    }

    void initializeGame() {
        snake.clear();
        snake.push_back(Position(GRID_WIDTH / 2, GRID_HEIGHT / 2));
        snake.push_back(Position(GRID_WIDTH / 2 - 1, GRID_HEIGHT / 2));
        snake.push_back(Position(GRID_WIDTH / 2 - 2, GRID_HEIGHT / 2));
        
        direction = Direction::RIGHT;
        nextDirection = Direction::RIGHT;
        score = 0;
        gameSpeed = 200.0f;
        hasSpecialFood = false;
        
        generateFood();
    }

    void generateFood() {
        do {
            food.x = xDist(gen);
            food.y = yDist(gen);
        } while (isSnakePosition(food));
    }

    void generateSpecialFood() {
        do {
            specialFood.x = xDist(gen);
            specialFood.y = yDist(gen);
        } while (isSnakePosition(specialFood) || specialFood == food);
        
        hasSpecialFood = true;
        specialFoodTimer.restart();
    }

    bool isSnakePosition(const Position& pos) {
        for (const auto& segment : snake) {
            if (segment == pos) return true;
        }
        return false;
    }

    void handleInput() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;
            }

            if (event.type == sf::Event::KeyPressed) {
                switch (currentState) {
                    case GameState::MENU:
                        handleMenuInput(event.key.code);
                        break;
                    case GameState::PLAYING:
                        handlePlayingInput(event.key.code);
                        break;
                    case GameState::PAUSED:
                        handlePausedInput(event.key.code);
                        break;
                    case GameState::GAME_OVER:
                        handleGameOverInput(event.key.code);
                        break;
                }
            }
        }
    }

    void handleMenuInput(sf::Keyboard::Key key) {
        if (key == sf::Keyboard::Enter || key == sf::Keyboard::Space) {
            currentState = GameState::PLAYING;
            initializeGame();
        } else if (key == sf::Keyboard::Escape || key == sf::Keyboard::Q) {
            window.close();
        }
    }

    void handlePlayingInput(sf::Keyboard::Key key) {
        if (key == sf::Keyboard::Space) {
            currentState = GameState::PAUSED;
            return;
        }

        // Movement controls
        switch (key) {
            case sf::Keyboard::Up:
            case sf::Keyboard::W:
                if (direction != Direction::DOWN) nextDirection = Direction::UP;
                break;
            case sf::Keyboard::Down:
            case sf::Keyboard::S:
                if (direction != Direction::UP) nextDirection = Direction::DOWN;
                break;
            case sf::Keyboard::Left:
            case sf::Keyboard::A:
                if (direction != Direction::RIGHT) nextDirection = Direction::LEFT;
                break;
            case sf::Keyboard::Right:
            case sf::Keyboard::D:
                if (direction != Direction::LEFT) nextDirection = Direction::RIGHT;
                break;
            default:
                break;
        }
    }

    void handlePausedInput(sf::Keyboard::Key key) {
        if (key == sf::Keyboard::Space || key == sf::Keyboard::R) {
            currentState = GameState::PLAYING;
        } else if (key == sf::Keyboard::Q || key == sf::Keyboard::Escape) {
            currentState = GameState::MENU;
        }
    }

    void handleGameOverInput(sf::Keyboard::Key key) {
        if (key == sf::Keyboard::Enter || key == sf::Keyboard::Space) {
            currentState = GameState::MENU;
        }
    }

    void update() {
        if (currentState != GameState::PLAYING) return;

        if (gameClock.getElapsedTime().asMilliseconds() >= gameSpeed) {
            direction = nextDirection;
            moveSnake();
            checkCollisions();
            checkFood();
            updateSpecialFood();
            gameClock.restart();
        }
    }

    void moveSnake() {
        Position newHead = snake[0];
        
        switch (direction) {
            case Direction::UP:    newHead.y--; break;
            case Direction::DOWN:  newHead.y++; break;
            case Direction::LEFT:  newHead.x--; break;
            case Direction::RIGHT: newHead.x++; break;
        }
        
        snake.insert(snake.begin(), newHead);
        snake.pop_back();
    }

    void checkCollisions() {
        Position head = snake[0];
        
        // Wall collision
        if (head.x <= 0 || head.x >= GRID_WIDTH - 1 || 
            head.y <= 0 || head.y >= GRID_HEIGHT - 1) {
            gameOver();
            return;
        }
        
        // Self collision
        for (size_t i = 1; i < snake.size(); ++i) {
            if (snake[i] == head) {
                gameOver();
                return;
            }
        }
    }

    void checkFood() {
        Position head = snake[0];
        
        // Regular food
        if (head == food) {
            score++;
            snake.push_back(snake.back()); // Grow snake
            generateFood();
            
            // Increase speed slightly
            if (gameSpeed > 50.0f) {
                gameSpeed -= 5.0f;
            }
            
            // Generate special food every 10 points
            if (score % 10 == 0) {
                generateSpecialFood();
            }
        }
        
        // Special food
        if (hasSpecialFood && head == specialFood) {
            score += 5;
            // Grow snake by 2 segments for special food
            snake.push_back(snake.back());
            snake.push_back(snake.back());
            hasSpecialFood = false;
        }
    }

    void updateSpecialFood() {
        if (hasSpecialFood && specialFoodTimer.getElapsedTime().asSeconds() >= 5.0f) {
            hasSpecialFood = false;
        }
    }

    void gameOver() {
        if (score > highScore) {
            highScore = score;
            saveHighScore();
        }
        currentState = GameState::GAME_OVER;
    }

    void render() {
        window.clear(sf::Color::Black);
        
        switch (currentState) {
            case GameState::MENU:
                renderMenu();
                break;
            case GameState::PLAYING:
                renderGame();
                break;
            case GameState::PAUSED:
                renderPaused();
                break;
            case GameState::GAME_OVER:
                renderGameOver();
                break;
        }
        
        window.display();
    }

    void renderMenu() {
        sf::Text title("SNAKE GAME", font, 72);
        title.setFillColor(sf::Color::Green);
        title.setPosition(WINDOW_WIDTH / 2 - title.getGlobalBounds().width / 2, 150);
        window.draw(title);
        
        sf::Text playText("Press ENTER or SPACE to Play", font, 24);
        playText.setFillColor(sf::Color::White);
        playText.setPosition(WINDOW_WIDTH / 2 - playText.getGlobalBounds().width / 2, 300);
        window.draw(playText);
        
        sf::Text quitText("Press Q or ESC to Quit", font, 24);
        quitText.setFillColor(sf::Color::White);
        quitText.setPosition(WINDOW_WIDTH / 2 - quitText.getGlobalBounds().width / 2, 350);
        window.draw(quitText);
        
        if (highScore > 0) {
            sf::Text highScoreText("High Score: " + std::to_string(highScore), font, 24);
            highScoreText.setFillColor(sf::Color::Yellow);
            highScoreText.setPosition(WINDOW_WIDTH / 2 - highScoreText.getGlobalBounds().width / 2, 450);
            window.draw(highScoreText);
        }
    }

    void renderGame() {
        // Draw walls
        sf::RectangleShape wall(sf::Vector2f(CELL_SIZE, CELL_SIZE));
        wall.setFillColor(sf::Color::Blue);
        
        // Top and bottom walls
        for (int x = 0; x < GRID_WIDTH; ++x) {
            wall.setPosition(x * CELL_SIZE, 0);
            window.draw(wall);
            wall.setPosition(x * CELL_SIZE, (GRID_HEIGHT - 1) * CELL_SIZE);
            window.draw(wall);
        }
        
        // Left and right walls
        for (int y = 0; y < GRID_HEIGHT; ++y) {
            wall.setPosition(0, y * CELL_SIZE);
            window.draw(wall);
            wall.setPosition((GRID_WIDTH - 1) * CELL_SIZE, y * CELL_SIZE);
            window.draw(wall);
        }
        
        // Draw snake
        sf::RectangleShape snakeSegment(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));
        snakeSegment.setFillColor(sf::Color(144, 238, 144)); // Light green
        
        for (const auto& segment : snake) {
            snakeSegment.setPosition(segment.x * CELL_SIZE, segment.y * CELL_SIZE);
            window.draw(snakeSegment);
        }
        
        // Draw regular food
        sf::RectangleShape foodRect(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));
        foodRect.setFillColor(sf::Color::Red);
        foodRect.setPosition(food.x * CELL_SIZE, food.y * CELL_SIZE);
        window.draw(foodRect);
        
        // Draw special food
        if (hasSpecialFood) {
            sf::RectangleShape specialFoodRect(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));
            specialFoodRect.setFillColor(sf::Color::Magenta);
            specialFoodRect.setPosition(specialFood.x * CELL_SIZE, specialFood.y * CELL_SIZE);
            window.draw(specialFoodRect);
        }
        
        // Draw score
        sf::Text scoreText("Score: " + std::to_string(score), font, 24);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(10, 10);
        window.draw(scoreText);
    }

    void renderPaused() {
        renderGame(); // Draw game in background
        
        // Draw pause overlay
        sf::RectangleShape overlay(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
        overlay.setFillColor(sf::Color(0, 0, 0, 128));
        window.draw(overlay);
        
        sf::Text pausedText("PAUSED", font, 48);
        pausedText.setFillColor(sf::Color::White);
        pausedText.setPosition(WINDOW_WIDTH / 2 - pausedText.getGlobalBounds().width / 2, 200);
        window.draw(pausedText);
        
        sf::Text scoreText("Score: " + std::to_string(score), font, 24);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(WINDOW_WIDTH / 2 - scoreText.getGlobalBounds().width / 2, 280);
        window.draw(scoreText);
        
        sf::Text resumeText("Press SPACE or R to Resume", font, 20);
        resumeText.setFillColor(sf::Color::White);
        resumeText.setPosition(WINDOW_WIDTH / 2 - resumeText.getGlobalBounds().width / 2, 350);
        window.draw(resumeText);
        
        sf::Text quitText("Press Q or ESC to Quit", font, 20);
        quitText.setFillColor(sf::Color::White);
        quitText.setPosition(WINDOW_WIDTH / 2 - quitText.getGlobalBounds().width / 2, 380);
        window.draw(quitText);
    }

    void renderGameOver() {
        sf::Text gameOverText("GAME OVER", font, 48);
        gameOverText.setFillColor(sf::Color::Red);
        gameOverText.setPosition(WINDOW_WIDTH / 2 - gameOverText.getGlobalBounds().width / 2, 200);
        window.draw(gameOverText);
        
        sf::Text finalScoreText("Final Score: " + std::to_string(score), font, 24);
        finalScoreText.setFillColor(sf::Color::White);
        finalScoreText.setPosition(WINDOW_WIDTH / 2 - finalScoreText.getGlobalBounds().width / 2, 280);
        window.draw(finalScoreText);
        
        sf::Text highScoreText("High Score: " + std::to_string(highScore), font, 24);
        highScoreText.setFillColor(sf::Color::Yellow);
        highScoreText.setPosition(WINDOW_WIDTH / 2 - highScoreText.getGlobalBounds().width / 2, 320);
        window.draw(highScoreText);
        
        sf::Text returnText("Press ENTER or SPACE to Return to Menu", font, 20);
        returnText.setFillColor(sf::Color::White);
        returnText.setPosition(WINDOW_WIDTH / 2 - returnText.getGlobalBounds().width / 2, 400);
        window.draw(returnText);
    }

    void run() {
        while (window.isOpen()) {
            handleInput();
            update();
            render();
        }
    }
};

int main() {
    try {
        SnakeGame game;
        game.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    
    return 0;
}