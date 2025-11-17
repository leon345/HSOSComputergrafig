#include "Player.h"

#include <array>
#include <cmath>
#include <vector>

#include "Coin.h"
#include "Model.h"
#include "Terrain.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * Verbesserungsvorschlag: für sin und cos https://www.perplexity.ai/search/6d0ff31e-c231-41bf-acca-eae1be0159b5
 */
Player::Player(std::list<BaseModel*>& models)
: position(-27.6f,23.0f, -44.3f), yaw(0.0f), pitch(0.0f), 
  movementSpeed(5.0f), rotationSpeed(1.0f),
  forwardBackward(0.0f), leftRight(0.0f), 
  mouseRotationX(0.0f), mouseRotationY(0.0f),
  velocity(0,0,0), acceleration(0,0,0),
  gravity(-9.81f), jumpHeight(2.0f), groundLevel(0.0f),
  isGrounded(true), jumpRequesting(false),
  m_WorldModels(models),
  startTip(0,1,0), startBase(0,-1,0), radius(0.5f),
  capsule(startTip + position, startBase + position, radius)
  
{
}

/**
 * TODO
 * Ansatz B: Velocity-Based höhe des Sprungs ermitteln
 * Laufgeschwindigkeit erhöhen und verlangsammen
 * Optional: doppelsprung, Sprung verlängern
 */
Player::~Player()
{
    
}

void Player::handleInput(GLFWwindow* window, float deltaTime)
{
    static  double lastX = 0.0f, lastY = 0.0f;
    forwardBackward = 0.0f;
    leftRight = 0.0f;

    static bool pWasPressed = false;
    bool pCurrentlyPressed = glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;

    if (pCurrentlyPressed && !pWasPressed)
    {
        setIsGamePaused(!getIsGamePaused());

        if (!getIsGamePaused())
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            lastX = xpos;
            lastY = ypos;
        } else
        {

            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            
        }
    }
    pWasPressed = pCurrentlyPressed;
    
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            forwardBackward = 1.5f;
        } else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        {
            forwardBackward = 0.4f;
        } else
        {
            forwardBackward = 1.0f;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        forwardBackward = -1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        leftRight = -1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        leftRight = 1.0f;
    }
    bool spaceCurrentlyPressed  = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
    if (spaceCurrentlyPressed && !spaceWasPressed && isGrounded)
    {
        jumpRequesting = true;
    }

    spaceWasPressed = spaceCurrentlyPressed;
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);


    double xoffset = xpos - lastX;
    double yoffset = -(ypos - lastY);

    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.003f;

    yaw += static_cast<float>(xoffset) * sensitivity;
    pitch += static_cast<float>(yoffset) * sensitivity;


    if (pitch >= M_PI/2)  pitch = M_PI/2 - 1e-3f;
    if (pitch <= -M_PI/2) pitch = -M_PI/2 + 1e-3f;
    //TODO Maus imput
}

void Player::update(float deltaTime)
{
    if (getIsGamePaused())
    {
        Vector forward(
        cosf(pitch) * cosf(yaw),
        sinf(pitch),
        cosf(pitch) * sinf(yaw)
    );
        forward.normalize();

        // Rechtsvektor:
        Vector right(-sinf(yaw), 0, cosf(yaw));

        // Bewegung:
        Vector horizontalMovement = forward * forwardBackward   + right   * leftRight ;

        /*jump(velocity, acceleration, deltaTime, gravity, horizontalMovement);
        move(velocity, horizontalMovement, movementSpeed, deltaTime);*/
    
        handleJump(velocity, horizontalMovement, deltaTime);
        handleMovement(velocity, horizontalMovement, deltaTime);
        applyPhysics(velocity, acceleration, deltaTime);
        // Euler
        //position += velocity * deltaTime;

        position += velocity * deltaTime + acceleration * (deltaTime * deltaTime * 0.5f);

        // Kapsel updaten
        capsule.tip = startTip + position;
        capsule.base = startBase + position;
        capsule.radius = radius;

        checkCollisionWithTriangles(m_WorldModels);
        if (!isGrounded) handleGroundCollision();
    }
    

    //std::cout << "Position: " <<position.X << ", " << position.Y << ", " << position.Z << "\n";
}

