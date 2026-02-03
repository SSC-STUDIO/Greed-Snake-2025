// Snake behavior - handles player and AI snake logic
#include "Snake.h"
#include "../Core/Collisions.h"

Vector2 SnakeSegment::GetVelocity() const
{
    return direction.GetNormalize() * GameState::Instance().currentPlayerSpeed;
}

bool SnakeSegment::CanRecordPosition() const {
    return timeSinceLastRecord >= GameState::Instance().recordInterval;
}

void Snake::Update(float deltaTime)
{
    currentTime += deltaTime;

    if (IsBeginRecord()) {
        RecordPos();
        currentTime = 0;
    }
}

Vector2 Snake::GetVelocity() const
{
    return direction.GetNormalize() * GameState::Instance().currentPlayerSpeed;
}

bool Snake::IsBeginRecord() const
{
    return currentTime >= GameState::Instance().recordInterval;
}

void Snake::RecordPos()
{
    float distanceToLast = 0;
    if (!posRecords.empty()) {
        distanceToLast = (position - posRecords.back()).GetLength();
    }

    if (distanceToLast >= GameConfig::SNAKE_SEGMENT_SPACING) {
        posRecords.push(position);
        while (posRecords.size() > 2) {
            posRecords.pop();
        }
    }
}

Vector2 Snake::GetRecordTime() const
{
    if (posRecords.empty()) {
        return position;
    }
    return posRecords.front();
}

void Snake::UpdateBody(const Snake& lastBody, Snake& currentBody)
{
    Vector2 targetPos = lastBody.GetRecordTime();
    Vector2 direction = (targetPos - currentBody.position).GetNormalize();
    currentBody.position = targetPos - direction * GameConfig::SNAKE_SEGMENT_SPACING;
}

void Snake::Draw(const Camera& camera) const
{
    Vector2 windowPos = position - camera.position;
    DrawCircleWithCamera(windowPos, radius, color);
}

void PlayerSnake::Update(float deltaTime)
{
    Snake::Update(deltaTime);

    for (size_t i = 0; i < segments.size(); i++) {
        if (i == 0) {
            UpdateBody(*this, segments[i]);
        }
        else {
            UpdateBody(segments[i - 1], segments[i]);
        }
        segments[i].Update(deltaTime);
    }
}

void PlayerSnake::Draw(const Camera& camera) const
{
    int baseR = (color >> 16) & 0xFF;
    int baseG = (color >> 8) & 0xFF; 
    int baseB = color & 0xFF;

    for (size_t i = 0; i < segments.size(); i++) {
        const auto& segment = segments[i];
        Vector2 windowPos = segment.position - camera.position;
        
        float ratio = static_cast<float>(i) / segments.size();
        int segR = baseR - static_cast<int>(ratio * baseR * 0.3f);
        int segG = baseG - static_cast<int>(ratio * baseG * 0.3f);
        int segB = baseB - static_cast<int>(ratio * baseB * 0.3f);
        int segColor = RGB(segR, segG, segB);
        
        float sizeRatio = 1.0f - ratio * 0.3f;
        float segRadius = segment.radius * sizeRatio;
        
        setfillcolor(segColor);
        setlinecolor(segColor);
        fillcircle(windowPos.x, windowPos.y, segRadius);
    }

    Snake::Draw(camera);
    Vector2 windowPos = position - camera.position;
    DrawSnakeEyes(windowPos, direction, radius);
}

void AISnake::Initialize()
{
    isDying = false;
    deathTimer = 0.0f;
    dyingSegmentIndex = -1;
    
    segments.resize(5);
    
    for (auto& segment : segments) {
        segment.posRecords = std::queue<Vector2>();
    }
    
    while (!posRecords.empty()) {
        posRecords.pop();
    }
    
    currentTime = 0;
}

