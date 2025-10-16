#pragma once
#include <vector>
#include <memory>
#include <string>
#include "Component.h"
#include "../Scene/Light.h"

class LightManager;
//Actor本身并没有什么作用，主要作用是作为component的容器
class Actor {
public:
    //explicit作用为避免函数隐式调用
    explicit Actor(const std::string& name) : name(name) {}
    ~Actor() = default;

    template<typename T, typename... Args>
    T* AddComponent(Args&&... args)
    {
        auto comp = std::make_unique<T>(this, std::forward<Args>(args)...);
        T* ptr = comp.get();
        components.push_back(std::move(comp));//move为移动语义，将左值转化为右值引用

        if (auto* light = dynamic_cast<Light*>(ptr))
        {
            LightManager::Get().registerLight(this, light);
        }
        return ptr;
    }

    template<typename T>
    T* GetComponent()
    {
        for (auto& c : components)
        {
            if (auto* p = dynamic_cast<T*>(c.get()))
                return p;
        }
        return nullptr;
    }

    template<typename T>
    std::vector<T*> GetComponents() {
        std::vector<T*> result;
        for (auto& c : components) {
            if (auto* p = dynamic_cast<T*>(c.get())) {
                result.push_back(p);
            }
        }
        return result;
    }

    void Update(float dt) { for (auto& c : components) c->Update(dt); }

    std::string getName() const { return name; }
private:
    std::string name;
    std::vector<std::unique_ptr<Component>> components;
};