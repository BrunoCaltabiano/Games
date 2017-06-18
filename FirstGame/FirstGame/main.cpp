#include <SFML\Graphics.hpp>
#include <SFML\Window.hpp>

using namespace std;
using namespace sf;


//defines an immutable compile time value
constexpr unsigned int windowWidth{ 800 }, windowHeight{ 600 };
constexpr  float ballRadius{ 10.f }, ballVelocity{ 6.f };
constexpr float paddleWidth{ 60.f }, paddleHeight{ 20.f }, paddleVelocity{ 8.f };

//brick constants
constexpr float blockWidth{ 60.f }, blockHeight{ 20.f };
constexpr unsigned int countBlocksX{ 11 }, countBlocksY{ 4 };

//ball class
//struct = class but everything is public
struct Ball {
	//CircleShape is a SFML class
	//defines a renderable circle
	CircleShape shape;

	//velocity instance of sfml type
	//a vector defined in sfml
	Vector2f velocity{ -ballVelocity, -ballVelocity };

	//ball constructor
	//mX = original x coordinate
	//mY = original y coordinate

	Ball(float mX, float mY) {
		//define position radius color and origin
		// to shape
		shape.setPosition(mX, mY);
		shape.setRadius(ballRadius);
		shape.setOutlineThickness(1);
		shape.setOutlineColor(Color::Black);
		shape.setFillColor(Color::White);
		shape.setOrigin(ballRadius, ballRadius);
	}
	//update function updates the ball's
	//location 
	void update() {

		shape.move(velocity);

		//keeps ball in window
		//does this by checking if ball
		//is in window
		if (left() < 0) velocity.x = ballVelocity;

		//if ball is leaving top right we go
		//bottom left
		else if (right() > windowWidth) velocity.x = -ballVelocity;

		//same for top/bottom 
		if (top() < 0) velocity.y = ballVelocity;

		else if (bottom() > windowHeight) velocity.y = -ballVelocity;
	}

	//bounce "properties" functions for the ball
	//gets commonly used values
	float x() { return shape.getPosition().x; }
	float y() { return shape.getPosition().y; }
	float left() { return x() - shape.getRadius(); }
	float right() { return x() + shape.getRadius(); }
	float top() { return y() - shape.getRadius(); }
	float bottom() { return y() + shape.getRadius(); }
};
//paddle class
struct Paddle {
	//Creates rectangle shape object
	RectangleShape shape;
	//velocity object for the paddle
	Vector2f velocity;

	//paddle constructor
	Paddle(float mX, float mY) {
		shape.setPosition(mX, mY);
		shape.setSize({paddleWidth, paddleHeight});
		shape.setFillColor(Color::Black);
		shape.setOrigin(paddleWidth / 2.f, paddleHeight / 2.f);
	}

	void update() {
		shape.move(velocity);

		//get user input to move
		//also keeps paddle inside window
		if (Keyboard::isKeyPressed(Keyboard::Key::A)
			&& left() > 0) velocity.x = -paddleVelocity; 
		else if (Keyboard::isKeyPressed(Keyboard::Key::D)
			&& right() < windowWidth) velocity.x = paddleVelocity;

		//if no input stop moving 
		else velocity.x = 0;
	}
	float x() { return shape.getPosition().x; }
	float y() { return shape.getPosition().y; }
	float left() { return x() - shape.getSize().x / 2.f; }
	float right() { return x() + shape.getSize().x / 2.f; }
	float top() { return y() - shape.getSize().y / 2.f; }
	float bottom() { return y() + shape.getSize().y / 2.f; }
};

//class for bricks
struct Brick{
	
	RectangleShape shape;

	//this bool val checks if
	//a brick is being hit

	bool destroyed{ false };

	//brick constructor
	Brick(float mX, float mY) {
		shape.setPosition(mX, mY);
		shape.setSize({ blockWidth, blockHeight });
		shape.setFillColor(Color::Magenta);
		shape.setOrigin(blockWidth / 2.f, blockHeight / 2.f);
	}

	float x() { return shape.getPosition().x; }
	float y() { return shape.getPosition().y; }
	float left() { return x() - shape.getSize().x / 2.f; }
	float right() { return x() + shape.getSize().x / 2.f; }
	float top() { return y() - shape.getSize().y / 2.f; }
	float bottom() { return y() + shape.getSize().y / 2.f; }
};

