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

#include <queue>
#include <atomic>
#include <iostream>

#include "Config.hpp"
#include "Utility/Benchmark.hpp"
#include "Threading/Scheduler.hpp"
#include "Threading/WorkerGroups/BatchedWorkerGroup.hpp"
#include "Threading/WorkerGroups/RelaxedWorkerGroup.hpp"

USING_DAEMON_NAMESPACE

void TestTask()
{
    for (volatile int i = 0; i < 1000000; ++i)
    {}
}

void TestBigTask()
{
    for (volatile int i = 0; i < 10000000; ++i)
    {}
}

/**
 * This kind of worker group can handle so called "unexpected" jobs
 *
 * The queue used internally is unlimited and can be inspected using events,
 * allowing to save a lot of CPU by putting to sleep unused workers.
 *
 * As a trade off, this kind of group can be relatively slow at handling jobs, making it
 * a perfect candidate for non critical tasks such as resource handling or I/O transactions
 */
void TestRelaxed(DAEsize const in_tasks_count, DAEuint16 const in_workers_count, DAEsize const in_sample_size)
{
    RelaxedWorkerGroup group(EWorkerGroupID::IO, in_workers_count);
    std::atomic_flag   wait = ATOMIC_FLAG_INIT;

    LOOPED_BENCHMARK("Execute tasks", in_sample_size)
    {
        // Adding dummy tasks
        for (DAEsize index = 0; index < in_tasks_count / 2; ++index)
            group.EnqueueTask(&TestTask);

        group.EnqueueTask(&TestBigTask);

        for (DAEsize index = 0; index < in_tasks_count / 2; ++index)
            group.EnqueueTask(&TestTask);

        // Adding the "notification" task that
        // will unlock this thread once completed
        group.EnqueueTask([&wait]() {
            wait.clear(std::memory_order_release);
        });

        // Locking this thread until completion of the batch
        // (ie. the last task has been handled)
        wait.test_and_set(std::memory_order_acquire);
        while (wait.test_and_set(std::memory_order_acquire))
        {}
    }
}

/**
 * As the name indicates, a batched worker group handles tasks by batch.
 * This specially useful when a certain batch of tasks needs to be executed again and again
 * without loosing time by having to re upload the same tasks in between each execution.
 * This comes with the downside of being harder to manipulate and overall less flexible
 */
void TestBatched(DAEsize const in_tasks_count, DAEuint16 const in_workers_count, DAEsize const in_sample_size)
{
    BatchedWorkerGroup group(EWorkerGroupID::Ecs, in_workers_count);
    std::atomic_flag   wait = ATOMIC_FLAG_INIT;

    BENCHMARK("Writing tasks")
    {
        std::vector<Task> tasks (in_tasks_count + 2);

        // Adding dummy tasks
        for (DAEsize index = 0; index < in_tasks_count + 1; ++index)
            tasks[index] = &TestTask;

        tasks[in_tasks_count / 2] = &TestBigTask;

        // Adding the "notification" task that
        // will unlock this thread once completed
        tasks[in_tasks_count] = [&wait]() {
            wait.clear(std::memory_order_release);
        };

        group.SetBatch(tasks);
    }

    LOOPED_BENCHMARK("Execute tasks", in_sample_size)
    {
        group.ConsumeBatch();

        // Locking this thread until completion of the batch
        // (ie. the last task has been handled)
        wait.test_and_set(std::memory_order_acquire);
        while (wait.test_and_set(std::memory_order_acquire))
        {}
    }
}

int main()
{
    // The sample size represents the number of frames simulated
    // in order to output a benchmark, the bigger it is, the greater the accuracy of the benchmark
    // but the slower it will be to compute
    DAEsize   const sample_size   = 500;
    DAEsize   const tasks_count   = 50;
    DAEuint16 const workers_count = 7;

    std::cout << "Executing " << tasks_count << " tasks with " << workers_count << " workers.\n";

    // This benchmark will measure how much these dummy tasks would cost us
    // if they where to be executed on a single thread
    BENCHMARK("Linear time")
    {
        TestBigTask();

        for (DAEsize index = 0; index < tasks_count; ++index)
            TestTask();
    }

    std::cout << "\n---- Relaxed test" << std::endl;
    TestRelaxed(tasks_count, workers_count, sample_size);

    std::cout << "\n---- Batched test" << std::endl;
    TestBatched(tasks_count, workers_count, sample_size);

    system("pause");

    return EXIT_SUCCESS;
}