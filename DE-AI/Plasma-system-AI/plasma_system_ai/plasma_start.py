import subprocess
import os
import argparse
import sys

def main():
    # 🔧 Dizin oluşturma kısmı
    ai_logs_dir = os.path.expanduser(".ai_logs")
    if not os.path.exists(ai_logs_dir):
        os.makedirs(ai_logs_dir)
        print(f"[+] Directory created: {ai_logs_dir}")

    parser = argparse.ArgumentParser(
        description="Plasma System-AI Starter\nStarts selected modules of the Plasma AI system.",
        epilog="""
Examples:
  python plasma_start.py --run-main-fregio --duration 120 --delay 5
  python plasma_start.py --run-readout-tens
  python plasma_start.py --run-main-fregio --run-readout-tens --duration 90
        """,
        formatter_class=argparse.RawDescriptionHelpFormatter
    )

    parser.add_argument(
        '--run-main-fregio',
        action='store_true',
        help='Run the system telemetry logger (main-fregio.py)'
    )
    parser.add_argument(
        '--run-readout-tens',
        action='store_true',
        help='Run the output writer or secondary AI task (writereadout-tens.py)'
    )
    parser.add_argument(
        '--duration',
        type=int,
        default=60,
        help='Logging duration in seconds for main-fregio.py (default: 60)'
    )
    parser.add_argument(
        '--delay',
        type=int,
        default=0,
        help='Delay in seconds before main-fregio.py starts (default: 0)'
    )

    args = parser.parse_args()

    current_dir = os.path.dirname(os.path.abspath(__file__))
    python_exec = sys.executable  # Universal Python executable

    if args.run_main_fregio:
        subprocess.run([
            python_exec,
            os.path.join(current_dir, "main-fregio.py"),
            "--duration", str(args.duration),
            "--delay", str(args.delay)
        ], check=True)

    if args.run_readout_tens:
        subprocess.run([
            python_exec,
            os.path.join(current_dir, "writereadout-tens.py")
        ], check=True)
