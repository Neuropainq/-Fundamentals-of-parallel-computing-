from pathlib import Path
import subprocess
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

root = Path(__file__).resolve().parent
exe = root / "matrixmult" / "matrixmult.exe"
out_png = root / "data" / "time_vs_size.png"

result = subprocess.run([str(exe)], capture_output=True, text=True)
if result.returncode != 0:
    print("matrixmult error")
    print(result.stdout)
    print(result.stderr)
    raise SystemExit(1)

sizes = []
times = []

for line in result.stdout.splitlines():
    # Expected line: N=100 | Time: 15 ms | ...
    if line.startswith("N=") and "Time:" in line:
        parts = line.split("|")
        n = int(parts[0].strip().replace("N=", ""))
        t = int(parts[1].strip().replace("Time:", "").replace("ms", ""))
        sizes.append(n)
        times.append(t)

if not sizes:
    print("No timing data found")
    print(result.stdout)
    raise SystemExit(1)

plt.figure(figsize=(8, 5))
plt.plot(sizes, times, marker="o")
plt.title("Time vs Matrix Size")
plt.xlabel("N")
plt.ylabel("Time (ms)")
plt.grid(True)
plt.tight_layout()
plt.savefig(out_png, dpi=150)

print("Saved:", out_png)
