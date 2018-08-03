load("//fatal/facebook/benchmark:build_defs.bzl", "fatal_benchmark")

def fatal_trie_benchmark(deps=None, **kwargs):
    deps = (deps or []) + ["//fatal/type/benchmark/trie:test_cases"]
    fatal_benchmark(deps=deps, **kwargs)
