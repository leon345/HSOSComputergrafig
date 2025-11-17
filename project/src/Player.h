#ifndef Player_hpp
#define Player_hpp
#include <list>
#include <vector>
#include <array>
#include <GL/glew.h>
#include <glfw/glfw3.h>

#include "basemodel.h"
#include "camera.h"
#include "CapsuleCollision.h"
#include "Matrix.h"
#include "vector.h"

class Player
{
public:
    Player(std::list<BaseModel*>& models);
    virtual ~Player();
    
    // WASD-Steuerung (ähnlich wie Tank::steer)
    void handleInput(GLFWwindow* window, float deltaTime);
    void update(float deltaTime);
    
    // First-Person Kamera-Funktionen
    Vector getPosition() const { return position; }
    Vector getForwardDirection() const;
    Matrix getViewMatrix() const;

    bool getIsGamePaused() const { return isGamePaused; }
    void setIsGamePaused(bool paused) { isGamePaused = paused; }
   
    
    // Bewegungsparameter
    void setMovementSpeed(float speed) { movementSpeed = speed; }
    void setRotationSpeed(float speed) { rotationSpeed = speed; }

    unsigned int getCollectedCoins() { return collectedCoins; }
    void setCollectedCoins(unsigned int number) { collectedCoins = number; }
    
    virtual void draw(const BaseCamera& Cam);

private:
    Vector position;        
    float yaw;             
    float pitch;           
    
   
    float movementSpeed;   
    float rotationSpeed;
    float airControlFactor = 0.3f;
    
    // Bewegungszustände (für zeitbasierte Animation)
    float forwardBackward; // -1.0 bis 1.0
    float leftRight;       // -1.0 bis 1.0
    float mouseRotationX;  // Für Maus-Look (optional)
    float mouseRotationY;

    Vector velocity;
    Vector acceleration;
    bool isGrounded = true;
    bool jumpRequesting = false;
    bool canJump = true;
    bool spaceWasPressed = false;

    float gravity;
    float jumpHeight;
    float groundLevel;

    bool isGamePaused = false;

    // Capsule Collision
    CapsuleCollision capsule;
    Vector startTip;
    Vector startBase;
    float radius;

    std::list<BaseModel*> m_WorldModels;

    unsigned int collectedCoins = 0;


    /*void move(Vector& velocity, const Vector& horizontalMovement, float movementSpeed, float deltaTime);
    void jump(Vector& velocity, Vector& acceleration, float deltaTime, float gravity, const Vector& horizontalMovement);*/

    void handleJump(Vector& velocity, const Vector& horizontalInput, float deltaTime);
    void handleMovement(Vector& velocity, const Vector& horizontalInput, float deltaTime);
    void applyPhysics(Vector& velocity, Vector& acceleration, float deltaTime);
    void handleGroundCollision();
    AABB playerQueryAABB(float searchRadius) const;
    void checkCollisionWithTriangles(std::list<BaseModel*>& models);
    void resolveCapsuleVsAABB(const AABB& box);

    /*
     * TOdo
     * Formel für Projektilbewebung implementieren
     *  p0 und v0 getrennt berechnen
     */
};


#endif // !Player_hpp
