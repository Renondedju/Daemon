/*
 *  MIT License
 *
 *  Copyright (c) 2019-2020 Basile Combet, Philippe Yi
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NON INFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#pragma once

#include <memory>
#include <unordered_map>

#include "Build/Namespace.hpp"

#include "ECS/EEventName.hpp"
#include "Types/FundamentalTypes.hpp"
#include "ECS/Safety/EventHandlerType.hpp"

BEGIN_RUKEN_NAMESPACE

class EntityAdmin;
class EventHandlerBase;

/**
 * \brief Systems transform data. They implement the logic that modifies the components.
 * \note Systems are a way to logically order event handlers, to treat a particular type of data
 * \see System::EventHandler
 *
 * It is important to notice that for lots of actions, you don’t care about the specific type of an entity;
 * what you care about is specific properties of these entities.
 * E.g. for rendering all you need is a mesh and a transform matrix; you don’t care if the entity is a player or a tree.
 */
class System
{
    public:

        // Helpers, allows for shorter declarations later on
        template <ComponentType... TComponents> using UpdateEventHandler = EventHandler<EEventName::OnUpdate, TComponents...>;
        template <ComponentType... TComponents> using StartEventHandler  = EventHandler<EEventName::OnStart , TComponents...>;
        template <ComponentType... TComponents> using EndEventHandler    = EventHandler<EEventName::OnEnd   , TComponents...>;

    private:

        #pragma region Members

        // Event handlers, used for the update of the system
        std::unordered_map<EEventName, std::unique_ptr<EventHandlerBase>> m_handlers {};

        #pragma endregion

    protected:

        #pragma region Members

        EntityAdmin& m_admin;

        #pragma endregion

        #pragma region Methods

        /**
         * \brief Setups an event handler in the system.
         *        Note that only one event handler for each event type is currently handled,
         *        setting multiple ones will simply override the previous one
         * \tparam TEventHandler Event handler type, must inherit from System::EventHandler
         */
        template <EventHandlerType TEventHandler>
        RkVoid SetupEventHandler() noexcept;

        #pragma endregion

    public:

        #pragma region Constructors

        System(EntityAdmin&  in_admin) noexcept;
        System(System const& in_copy) = default;
        System(System&&      in_move) = default;
        virtual ~System()             = default;

        #pragma endregion

        #pragma region Methods

        /**
         * \brief Returns the setup event handler for the passed event (if any) 
         * \param in_event_name Event name
         * \return Event handler instance or nullptr
         */
        EventHandlerBase* GetEventHandler(EEventName in_event_name) const noexcept;

        #pragma endregion

        #pragma region Operators

        System& operator=(System const& in_copy) = delete;
        System& operator=(System&&      in_move) = delete;

        #pragma endregion
};

#include "ECS/System.inl"

END_RUKEN_NAMESPACE