import json
import subprocess
import sys

literal = subprocess.check_output([sys.argv[1]], encoding="utf-8").splitlines()
expected = "".join(chr(i) for i in range(32)) + '"\\日本語'
assert json.loads(literal[0]) == expected
assert [json.loads(line) for line in literal[1:]] == [
    -(2**63), 2**63 - 1, 1e-7, 1.2345678901234567, None, None, None,
]
