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

#include "ECS/EntityAdmin.hpp"
#include "Core/ServiceProvider.hpp"

#include "ECS/System.hpp"

USING_RUKEN_NAMESPACE

EntityAdmin::EntityAdmin(ServiceProvider& in_service_provider) noexcept:
    Service     {in_service_provider},
    m_scheduler {m_service_provider.LocateService<Scheduler>()}
{ }

RkVoid EntityAdmin::BuildEventExecutionPlan(EEventName const in_event_name) noexcept
{
    std::unique_ptr<ExecutionPlan>& execution_plan = m_execution_plans[in_event_name];

    if (execution_plan == nullptr)
        execution_plan = std::make_unique<ExecutionPlan>();

    execution_plan->ResetPlan();

    // FIXME: This current implementation does not take care of the potential optimizations that could be made
    // by taking into account component read and writes
    for (auto& system: m_systems)
    {
        EventHandlerBase* event_handler = system->GetEventHandler(in_event_name);
        if (event_handler == nullptr)
            continue;

        execution_plan->AddInstruction([&event_handler] { event_handler->Execute(); });
        execution_plan->EndInstructionPack();
    }
}

RkVoid EntityAdmin::ExecuteEvent(EEventName const in_event_name) noexcept
{
    if (m_execution_plans[in_event_name] == nullptr)
        BuildEventExecutionPlan(in_event_name);

    std::unique_ptr<ExecutionPlan>& execution_plan = m_execution_plans.at(in_event_name);

    // If multithreading is available
    if (m_scheduler)
        execution_plan->ExecutePlanAsynchronously(*m_scheduler);
    else
        execution_plan->ExecutePlanSynchronously();
}