Vector Player::getForwardDirection() const
{
    Vector fwd(
     cosf(pitch) * cosf(yaw),
     sinf(pitch),
     cosf(pitch) * sinf(yaw)
 );
    fwd.normalize();
    return fwd; //TODO sinus nicht performant
}

Matrix Player::getViewMatrix() const
{
    Vector forward = getForwardDirection();
    Vector target = position + forward;
    Vector up = Vector(0.0f, 1.0f, 0.0f);

    //LookAt Matrix
    Matrix view;
    view.identity();

    //
    Matrix translation;
    translation.translation(-position.X, -position.Y, -position.Z);

    //Rotattion
    Matrix rotation;
    rotation.rotationY(-yaw);

    view = rotation * translation;
    return view;
}


void Player::draw(const BaseCamera& Cam)
{
    //TODO vielleicht Haende oder Gegenstände rendern
}

void Player::handleJump(Vector& velocity, const Vector& horizontalMovement, float deltaTime)
{
    if (jumpRequesting && isGrounded)
    {
        float initialJumpVelocity = sqrtf( -2.0f * gravity * jumpHeight);
        velocity.Y = initialJumpVelocity;
        
        velocity.X = horizontalMovement.X * movementSpeed;
        velocity.Z = horizontalMovement.Z * movementSpeed;
        
        isGrounded = false;
        jumpRequesting = false;
    }
}

void Player::handleMovement(Vector& velocity, const Vector& horizontalMovement, float deltaTime)
{
    if (isGrounded)
    {
        velocity.X = horizontalMovement.X * movementSpeed;
        velocity.Z = horizontalMovement.Z * movementSpeed;
    }
    else
    {
        velocity.X += horizontalMovement.X * movementSpeed * airControlFactor * deltaTime;
        velocity.Z += horizontalMovement.Z * movementSpeed * airControlFactor * deltaTime;
    }
}

void Player::applyPhysics(Vector& velocity, Vector& acceleration, float deltaTime)
{
    acceleration = Vector(0.0f, gravity, 0.0f);
    velocity.Y += acceleration.Y * deltaTime;
}

void Player::handleGroundCollision() //TODO richtige Collision verwenden dann
{
    float lowest = capsule.base.Y - capsule.radius;
    if (lowest <= groundLevel)
    {
        float penetration = groundLevel - lowest;
        position.Y += penetration;
        
        capsule.tip = startTip + position;
        capsule.base = startBase + position;

        velocity.Y = 0.0f;
        isGrounded = true;
    }
    else
    {
        isGrounded = false;
    }
}


AABB Player::playerQueryAABB(float searchRadius) const
{
    float r = capsule.radius + searchRadius;
    Vector minPt(std::min(capsule.tip.X, capsule.base.X),
                 std::min(capsule.tip.Y, capsule.base.Y),
                 std::min(capsule.tip.Z, capsule.base.Z));
    Vector maxPt(std::max(capsule.tip.X, capsule.base.X),
                 std::max(capsule.tip.Y, capsule.base.Y),
                 std::max(capsule.tip.Z, capsule.base.Z));
    minPt = minPt - Vector(r,r,r);
    maxPt = maxPt + Vector(r,r,r);
    return AABB(minPt, maxPt);
}

void Player::checkCollisionWithTriangles(std::list<BaseModel*>& models)
{
    const float searchRadius = 0.05f; 
    AABB queryBox = playerQueryAABB(searchRadius);

    std::vector<std::array<Vector, 3>> nearbyTriangles;
    nearbyTriangles.reserve(512);

    for (auto it = models.begin(); it != models.end(); )
    {
        BaseModel *model = *it;
        AABB modelBox = model->boundingBox();

        if (!modelBox.intersects(queryBox))
        {
            ++it;
            continue;
        }

        if (model->getIsCollectable() && capsule.intersectsAABB(modelBox))
        {
            Coin* coin = dynamic_cast<Coin*>(model);
            if (coin)
            {
                ++collectedCoins;
                coin->collect();
            }
        }

        Terrain* terrain = dynamic_cast<Terrain*>(model);
        if (terrain)
        {
            terrain->getTrianglesInAABB(queryBox, nearbyTriangles);
            ++it;
            continue;
        }

        if (model->getIsColldeable() && capsule.intersectsAABB(modelBox))
        {
            resolveCapsuleVsAABB(modelBox);
        }
        model->getTrianglesInAABB(queryBox, nearbyTriangles);
        ++it;
    }
    
    
    for (const auto& tri : nearbyTriangles)
    {
        Vector normal;
        float penetration;
        if (capsule.intersectsTriangle(tri[0], tri[1], tri[2], normal, penetration))
        {
            position += normal * penetration;
            capsule.tip  += normal * penetration;
            capsule.base += normal * penetration;

            float vn = velocity.dot(normal);
            if (vn < 0.0f) velocity = velocity - (normal * vn);

            const float triGroundThreshold = 0.5f;
            if (normal.Y > triGroundThreshold)
            {
                isGrounded = true;
                velocity.Y = 0.0f;
            }
        }
    }
}

