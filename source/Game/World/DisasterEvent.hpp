// Pure virtual class and functions
#pragma once

class DisasterEvent
{
public:
    virtual ~DisasterEvent()                = default;
    virtual void        OnStart()           = 0;
    virtual void        OnTick(double dt)   = 0;
    virtual void        OnEnd()             = 0;
    virtual bool        IsFinished() const  = 0;
    virtual void        Draw() const        = 0;
    virtual const char* GetName() const     = 0;
    virtual float       GetProgress() const = 0; // 0.0 ~ 1.0

    void SetDebugDraw(bool d) const { debugDraw = d; }

protected:
    mutable bool debugDraw = false;
};