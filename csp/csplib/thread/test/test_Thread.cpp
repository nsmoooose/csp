/* Combat Simulator Project
 * Copyright (C) 2025 Henrik Nilsson <nsmoooose@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */


/**
 * @file test_Thread.cpp
 * @brief Test for csplib/thread/Thread.h.
 */

#include <csp/csplib/thread/Thread.h>
#include <csp/csplib/util/Testing.h>


class MyTask : public csp::Task {
public:
	bool m_task_has_been_executed;

	MyTask() : m_task_has_been_executed(false) {}

	virtual void run() {
		m_task_has_been_executed = true;
	}
};

CSP_TESTFIXTURE(Thread) {

	CSP_TESTCASE(TestThread) {
		csp::Ref<MyTask> task = new MyTask();
		CSP_EXPECT(task->m_task_has_been_executed == false);

		csp::Thread thread;
		thread.start(task.get());
		thread.join(5000);
		CSP_EXPECT(task->m_task_has_been_executed == true);
	}
};
