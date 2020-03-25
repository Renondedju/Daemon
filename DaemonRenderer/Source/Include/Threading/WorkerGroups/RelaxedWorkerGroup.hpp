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
 *  SOFTWARE.
 */

#pragma once

#include "Config.hpp"

#include "Threading/Task.hpp"
#include "Threading/ThreadSafeLockQueue.hpp"
#include "Threading/WorkerGroups/WorkerGroup.hpp"

BEGIN_DAEMON_NAMESPACE

/**
 * This kind of worker group can handle so called "unexpected" jobs
 *
 * The queue used internally is unlimited and can be inspected using events,
 * allowing to save a lot of CPU by putting to sleep unused workers.
 *
 * As a trade off, this kind of group can be relatively slow at handling jobs, making it
 * a perfect candidate for non critical tasks such as resource handling or I/O transactions
 */
class RelaxedWorkerGroup final : public WorkerGroup
{
    private:

        #pragma region Members

        ThreadSafeLockQueue<Task> m_tasks;

        #pragma endregion

        #pragma region Methods

        /**
         * \brief Main routine of every worker in the group
         */
        DAEvoid WorkerRoutine() noexcept;

        #pragma endregion

    public:

        #pragma region Constructors

        /**
         * \brief Initializes a working group
         * \param in_group_id Unique ID of the group
         * \param in_workers_count Number of workers in this group
         */
        RelaxedWorkerGroup(EWorkerGroupID in_group_id, DAEuint16 in_workers_count) noexcept;

        RelaxedWorkerGroup(RelaxedWorkerGroup const& in_copy) = delete;
        RelaxedWorkerGroup(RelaxedWorkerGroup&&      in_move) = delete;
        ~RelaxedWorkerGroup();

        #pragma endregion

        #pragma region Methods

        /**
         * \brief Enqueue a task to be handled by the working group
         * \param in_task Task to handle on another thread
         */
        DAEvoid EnqueueTask(Task&& in_task) noexcept;

        #pragma endregion

        #pragma region Operators

        RelaxedWorkerGroup& operator=(RelaxedWorkerGroup const& in_copy) noexcept = delete;
        RelaxedWorkerGroup& operator=(RelaxedWorkerGroup&&      in_move) noexcept = delete;

        #pragma endregion
};

END_DAEMON_NAMESPACE