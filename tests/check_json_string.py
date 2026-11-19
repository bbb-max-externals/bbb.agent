import json
import subprocess
import sys

literal = subprocess.check_output([sys.argv[1]], encoding="utf-8")
expected = "".join(chr(i) for i in range(32)) + '"\\日本語'
assert json.loads(literal) == expected
