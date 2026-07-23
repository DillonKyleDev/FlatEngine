#pragma once


namespace FlatEngine
{
    class Component;
    class Script;
}

namespace FL = FlatEngine;

namespace FlatGui
{
    namespace Inspector
    {
        extern void BeginComponent(FL::Component* component, FL::Component*& queuedForDelete);
        extern void EndComponent(FL::Component* component);
        extern void RenderScriptComponent(FL::Script* script);
    }
}