void AISnake::Update(const std::vector<FoodItem>& foodItems, float deltaTime, const Vector2& playerHeadPos)
{
    if (isDying) {
        UpdateDeathAnimation(deltaTime);
        return;
    }
    
    directionChangeTimer += deltaTime;
    
    if (directionChangeTimer >= GameConfig::AI_DIRECTION_CHANGE_TIME) {
        if (rand() % 100 < static_cast<int>(aggressionFactor * 100)) {
            Vector2 toPlayer = playerHeadPos - position;
            float distToPlayer = toPlayer.GetLength();
            
            if (distToPlayer < GameConfig::AI_VIEW_RANGE * 2) {
                direction = toPlayer.GetNormalize();
            } else {
                float angle = (rand() % 360) * 3.14159f / 180.0f;
                direction = Vector2(cos(angle), sin(angle));
            }
        } else {
            float closestDist = GameConfig::AI_VIEW_RANGE;
            Vector2 closestFood = position;
            bool foodFound = false;
            
            for (const auto& food : foodItems) {
                if (food.collisionRadius <= 0) continue;
                
                float dist = (food.position - position).GetLength();
                if (dist < closestDist) {
                    closestDist = dist;
                    closestFood = food.position;
                    foodFound = true;
                }
            }
            
            if (foodFound) {
                direction = (closestFood - position).GetNormalize();
            } else {
                float angle = (rand() % 360) * 3.14159f / 180.0f;
                direction = Vector2(cos(angle), sin(angle));
            }
        }
        
        directionChangeTimer = 0.0f;
    }
    
const float borderAvoidDistance = 200.0f;
    Vector2 borderAvoidDir(0, 0);
    int borderCount = 0;
    
    if (position.x - GameConfig::PLAY_AREA_LEFT < borderAvoidDistance) {
        borderAvoidDir.x += 1.0f;
        borderCount++;
    }
    
    if (GameConfig::PLAY_AREA_RIGHT - position.x < borderAvoidDistance) {
        borderAvoidDir.x -= 1.0f;
        borderCount++;
    }
    
    if (position.y - GameConfig::PLAY_AREA_TOP < borderAvoidDistance) {
        borderAvoidDir.y += 1.0f;
        borderCount++;
    }
    
    if (GameConfig::PLAY_AREA_BOTTOM - position.y < borderAvoidDistance) {
        borderAvoidDir.y -= 1.0f;
        borderCount++;
    }
    
    if (borderCount > 0) {
        borderAvoidDir = borderAvoidDir.GetNormalize();
        direction = (direction + borderAvoidDir * 3.0f).GetNormalize();
    }
    
    Vector2 velocity = direction.GetNormalize() * GameState::Instance().currentPlayerSpeed * 0.25f;
    position = position + velocity * deltaTime;
    
    Snake::Update(deltaTime);

    for (size_t i = 0; i < segments.size(); i++) {
        if (i == 0) {
            UpdateBody(*this, segments[i]);
        }
        else {
            UpdateBody(segments[i - 1], segments[i]);
        }
        segments[i].Update(deltaTime);
    }
    
    for (const auto& food : foodItems) {
        if (food.collisionRadius <= 0) continue;
        
        if (CheckCollisionWithPoint(food.position, food.collisionRadius)) {
            float growthAmount = food.collisionRadius / 10.0f;
            
            Snake newSegment;
            if (!segments.empty()) {
                newSegment = segments.back();
            } else {
                newSegment = *this;
            }
            
            if (!segments.empty()) {
                Vector2 lastSegPos = segments.back().position;
                Vector2 dir = segments.back().direction;
                newSegment.position = lastSegPos - dir * GameConfig::SNAKE_SEGMENT_SPACING;
            } else {
                Vector2 dir = direction;
                newSegment.position = position - dir * GameConfig::SNAKE_SEGMENT_SPACING;
            }
            
            segments.push_back(newSegment);
            
            const int maxSegments = 20;
            if (segments.size() > maxSegments) {
                segments.resize(maxSegments);
            }
            
            break;
        }
    }
}

void AISnake::UpdateDeathAnimation(float deltaTime) {
    deathTimer += deltaTime;
    
    int targetSegmentIndex = static_cast<int>(deathTimer / segmentFadeTime) - 1;
    
    if (targetSegmentIndex > dyingSegmentIndex) {
        dyingSegmentIndex = targetSegmentIndex;
        
        if (dyingSegmentIndex == -1) {
            radius = 0;
        }
        else if (dyingSegmentIndex >= 0 && dyingSegmentIndex < segments.size()) {
            if (GameConfig::SOUND_ON) {
                PlaySound(_T(".\\Resource\\SoundEffects\\Pop.wav"), NULL, SND_FILENAME | SND_ASYNC);
            }
            segments[dyingSegmentIndex].radius = 0;
        }
        else if (dyingSegmentIndex >= segments.size()) {
            for (auto& segment : segments) {
                segment.radius = 0;
            }
            isDying = false;
        }
    }
}

void AISnake::StartDying(int foodValue) {
    isDying = true;
    deathTimer = 0.0f;
    dyingSegmentIndex = -1;
    foodValueOnDeath = foodValue;
}

void AISnake::Draw(const Camera& camera) const
{
    int baseR = (color >> 16) & 0xFF;
    int baseG = (color >> 8) & 0xFF; 
    int baseB = color & 0xFF;

    for (size_t i = 0; i < segments.size(); i++) {
        const auto& segment = segments[i];
        Vector2 windowPos = segment.position - camera.position;
        
        float ratio = static_cast<float>(i) / segments.size();
        int segR = baseR - static_cast<int>(ratio * baseR * 0.3f);
        int segG = baseG - static_cast<int>(ratio * baseG * 0.3f);
        int segB = baseB - static_cast<int>(ratio * baseB * 0.3f);
        int segColor = RGB(segR, segG, segB);
        
        setfillcolor(segColor);
        setlinecolor(segColor);
        fillcircle(windowPos.x, windowPos.y, segment.radius);
    }

    Snake::Draw(camera);
    Vector2 windowPos = position - camera.position;
    DrawSnakeEyes(windowPos, direction, radius);
}

bool PlayerSnake::CheckCollisionWith(const Snake& other) const {
    if (Snake::CheckCollisionWith(other))
        return true;
        
    for (const auto& segment : segments) {
        if (CollisionManager::CheckCircleCollision(
                other.position, other.radius,
                segment.position, segment.radius)) {
            return true;
        }
    }
    
    return false;
}

bool AISnake::CheckCollisionWith(const Snake& other) const {
    if (Snake::CheckCollisionWith(other))
        return true;
        
    for (const auto& segment : segments) {
        if (CollisionManager::CheckCircleCollision(
                other.position, other.radius,
                segment.position, segment.radius)) {
            return true;
        }
    }
    
    return false;
}

bool Snake::CheckCollisionWith(const Snake& other) const {
    return CollisionManager::CheckCircleCollision(
        position, radius, other.position, other.radius);
}

bool Snake::CheckCollisionWithPoint(const Vector2& point, float pointRadius) const {
    return CollisionManager::CheckCircleCollision(
        position, radius, point, pointRadius);
}


