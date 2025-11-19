#pragma once
class Actor;

class Component 
{
public:
    explicit Component(Actor* owner) : owner(owner) {}
    virtual ~Component() = default;
    virtual void Update(float dt) {}

    Actor* GetOwner() { return owner; }
protected:
    Actor* owner;
};
