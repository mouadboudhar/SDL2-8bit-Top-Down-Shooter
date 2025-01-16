#include "game.h"

RigidBody *initBodyPosition(RigidBody *body, float x, float y)
{
    body->x = x;
    body->y = y;
    return body;
}

RigidBody *initBodyPhysics(RigidBody *body, float velocityX, float velocityY, float acceleration, float maxSpeed, float friction)
{
    body->velX = velocityX;
    body->velY = velocityY;
    body->acceleration = acceleration;
    body->maxSpeed = maxSpeed;
    body->friction = friction;
    return body;
}

RigidBody *initBodyHpDmgSt(RigidBody *body, float damage, int maxhealth, int health, bool state)
{
    body->damage = damage;
    body->maxhealth = maxhealth;
    body->health = health;
    body->state = state;
    return body;
}

RigidBody *initBody(float x, float y, float velocityX, float velocityY, float acceleration, float maxSpeed, float friction, float damage, int maxhealth, int health, bool state)
{
    RigidBody *body = malloc(sizeof(RigidBody));
    if (body == NULL)
    {
        printf("Failed to allocate memory for body\n");
        exit(1);
    }
    initBodyPosition(body, x, y);
    initBodyPhysics(body, velocityX, velocityY, acceleration, maxSpeed, friction);
    initBodyHpDmgSt(body, damage, maxhealth, health, state);
    return body;
}