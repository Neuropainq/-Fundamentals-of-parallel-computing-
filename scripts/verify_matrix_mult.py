from pathlib import Path
import numpy as np
import re


path_data = Path.cwd() / "data"
for c_file in path_data.glob('C_*.txt'):
    n = int(re.search(r'C_(\d+)\.txt', c_file.name).group(1))

    A = np.loadtxt(path_data / f'A_{n}.txt', dtype=np.int64)
    B = np.loadtxt(path_data / f'B_{n}.txt', dtype=np.int64)
    C = np.loadtxt(path_data / f'C_{n}.txt', dtype=np.int64)

    ok = np.array_equal(A @ B, C)
    print(f'n={n}: {ok}')