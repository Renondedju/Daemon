/*
 *  MIT License
 *
 *  Copyright (c) 2019 Basile Combet, Philippe Yi
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
 */

#include "Threading/WorkerGroups/RelaxedWorkerGroup.hpp"
#include "Threading/SynchronizedAccess.hpp"

USING_DAEMON_NAMESPACE

DAEvoid RelaxedWorkerGroup::WorkerRoutine() noexcept
{
    Task task = nullptr;

    // While we get a valid task to handle
    // This function will return false when the worker group will
    // get dismantled, meaning that the worker needs to be released
    //
    // Also since we are using a locking queue, the worker
    // will be put to rest if nothing is available.
    while (m_tasks.Dequeue(task))
        task();
}

RelaxedWorkerGroup::RelaxedWorkerGroup(EWorkerGroupID const in_group_id, DAEuint16 const in_workers_count) noexcept:
    WorkerGroup {in_group_id, in_workers_count},
    m_tasks     {}
{
    // Giving workers a job
    for(DAEuint16 index = 0; index < in_workers_count; ++index)
        m_workers[index].Execute(&RelaxedWorkerGroup::WorkerRoutine, this);
}

RelaxedWorkerGroup::~RelaxedWorkerGroup()
{
    // Waiting for the queue to be empty
    m_tasks.WaitUntilEmpty();

    // Detaching every worker.
    // This way any ongoing job doesn't require the group to stay alive and we can
    // unlock the main thread to do other things instead of waiting for us
    for (auto& worker: m_workers)
        worker.Detach();

    // Calling this will cause the release of the task queue
    // and thus the exit of every worker that will be done in the background
    // because of the previous Detach call
    m_tasks.Release();
}

DAEvoid RelaxedWorkerGroup::EnqueueTask(Task&& in_task) noexcept
{
    m_tasks.Enqueue(std::forward<Task>(in_task));
}
