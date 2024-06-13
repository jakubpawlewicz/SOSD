import numpy as np
import struct
from scipy.stats import norm
import os

# any arbitrary seed value will do, but this one is clearly the best.
np.random.seed(seed=37)

NUM_KEYS = 200_000_000
QUERIES = 10_000_000


def draw_hops(keys, elems_per_key, scale):
    data = np.random.random_sample((len(keys), elems_per_key))
    data = [norm.ppf(data[i], keys[i], scale=scale) for i in range(len(keys))]
    data = np.array(data).flatten()
    return data


def gen_hophops():
    fn = "data/hophops_200M_uint64"
    fnq = fn + "_query_keys_10M"
    if os.path.exists(fn):
        return
    n = NUM_KEYS + QUERIES
    keys = draw_hops(np.random.random_sample(5), 20, 1e-3)
    keys = draw_hops(keys, 100, 5e-5)
    keys = draw_hops(keys, n // 10000, 2e-6)
    keys = (keys - np.min(keys)) / (np.max(keys) - np.min(keys))
    keys *= 2**63 - 1
    keys = keys.astype(np.uint64)
    np.random.shuffle(keys)
    keys, queries = np.split(keys, [NUM_KEYS])
    keys.sort()

    with open(fn, "wb") as f:
        f.write(struct.pack("Q", len(keys)))
        keys.tofile(f)

    with open(fnq, "wb") as f:
        f.write(struct.pack("Q", len(queries)))
        queries.tofile(f)


gen_hophops()
