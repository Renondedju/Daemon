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
 */

#pragma once

#include "Config.hpp"
#include "Containers/Vector.hpp"
#include "Types/FundamentalTypes.hpp"

#include "Threading/Task.hpp"
#include "Threading/Worker.hpp"
#include "Threading/WorkerGroups/EWorkerGroupID.hpp"

BEGIN_DAEMON_NAMESPACE

/**
 * \brief A working group is a group of uniquely identifiable workers
 *        designed to handle specific types of tasks
 */
class WorkerGroup
{
    protected:

        #pragma region Members

        EWorkerGroupID m_group_id;
        Vector<Worker> m_workers;

        #pragma endregion

    public:

        #pragma region Constructors

        /**
         * \brief Initializes a working group
         *        Workers are in a lethargic state meaning that memory has been
         *        allocated but the workers still needs to be initialized with jobs.
         * \param in_group_id Unique ID of the group
         * \param in_workers_count Number of workers in this group
         */
        WorkerGroup(EWorkerGroupID in_group_id, DAEuint16 in_workers_count) noexcept;

        WorkerGroup(WorkerGroup const& in_copy) = default;
        WorkerGroup(WorkerGroup&&      in_move) = default;
        virtual ~WorkerGroup()                  = default;

        #pragma endregion

        #pragma region Methods

        /**
         * \brief Returns the size of the working group
         * \return Size of the working group
         */
        [[nodiscard]]
        DAEuint16 GetGroupSize() const noexcept;

        /**
         * \brief Returns the unique identifier of the group
         * \return Unique EWorkerGroupID of the group
         */
        [[nodiscard]]
        EWorkerGroupID GetID() const noexcept;

        #pragma endregion

        #pragma region Operators

        WorkerGroup& operator=(WorkerGroup const& in_copy) noexcept = delete;
        WorkerGroup& operator=(WorkerGroup&&      in_move) noexcept = delete;

        #pragma endregion

};

END_DAEMON_NAMESPACE