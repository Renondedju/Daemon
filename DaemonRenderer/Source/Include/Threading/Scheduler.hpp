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

#include <unordered_map>

#include "Config.hpp"
#include "Types/FundamentalTypes.hpp"
#include "Threading/WorkerGroups/WorkerGroup.hpp"
#include "Threading/WorkerGroups/EWorkerGroupID.hpp"

BEGIN_DAEMON_NAMESPACE

/**
 * \brief This class is responsible for the
 *        repartition of different tasks between workers
 */
class Scheduler
{
    private:

        #pragma region Members

        std::unordered_map<EWorkerGroupID, WorkerGroup*> m_groups;

        #pragma endregion

    public:

        #pragma region Constructors

        Scheduler();
        Scheduler(Scheduler const& in_copy) = delete;
        Scheduler(Scheduler&&      in_move) = delete;
        ~Scheduler();

        #pragma endregion

        #pragma region Methods

        /**
         * \brief Schedules a task to be handled by a given working group
         * \param in_task Task to schedule, any return value will be discarded
         * \param in_worker_group Defines the group of workers that will be able to handle the job
         */
        DAEvoid EnqueueTask(Task&& in_task, EWorkerGroupID in_worker_group) noexcept;

        /**
         * \brief Returns the size of a worker group
         * \param in_worker_group Worker group to measure
         * \return Size of the passed worker group
         */
        [[nodiscard]]
        DAEsize GetWorkerGroupSize(EWorkerGroupID in_worker_group) const noexcept;

        #pragma endregion 

        #pragma region Operators

        Scheduler& operator=(Scheduler const& in_copy) = delete;
        Scheduler& operator=(Scheduler&&      in_move) = delete;

        #pragma endregion
};

END_DAEMON_NAMESPACE