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

#include <thread>

#include "Build/Namespace.hpp"
#include "Types/FundamentalTypes.hpp"

BEGIN_RUKEN_NAMESPACE

class Worker
{
    private:

        #pragma region Variables

        std::thread m_thread {};

        #pragma endregion

    public:

        #pragma region Constructors

        Worker()                      = default;
        Worker(Worker const& in_copy) = delete;
        Worker(Worker&&      in_move) = default;
        ~Worker() noexcept;

        #pragma endregion

        #pragma region Methods

        /**
         * \brief Checks if the worker is busy or not 
         * \return True if the worker is busy
         */
        [[nodiscard]]
        RkBool Available() const noexcept;

        /**
         * \brief Waits for the last task to execute, and starts the execution of this new job 
         * \tparam TExecutable Type of the job
         * \param in_job job to execute when done with the previous one
         * \param in_args Args of the job to execute
         */
        template <typename TExecutable, typename ...TArgs>
        RkVoid Execute(TExecutable in_job, TArgs... in_args) noexcept;

        /**
         * \brief Waits for the last task to execute, and starts the execution of this new job 
         * \tparam TExecutable Type of the job
         * \param in_job job to execute when done with the previous one
         * \param in_args Args of the job to execute
         */
        template <typename TExecutable, typename ...TArgs>
        RkVoid ExecuteWithInstance(TExecutable in_job, TArgs... in_args) noexcept;

        /**
         * \brief Locks the current thread until the current job has been done.
         *        If there was no job currently executed, this method has no effect.
         */
        RkVoid WaitForAvailability() noexcept;

        /**
         * \brief Separates the thread of execution from the worker object, allowing execution to continue independently.
         * 
         * \note Any allocated resources will be freed once the thread exits.
         * \note If the worker was available, this method has no effect
         */
        RkVoid Detach() noexcept;

        /**
         * \brief Returns the current underlying thread
         * \return Worker's thread
         */
        [[nodiscard]]
        std::thread& Thread() noexcept;

        [[nodiscard]]
        std::thread::id ID() const noexcept;

        #pragma endregion

        #pragma region Operators

        Worker& operator=(Worker const& in_copy) = delete;
        Worker& operator=(Worker&&      in_move) = default;

        #pragma endregion

};

#include "Threading/Worker.inl"

END_RUKEN_NAMESPACE
