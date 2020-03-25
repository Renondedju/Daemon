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

#include "Threading/WorkerGroups/BatchedWorkerGroup.hpp"

USING_DAEMON_NAMESPACE

DAEvoid BatchedWorkerGroup::WorkerRoutine(Worker& in_worker) noexcept
{
    // Workers are put to sleep as soon as they are created to give
    // the main thread the time to setup the task queue
    //
    // Note that in this state the worker "deadlocked" himself meaning
    // that the only way to restore the execution of the thread is to do it from
    // another thread, in this case, the main thread
    in_worker.lock.test_and_set(std::memory_order_acquire);
    while(in_worker.lock.test_and_set(std::memory_order_acquire))
    {}

    // While we are asked to live (ie. work)
    while (m_living.load(std::memory_order_acquire))
    {
        // Fetching the task
        Task task = m_tasks[m_cursor.fetch_add(1)];

        // Checking if we popped a valid task
        if (task != nullptr)
            task();

        // Otherwise, the thread is put to sleep while waiting for the next batch to come in
        else
        {
            in_worker.lock.test_and_set(std::memory_order_acquire);
            while(in_worker.lock.test_and_set(std::memory_order_acquire))
            {}
        }
    }
}

BatchedWorkerGroup::BatchedWorkerGroup(EWorkerGroupID const in_group_id, DAEuint16 const in_workers_count) noexcept:
    WorkerGroup {in_group_id, in_workers_count},
    m_cursor    {0},
    m_living    {true},
    m_tasks     {}
{
    // Giving workers a job
    for(Worker& worker : m_workers)
        worker.ExecuteWithInstance(&BatchedWorkerGroup::WorkerRoutine, this);
}

BatchedWorkerGroup::~BatchedWorkerGroup()
{
    // Contrarily to the relaxed group, jobs are not guaranteed to be handled once the group is planning
    // to dismantle itself, thus we only ask workers to finish what they are doing and then kill themselves

    // Notifying workers that they don't have anything left to do
    m_living.store(false, std::memory_order_release);

    // Waking up workers
    for (Worker& worker : m_workers)
        worker.lock.clear(std::memory_order_release);

    // Waiting for workers to kill themselves
    // Since we know they are currently exiting their main loop, waiting for them to do so before destroying
    // any data is actually safer
    for (auto& worker: m_workers)
        worker.WaitForAvailability();
}

DAEvoid BatchedWorkerGroup::SetBatch(std::vector<Task>& in_new_tasks) noexcept
{
    // Coping jobs
    m_tasks = in_new_tasks;

    // Adding end markers
    for (DAEsize index = 0; index < m_workers.size(); ++index)
        m_tasks.emplace_back(Task(nullptr));
}

DAEvoid BatchedWorkerGroup::ConsumeBatch() noexcept
{
    // Resetting the cursor to 0
    m_cursor.store(0, std::memory_order_release);

    // Waking up workers
    for (Worker& worker : m_workers)
        worker.lock.clear(std::memory_order_release);
}
