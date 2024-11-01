#pragma once

/**
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com )
 * @version 1.0
 * 
 * @section DESCRIPTION
 * 
 * Application class definition
 */

#include <string_view>

namespace Engine
{

    class Layer
    {
    public:
        virtual ~Layer() = default;
        virtual void Init() = 0;
        virtual void Destroy() = 0;
        virtual void OnAttach() = 0;
        virtual void OnDettach() = 0;
        virtual void OnDestroy() = 0;
        virtual void OnUpdate() = 0;
        virtual void OnMouseClickEvent() = 0;
        virtual void OnMouseMoveEvent() = 0;
        virtual void OnKeyboardEvent() = 0;
        virtual std::string_view GetName();
        virtual std::string_view GetName() const;

        bool ShouldExit();
        void SetShouldExit(bool value);

    private:
        std::string_view m_Name;
        bool m_ShouldExit;
    };
}// namespace Engine