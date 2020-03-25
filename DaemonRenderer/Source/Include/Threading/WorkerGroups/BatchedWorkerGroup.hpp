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
#include "Threading/WorkerGroups/WorkerGroup.hpp"

BEGIN_DAEMON_NAMESPACE

/**
 * \brief As the name indicates, a batched worker group handles tasks by batch.
 *        This specially useful when a certain batch of tasks needs to be executed again and again
 *        without loosing time by having to re upload the same tasks in between each execution.
 *        This comes with the downside of being harder to manipulate and overall less flexible
 */
class BatchedWorkerGroup final : public WorkerGroup
{
    private:

        #pragma region Members

        std::atomic_size_t m_cursor;
        std::atomic_bool   m_living;
        std::vector<Task>  m_tasks;

        #pragma endregion

        #pragma region Methods

        /**
         * \brief Main routine of every worker in the group
         */
        DAEvoid WorkerRoutine(Worker& in_worker) noexcept;

        #pragma endregion

    public:

        #pragma region Constructors

        /**
         * \brief Initializes a working group
         * \param in_group_id Unique ID of the group
         * \param in_workers_count Number of workers in this group
         */
        BatchedWorkerGroup(EWorkerGroupID in_group_id, DAEuint16 in_workers_count) noexcept;

        BatchedWorkerGroup(BatchedWorkerGroup const& in_copy) = delete;
        BatchedWorkerGroup(BatchedWorkerGroup&&      in_move) = delete;
        ~BatchedWorkerGroup();

        #pragma endregion

        #pragma region Methods

        /**
         * \brief Sets the tasks batch to be processed
         * \param in_new_tasks New batch to process
         * \warning The workers should be done with any previous batch before calling this method
         * \todo This method requires some serious optimizations !
         */
        DAEvoid SetBatch(std::vector<Task>& in_new_tasks) noexcept;

        /**
         * \brief Wakes up any worker that has been put to sleep and
         *        resets the reading head, essentially executing the previously
         *        set batch of tasks.
         */
        DAEvoid ConsumeBatch() noexcept;

        #pragma endregion

        #pragma region Operators

        BatchedWorkerGroup& operator=(BatchedWorkerGroup const& in_copy) noexcept = delete;
        BatchedWorkerGroup& operator=(BatchedWorkerGroup&&      in_move) noexcept = delete;

        #pragma endregion
};

END_DAEMON_NAMESPACE