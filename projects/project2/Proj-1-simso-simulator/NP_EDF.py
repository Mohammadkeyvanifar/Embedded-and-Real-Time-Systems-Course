from simso.core import Scheduler


class NP_EDF(Scheduler):
    def init(self):
        self.ready_list = []

    def on_activate(self, job):
        self.ready_list.append(job)

        # Only reschedule if CPU is idle.
        if job.cpu.running is None:
            job.cpu.resched()

    def on_terminated(self, job):
        self.ready_list.remove(job)

        # Now the CPU is free, choose next job.
        job.cpu.resched()

    def schedule(self, cpu):
        # Non-preemptive behavior:
        # if something is already running, keep it.
        if cpu.running and cpu.running.is_active():
            return (cpu.running, cpu)

        # CPU is idle -> choose earliest deadline.
        if self.ready_list:
            job = min(self.ready_list, key=lambda j: j.absolute_deadline)
        else:
            job = None

        return (job, cpu)
