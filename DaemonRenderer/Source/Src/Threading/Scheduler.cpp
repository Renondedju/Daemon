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

#include "Threading/Scheduler.hpp"

#include "Threading/WorkerGroups/BatchedWorkerGroup.hpp"
#include "Threading/WorkerGroups/RelaxedWorkerGroup.hpp"

USING_DAEMON_NAMESPACE

Scheduler::Scheduler():
    m_groups {}
{
    // Decreasing the concurrency by 1 since the main thread is already spawned
    DAEuint16 const concurrency = static_cast<DAEuint16 const>(std::thread::hardware_concurrency() - 1);

    DAEuint16 const io_threads  = concurrency / 4;
    DAEuint16 const ecs_threads = concurrency - io_threads;

    m_groups.try_emplace<WorkerGroup*>(EWorkerGroupID::IO , new RelaxedWorkerGroup(EWorkerGroupID::IO , io_threads ));
    m_groups.try_emplace<WorkerGroup*>(EWorkerGroupID::Ecs, new BatchedWorkerGroup(EWorkerGroupID::Ecs, ecs_threads));
}

Scheduler::~Scheduler()
{
    // Releasing groups
    for (auto [id, group]: m_groups)
        delete group;
}

DAEvoid Scheduler::EnqueueTask(Task&& in_task, EWorkerGroupID const in_worker_group) noexcept
{
    //m_groups.at(in_worker_group)->EnqueueTask(std::forward<Task>(in_task));
}

DAEsize Scheduler::GetWorkerGroupSize(EWorkerGroupID const in_worker_group) const noexcept
{
    return m_groups.at(in_worker_group)->GetGroupSize();
}