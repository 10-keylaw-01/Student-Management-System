"""Backend communicator for C++ subprocess interaction."""

import subprocess
import threading
import queue
from typing import Optional


class BackendCommunicator:
    """Handles communication with C++ backend via subprocess."""
    
    def __init__(self, executable_path: str):
        self.executable_path = executable_path
        self.process: Optional[subprocess.Popen] = None
        self.output_queue = queue.Queue()
        self.reader_thread: Optional[threading.Thread] = None
        self.is_running = False
        self._lock = threading.Lock()
    
    def start(self):
        """Start the backend process."""
        if self.is_running:
            return
        
        try:
            self.process = subprocess.Popen(
                [self.executable_path],
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                encoding='utf-8',
                errors='replace',
                bufsize=1,
            )
            self.is_running = True
            self.reader_thread = threading.Thread(target=self._read_output, daemon=True)
            self.reader_thread.start()
        except FileNotFoundError:
            raise Exception(f"Executable not found: {self.executable_path}")
        except Exception as e:
            raise Exception(f"Failed to start backend: {str(e)}")
    
    def _read_output(self):
        """Read output from backend in separate thread."""
        if not self.process:
            return
        
        try:
            for line in iter(self.process.stdout.readline, ''):
                if line:
                    self.output_queue.put(('output', line.rstrip('\n')))
                if not self.is_running:
                    break
        except Exception as e:
            self.output_queue.put(('error', str(e)))
        finally:
            self.is_running = False
    
    def send_command(self, command: str) -> str:
        """Send command and get response."""
        if not self.is_running or not self.process:
            raise Exception("Backend not running")
        
        try:
            self.process.stdin.write(command + '\n')
            self.process.stdin.flush()
        except Exception as e:
            self.is_running = False
            raise Exception(f"Failed to send command: {str(e)}")
    
    def get_output(self, timeout: float = 0.1) -> Optional[tuple]:
        """Get next output from queue."""
        try:
            return self.output_queue.get(timeout=timeout)
        except queue.Empty:
            return None
    
    def stop(self):
        """Stop the backend process."""
        self.is_running = False
        if self.process:
            try:
                self.process.stdin.close()
                self.process.terminate()
                self.process.wait(timeout=2)
            except Exception:
                self.process.kill()
            finally:
                self.process = None
    
    def __del__(self):
        self.stop()
