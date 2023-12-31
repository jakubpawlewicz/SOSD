import numpy as np
import struct
from scipy.stats import norm
import os

# any arbitrary seed value will do, but this one is clearly the best.
np.random.seed(seed=42)

NUM_KEYS = 200_000_000

def gen_hops(bits, hops, hops_str):
    fn = "data/hops_" + hops_str + "_200M_uint" + str(bits)
    if os.path.exists(fn):
        return
    print(hops_str + " hops " + str(bits) + " bit...")
    elems_per_hop = NUM_KEYS // hops
    loc = np.random.random_sample((hops,))
    loc.sort()
    scale = 1.0 / 5.0 / hops
    keys = np.random.random_sample((hops, elems_per_hop))
    keys = [norm.ppf(keys[i], loc[i], scale=scale) for i in range(hops)]
    keys = np.array(keys).flatten()
    keys.sort()
    keys = (keys - np.min(keys)) / (np.max(keys) - np.min(keys))
    keys *= 2**min(bits, 63) - 1
    keys = keys.astype(np.uint32 if bits == 32 else np.uint64)

    with open(fn, "wb") as f:
        f.write(struct.pack("Q", len(keys)))
        keys.tofile(f)

print("Generating hops data...")
gen_hops(32, 100, '100')
gen_hops(64, 100, '100')
gen_hops(32, 10_000, '10k')
gen_hops(64, 10_000, '10k')
gen_hops(32, 1_000_000, '1m')
gen_hops(64, 1_000_000, '1m')
