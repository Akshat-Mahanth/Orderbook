import subprocess, os, signal

class EngineLauncher:
    def __init__(self, cmd=["./sim"]):
        self.cmd = cmd
        self.proc = None

    def start(self):
        if self.proc is None:
            self.proc = subprocess.Popen(
                self.cmd,
                stdout=subprocess.DEVNULL,
                stderr=subprocess.DEVNULL
            )

    def stop(self):
        if self.proc:
            self.proc.send_signal(signal.SIGINT)
            self.proc.wait(timeout=2)
            self.proc = None

