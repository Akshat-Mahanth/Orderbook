# viz/sim_controller.py

import subprocess
import os
import signal
import time


class SimController:
    def __init__(self, sim_path="./sim"):
        self.sim_path = sim_path
        self.proc = None

    # -----------------------------------------
    def start(self) -> bool:
        # already running
        if self.proc is not None and self.proc.poll() is None:
            print("[SIM] already running")
            return False

        try:
            self.proc = subprocess.Popen(
                [self.sim_path],
                stdout=subprocess.DEVNULL,
                stderr=subprocess.DEVNULL,
                preexec_fn=os.setsid  # create new process group
            )
            print("[SIM] started, pid =", self.proc.pid)
            return True
        except Exception as e:
            print("[SIM] start failed:", e)
            self.proc = None
            return False

    # -----------------------------------------
    def stop(self) -> bool:
        if self.proc is None:
            print("[SIM] stop called but no process")
            return False

        try:
            pgid = os.getpgid(self.proc.pid)
            print("[SIM] sending SIGINT to pgid", pgid)
            os.killpg(pgid, signal.SIGINT)

            # give it time to exit cleanly
            self.proc.wait(timeout=2)

        except Exception as e:
            print("[SIM] SIGINT failed, forcing kill:", e)
            try:
                os.killpg(os.getpgid(self.proc.pid), signal.SIGKILL)
            except Exception:
                pass

        finally:
            self.proc = None
            print("[SIM] stopped")
            return True

    # -----------------------------------------
    def is_running(self):
        return self.proc is not None

