#!/usr/bin/env python3

import sys
import os

def main():
    if len(sys.argv) < 2:
        print("Error: No command specified. Use 'ce', 'ssb_sf1', 'ssb_sf10' or 'artificial'.", file=sys.stderr)
        sys.exit(1)

    command = sys.argv[1]
    args = sys.argv[2:]

    if command == "ce":
        executable = "/factdb/cmake-build-release/benchAll"
        args += ["ce"]
    elif command == "artificial":
        executable = "/factdb/cmake-build-release/benchArtificial"
    elif command == "ssb_sf1":
        executable = "/factdb/cmake-build-release/benchAll"
        args += ["ssb_sf1"]
    elif command == "ssb_sf10":
        executable = "/factdb/cmake-build-release/benchAll"
        args += ["ssb_sf10"]
    else:
        print(f"Error: Unknown command '{command}'. Expected 'ce' or 'artificial'.", file=sys.stderr)
        sys.exit(1)

    # Replace the current process with the executable
    os.execvp(executable, [executable] + args)

if __name__ == "__main__":
    main()
