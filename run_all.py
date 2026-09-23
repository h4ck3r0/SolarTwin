#!/usr/bin/env python3
"""
SolarTwin — Unified Launcher Script
====================================
Starts all backend and frontend services simultaneously:
  1. [EMT-8001]  Pandapower EMT Solver API   (http://127.0.0.1:8001)
  2. [LSTM-8000] LSTM Live Prediction Server (http://localhost:8000)
  3. [WEB-3000]  Next.js Frontend Platform   (http://localhost:3000)

Features:
  - Cross-platform (Windows / Linux / macOS)
  - Colorized prefix logs for each service
  - Clean process tree termination on Ctrl+C (no orphan Node/Python background processes)
  - Port conflict detection
  - Configurable site coordinates via CLI or environment variables
  - Optional auto-open browser flag (--open)

Usage:
  python run_all.py
  python run_all.py --open
  python run_all.py --help
"""

import argparse
import os
import platform
import shutil
import signal
import socket
import subprocess
import sys
import threading
import time
import webbrowser
from pathlib import Path

# ── ANSI Styling ─────────────────────────────────────────────────────────────
if sys.platform == "win32":
    # Enable ANSI escape sequences on Windows console
    import ctypes
    kernel32 = ctypes.windll.kernel32
    kernel32.SetConsoleMode(kernel32.GetStdHandle(-11), 7)

RESET  = "\033[0m"
BOLD   = "\033[1m"
DIM    = "\033[2m"
CYAN   = "\033[36m"
GREEN  = "\033[32m"
YELLOW = "\033[33m"
BLUE   = "\033[34m"
MAGENTA= "\033[35m"
RED    = "\033[31m"

TAG_SYS   = f"{BOLD}{MAGENTA}[SYSTEM]      {RESET} "
TAG_EMT   = f"{BOLD}{CYAN}[EMT-8001]    {RESET} "
TAG_LSTM  = f"{BOLD}{GREEN}[LSTM-8000]   {RESET} "
TAG_FRONT = f"{BOLD}{BLUE}[FRONTEND-3000]{RESET} "

# ── Paths ────────────────────────────────────────────────────────────────────
REPO_ROOT = Path(__file__).resolve().parent
PYTHON_BACKEND_DIR = REPO_ROOT / "python_backend"
FRONTEND_DIR = REPO_ROOT / "upqc-simulation-platform"


def is_port_in_use(port: int, host: str = "127.0.0.1") -> bool:
    """Check if a TCP port is currently open and accepting connections."""
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.settimeout(0.6)
        return s.connect_ex((host, port)) == 0


def kill_process_tree(pid: int):
    """Recursively kill a process and all of its spawned child processes."""
    if sys.platform == "win32":
        try:
            subprocess.run(
                ["taskkill", "/F", "/T", "/PID", str(pid)],
                stdout=subprocess.DEVNULL,
                stderr=subprocess.DEVNULL,
            )
        except Exception:
            pass
    else:
        try:
            os.killpg(os.getpgid(pid), signal.SIGTERM)
        except Exception:
            try:
                os.kill(pid, signal.SIGTERM)
            except Exception:
                pass


def stream_output(pipe, prefix: str):
    """Read lines from a subprocess stdout/stderr and print with colored tag."""
    try:
        for line in iter(pipe.readline, ""):
            if not line:
                break
            stripped = line.rstrip("\r\n")
            if stripped:
                print(f"{prefix}{stripped}", flush=True)
    except Exception:
        pass
    finally:
        pipe.close()


def find_npm_command():
    """Find npm executable properly on Windows and POSIX."""
    if sys.platform == "win32":
        npm_cmd = shutil.which("npm.cmd") or shutil.which("npm")
    else:
        npm_cmd = shutil.which("npm")
    return npm_cmd


def wait_for_port(port: int, timeout: float = 15.0) -> bool:
    """Wait until a local port is up, or until timeout."""
    start_time = time.time()
    while time.time() - start_time < timeout:
        if is_port_in_use(port):
            return True
        time.sleep(0.4)
    return False


