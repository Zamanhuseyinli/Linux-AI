import subprocess
import os
import argparse
import sys

def main():
    # 🔧 Dizin oluşturma kısmı
    ai_logs_dir = os.path.expanduser("~/.ai_logs")
    if not os.path.exists(ai_logs_dir):
        os.makedirs(ai_logs_dir)
        print(f"[+] Directory created: {ai_logs_dir}")

    parser = argparse.ArgumentParser(
        description="Plasma System-AI Starter\nStarts selected modules of the Plasma AI system.",
        epilog="""
Examples:
  python plasma_start.py --run-main-fregio --duration 120 --delay 5
  python plasma_start.py --run-readout-tens --tens-analyze --tens-suggest
  python plasma_start.py --run-main-fregio --run-readout-tens --duration 90
        """,
        formatter_class=argparse.RawDescriptionHelpFormatter
    )

    # Ana modül bayrakları
    parser.add_argument('--run-main-fregio', action='store_true',
                        help='Run the system telemetry logger (main-fregio.py)')
    parser.add_argument('--run-readout-tens', action='store_true',
                        help='Run the output writer or secondary AI task (writereadout-tens.py)')

    # main-fregio.py argümanları
    parser.add_argument('--duration', type=int, default=60,
                        help='Logging duration in seconds for main-fregio.py (default: 60)')
    parser.add_argument('--delay', type=int, default=0,
                        help='Delay in seconds before main-fregio.py starts (default: 0)')

    # writereadout-tens.py argümanları
    parser.add_argument('--tens-analyze', action='store_true',
                        help='Run analysis mode in writereadout-tens.py')
    parser.add_argument('--tens-suggest', action='store_true',
                        help='Run suggestion mode in writereadout-tens.py')
    parser.add_argument('--tens-loop', action='store_true',
                        help='Run monitoring loop in writereadout-tens.py')
    parser.add_argument('--tens-auto-prop', action='store_true',
                        help='Enable auto-prop in writereadout-tens.py')
    parser.add_argument('--tens-most-intensive', action='store_true',
                        help='Show most intensive process in writereadout-tens.py')
    parser.add_argument('--tens-interval', type=int, default=60,
                        help='Interval for monitor loop (default: 60 seconds)')
    parser.add_argument('--tens-get-interval', choices=["s", "m"],
                        help='Time unit for writereadout-tens.py (s for seconds, m for minutes)')

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
        readout_args = [python_exec, os.path.join(current_dir, "writereadout-tens.py")]

        if args.tens_analyze:
            readout_args.append("--analyze")
        if args.tens_suggest:
            readout_args.append("--suggest")
        if args.tens_loop:
            readout_args.append("--loop")
        if args.tens_auto_prop:
            readout_args.append("--auto-prop")
        if args.tens_most_intensive:
            readout_args.append("--most-intensive")

        readout_args.extend(["--interval", str(args.tens_interval)])

        if args.tens_get_interval:
            readout_args.extend(["--get-interval", args.tens_get_interval])

        subprocess.run(readout_args, check=True)

if __name__ == "__main__":
    main()