//generic function template for collision checking
template<class T1, class T2> bool isIntersecting(T1& mA, T2& mB) {
	return mA.right() >= mB.left() && mA.left() <= mB.right()
		&& mA.bottom() >= mB.top() && mA.top() <= mB.bottom();
}

//paddle/ball collision checker
void testCollision(Paddle& mPaddle, Ball& mBall) {
	//if no intersection, get out of function
	if (!isIntersecting(mPaddle, mBall)) return;

	//otherwise push ball out
	mBall.velocity.y = -ballVelocity;

	//adjust direction of ball
	//upon hit on paddle
	if (mBall.x() < mPaddle.x()) mBall.velocity.x = -ballVelocity;
	else mBall.velocity.x = ballVelocity;
}

//ball brick collision
void testCollision(Brick& mBrick, Ball& mBall) {
	//if not intersecting, get out of func
	if (!isIntersecting(mBrick, mBall)) return;

	//otherwise the a brick is hit
	mBrick.destroyed = true;

	//calculates total brick intersection by
	//the ball
	float overlapLeft{ mBall.right() - mBrick.left() };
	float overlapRight{ mBrick.right() - mBall.left() };
	float overlapTop{ mBall.bottom() - mBrick.top() };
	float overlapBottom{ mBrick.bottom() - mBall.top() };

	//if magnitude of left overlap is < right
	//ball has hit brick from left
	bool ballFromLeft(abs(overlapLeft) < abs(overlapRight));

	//we can apply the same idea for top/bottom collision
	bool ballFromTop(abs(overlapTop) < abs(overlapBottom));

	//store min overlaps for x/y axes
	float minOverlapX{ ballFromLeft ? overlapLeft : overlapRight };
	float minOverlapY{ ballFromTop ? overlapTop : overlapBottom };

	//changes x or y velocities of the ball
	//based on realistic collision reaction
	if (abs(minOverlapX) < abs(minOverlapY))
		mBall.velocity.x = ballFromLeft ? -ballVelocity : ballVelocity;
	else
		mBall.velocity.y = ballFromTop ? -ballVelocity : ballVelocity;
}
void gameOver(Ball& mBall) {

	
	}

int main()
{

	//creates instance of the ball
	// renderes it to center of window
	Ball ball{windowWidth / 2, windowHeight / 2};

	//creates paddle instance
	//renderes to bottom center of window
	Paddle paddle{ windowWidth / 2, windowHeight - 50 };

	//Creates brick instance
	//std::vector contains any number of
	//instances
	vector<Brick> bricks;

	//for loop makes 2D bricks
	for (int iX{ 0 }; iX < countBlocksX; iX++) {
		for (int iY{ 0 }; iY < countBlocksY; iY++) {
			//emplace is a c++11 mechanic allowing constructing
			//object in a container
			bricks.emplace_back((iX + 1) * (blockWidth + 3) + 22,
				(iY + 2) * (blockHeight + 3));
		}
	}

	//creation of the game window
	RenderWindow window{ {windowWidth, windowHeight}, "Arkanoid - 1" };
	window.setFramerateLimit(60);


	//actual game loop
	while (true) {
		
		// clear window call
		window.clear(Color::White);
		
		//event object prevents screen from freezing
		Event event;
		window.pollEvent(event);
		//if esc key is pressed, then quit the game
		if (Keyboard::isKeyPressed(Keyboard::Key::Escape)) break;
		
		//calls update function
		//in game loop
		ball.update();
		
		paddle.update();

		if (ball.bottom() > windowHeight) break;

		//collision checker every loop
		testCollision(paddle, ball);

		//another foreach loop tests brick collisions
		for (auto& brick : bricks) testCollision(brick, ball);

		//STL algorithm erases destroyed bricks
		//from the brick vector using
		//c++11 lambda
		bricks.erase(remove_if(begin(bricks), end(bricks),
			[](const Brick& mBrick) {return mBrick.destroyed; }),
			end(bricks));
		//^erase function takes start iterator and end iterator
		//deletes every object between start and end
		//inside the vector
		//remove if takes range of bricks vector
		//'[]' removes all bricks in vector if it has been destroyed


		//draws ball
		window.draw(ball.shape);

		window.draw(paddle.shape);

		//must draw every brick in window
		//c++ foreach loop says:
		//draw every brick object in bricks
		for (auto& brick : bricks) window.draw(brick.shape);
		//call displays window
		window.display();
	}
	

	return 0;
}