def main():
    parser = argparse.ArgumentParser(
        description="SolarTwin Unified Launcher — Starts Backend (8000, 8001) & Frontend (3000)"
    )
    parser.add_argument(
        "--open", "-o", action="store_true", help="Automatically open browser to http://localhost:3000"
    )
    parser.add_argument(
        "--site-lat",
        default=os.getenv("SITE_LAT", "12.9007"),
        help="Installation site latitude (default: 12.9007 for RNSIT, Bangalore)",
    )
    parser.add_argument(
        "--site-lon",
        default=os.getenv("SITE_LON", "77.5174"),
        help="Installation site longitude (default: 77.5174 for RNSIT, Bangalore)",
    )
    parser.add_argument(
        "--no-frontend", action="store_true", help="Start only the Python backends (skip Next.js)"
    )
    parser.add_argument(
        "--no-backend", action="store_true", help="Start only the Next.js frontend (skip Python)"
    )
    args = parser.parse_args()

    # Banner
    print()
    print(f"{BOLD}{CYAN}==============================================================={RESET}")
    print(f"{BOLD}{GREEN}           ☀️  SolarTwin Digital Twin Platform Launcher           {RESET}")
    print(f"{BOLD}{CYAN}==============================================================={RESET}")
    print(f"{TAG_SYS}Python Executable : {sys.executable}")
    print(f"{TAG_SYS}Repository Root   : {REPO_ROOT}")
    print(f"{TAG_SYS}Site Coordinates  : lat={args.site_lat}, lon={args.site_lon}")
    print(f"{BOLD}{CYAN}---------------------------------------------------------------{RESET}")

    # Check for port collisions
    ports_to_check = []
    if not args.no_backend:
        ports_to_check.extend([(8001, "EMT Solver"), (8000, "LSTM Live Server")])
    if not args.no_frontend:
        ports_to_check.append((3000, "Next.js Frontend"))

    in_use_warnings = []
    for port, name in ports_to_check:
        if is_port_in_use(port):
            in_use_warnings.append(f"Port {port} ({name}) is already in use!")

    if in_use_warnings:
        print(f"{TAG_SYS}{YELLOW}⚠️  Warning: Port collision(s) detected:{RESET}")
        for w in in_use_warnings:
            print(f"{TAG_SYS}{YELLOW}    - {w}{RESET}")
        print(f"{TAG_SYS}{YELLOW}If you have an old session running, terminate it first.{RESET}\n")

    # Environment setup
    env = os.environ.copy()
    env["PYTHONUNBUFFERED"] = "1"
    env["SITE_LAT"] = str(args.site_lat)
    env["SITE_LON"] = str(args.site_lon)
    # Add python_backend to PYTHONPATH so modules import seamlessly
    existing_pythonpath = env.get("PYTHONPATH", "")
    env["PYTHONPATH"] = f"{PYTHON_BACKEND_DIR}{os.pathsep}{REPO_ROOT}{os.pathsep}{existing_pythonpath}"

    procs = []

    try:
        # 1. Start Python EMT Solver Backend (8001)
        if not args.no_backend:
            print(f"{TAG_SYS}Starting EMT Solver API on port 8001...")
            proc_emt = subprocess.Popen(
                [sys.executable, "main.py"],
                cwd=str(PYTHON_BACKEND_DIR),
                env=env,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True,
                bufsize=1,
            )
            procs.append(("EMT Solver (8001)", proc_emt))
            t_emt = threading.Thread(target=stream_output, args=(proc_emt.stdout, TAG_EMT), daemon=True)
            t_emt.start()

            # 2. Start LSTM Live Prediction Server (8000)
            print(f"{TAG_SYS}Starting LSTM Live Prediction Server on port 8000...")
            proc_lstm = subprocess.Popen(
                [sys.executable, "live_server.py"],
                cwd=str(PYTHON_BACKEND_DIR),
                env=env,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True,
                bufsize=1,
            )
            procs.append(("LSTM Server (8000)", proc_lstm))
            t_lstm = threading.Thread(target=stream_output, args=(proc_lstm.stdout, TAG_LSTM), daemon=True)
            t_lstm.start()

        # 3. Start Next.js Frontend (3000)
        if not args.no_frontend:
            npm_cmd = find_npm_command()
            if not npm_cmd:
                print(f"{TAG_SYS}{RED}❌ Error: 'npm' command not found! Please ensure Node.js is installed.{RESET}")
            else:
                print(f"{TAG_SYS}Starting Next.js Frontend on port 3000 using {npm_cmd}...")
                proc_frontend = subprocess.Popen(
                    [npm_cmd, "run", "dev"],
                    cwd=str(FRONTEND_DIR),
                    env=env,
                    stdout=subprocess.PIPE,
                    stderr=subprocess.STDOUT,
                    text=True,
                    bufsize=1,
                )
                procs.append(("Next.js Frontend (3000)", proc_frontend))
                t_front = threading.Thread(
                    target=stream_output, args=(proc_frontend.stdout, TAG_FRONT), daemon=True
                )
                t_front.start()

        # Wait briefly and display handy status dashboard
        time.sleep(2)
        print()
        print(f"{BOLD}{GREEN}==============================================================={RESET}")
        print(f"{BOLD}{GREEN}              🚀 All requested services are launching!          {RESET}")
        print(f"{BOLD}{GREEN}==============================================================={RESET}")
        if not args.no_frontend:
            print(f"{TAG_SYS}🌐 Web Dashboard    : {BOLD}{CYAN}http://localhost:3000{RESET}")
        if not args.no_backend:
            print(f"{TAG_SYS}⚡ Live LSTM Data   : {BOLD}{CYAN}http://localhost:8000/api/live_data{RESET}")
            print(f"{TAG_SYS}🔬 EMT Solver Docs  : {BOLD}{CYAN}http://localhost:8001/docs{RESET}")
        print(f"{TAG_SYS}ℹ️  Press {BOLD}{RED}Ctrl+C{RESET} at any time to gracefully stop all services.")
        print(f"{BOLD}{GREEN}==============================================================={RESET}\n")

        # Optional auto-browser opening
        if args.open and not args.no_frontend:
            def open_browser():
                print(f"{TAG_SYS}Waiting for frontend port 3000 to become responsive...")
                if wait_for_port(3000, timeout=20.0):
                    print(f"{TAG_SYS}Opening browser to http://localhost:3000 ...")
                    webbrowser.open("http://localhost:3000")
            threading.Thread(target=open_browser, daemon=True).start()

        # Main supervision loop: monitor child processes
        while True:
            for name, proc in procs:
                ret = proc.poll()
                if ret is not None:
                    print(f"{TAG_SYS}{RED}⚠️  Service '{name}' exited unexpectedly with code {ret}!{RESET}")
                    raise KeyboardInterrupt
            time.sleep(1)

    except KeyboardInterrupt:
        print(f"\n{TAG_SYS}{YELLOW}Shutdown signal received (Ctrl+C). Terminating all services...{RESET}")
    finally:
        for name, proc in procs:
            print(f"{TAG_SYS}Stopping {name} (PID: {proc.pid})...")
            kill_process_tree(proc.pid)
        print(f"{TAG_SYS}{GREEN}All services stopped cleanly. Goodbye!{RESET}\n")


if __name__ == "__main__":
    main()