void Player::resolveCapsuleVsAABB(const AABB& box)
{
    // Kapsel-AABB
    Vector capMin(std::min(capsule.tip.X, capsule.base.X),
                  std::min(capsule.tip.Y, capsule.base.Y),
                  std::min(capsule.tip.Z, capsule.base.Z));
    Vector capMax(std::max(capsule.tip.X, capsule.base.X),
                  std::max(capsule.tip.Y, capsule.base.Y),
                  std::max(capsule.tip.Z, capsule.base.Z));

    capMin = capMin - Vector(capsule.radius, capsule.radius, capsule.radius);
    capMax = capMax + Vector(capsule.radius, capsule.radius, capsule.radius);

    // Prüfe Überlappung (AABB vs AABB)
    if (capMax.X < box.Min.X || capMin.X > box.Max.X ||
        capMax.Y < box.Min.Y || capMin.Y > box.Max.Y ||
        capMax.Z < box.Min.Z || capMin.Z > box.Max.Z) {
        return; // kein Overlap
    }

    // Berechne Penetration pro Achse (positive Werte = Überlappung)
    float overlapX = std::min(capMax.X, box.Max.X) - std::max(capMin.X, box.Min.X);
    float overlapY = std::min(capMax.Y, box.Max.Y) - std::max(capMin.Y, box.Min.Y);
    float overlapZ = std::min(capMax.Z, box.Max.Z) - std::max(capMin.Z, box.Min.Z);

    // kleinste Überlappung wählen
    float minOverlap = overlapX;
    int axis = 0; // 0 = X, 1 = Y, 2 = Z
    if (overlapY < minOverlap) { minOverlap = overlapY; axis = 1; }
    if (overlapZ < minOverlap) { minOverlap = overlapZ; axis = 2; }

    Vector normal(0.0f, 0.0f, 0.0f);
    if (axis == 0) {
        float capCenterX = (capMin.X + capMax.X) * 0.5f;
        float boxCenterX = (box.Min.X + box.Max.X) * 0.5f;
        normal.X = (capCenterX < boxCenterX) ? -1.0f : 1.0f;
    } else if (axis == 1) {
        float capCenterY = (capMin.Y + capMax.Y) * 0.5f;
        float boxCenterY = (box.Min.Y + box.Max.Y) * 0.5f;
        normal.Y = (capCenterY < boxCenterY) ? -1.0f : 1.0f;
    } else {
        float capCenterZ = (capMin.Z + capMax.Z) * 0.5f;
        float boxCenterZ = (box.Min.Z + box.Max.Z) * 0.5f;
        normal.Z = (capCenterZ < boxCenterZ) ? -1.0f : 1.0f;
    }
    
    position += normal * minOverlap;
    
    capsule.tip = startTip + position;
    capsule.base = startBase + position;

    // Velocity korrigieren (entferne Einwärtskomponente)
    float vn = velocity.dot(normal);
    if (vn < 0.0f) {
        velocity = velocity - (normal * vn);
    }

    // Setze grounded wenn Normal nach oben zeigt
    const float groundNormalThreshhold = 0.9f;
    if (axis == 1 && normal.Y >= groundNormalThreshhold)
    {
        float desiredBaseY = box.Max.Y + capsule.radius;
        float deltaY = desiredBaseY - capsule.base.Y;
        if (std::fabs(deltaY) > 1e-6f) {
            position.Y += deltaY;
            capsule.tip = startTip + position;
            capsule.base = startBase + position;
        }
        
        isGrounded = true;
        if (velocity.Y < 0.0f) velocity.Y = 0.0f;
    } else {
        // falls nicht Boden, keine Änderung an isGrounded hier (bleibt wie vorher)
    }
    
}