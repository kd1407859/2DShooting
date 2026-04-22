#pragma once

class GameObject {
public:
    Math::Vector2 pos{0,0}; // À•W
    bool isDead = false; // ¶‘¶ƒtƒ‰ƒO

    virtual void Update() {}
    virtual void Draw() {}
    virtual void OnHit() {} // “–‚½‚Á‚½‚Ìˆ—
};