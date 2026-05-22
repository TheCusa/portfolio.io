#!/usr/bin/env bash
set -e

# Script used to release all Git LFS locks owned by the current user in a
# single batch.
# Yes, this is a slow process, even by using an automated script.
# Git LFS is slow as hell, so be prepared to leave it working for some time.
#
# Note: you need to have a working configuration of the UE Git LFS 2 plugin,
# specifically you need to set your GitLab username, otherwise the script will
# just try to unlock everyone's files, making you waste a lot of time.


: ${chunk_size:=16}
: ${max_parallel_procs:=16}
: ${list_only:=false}

usage () {
  >&2 cat <<- EOF
	usage: ./unlock-all.sh [-fhl] [-u <username>]
	args:
	  -f, --force   Force unlock files, used for unlocking other users' files
	  -l, --list    List user's locks only, without unlocking
	  -u, --user    Manually set the username.
	                Pair with -f for unlocking other users' files
	  -h, --help    Show this message
	EOF
}

# Get GitLab username for the current user
get_user () {
  local platform lfs_plugin_file

  # Identify platform
  case $(uname -o) in
    # Git Bash relies on MSYS2 on Windows
    Msys | MINGW*) platform=WindowsEditor ;;
    Darwin) platform=MacEditor ;;
    GNU/Linux)
      echo >&2 "warning: the script is untested on Linux"
      platform=LinuxEditor ;;
    *) echo >&2 "error: not supported on platform '$(uname -o)'"
      return 1
      ;;
  esac

  lfs_plugin_file="Saved/Config/$platform/SourceControlSettings.ini"
  if [[ -f "$lfs_plugin_file" ]]; then
    # Read Git LFS plugin config file and filter for the username line 
    < "$lfs_plugin_file" grep 'LfsUserName=' |
    # Filter key out, keeping the username value only
    sed 's/LfsUserName=//'

    return 0
  fi

  echo >&2 "Git LFS plugin config file not found in '$lfs_plugin_file'"
  echo >&2 'Unlocking files from all users, it will take much longer...'
  return 1

  # Just use an empty user, so that no file will be filtered out
}

# Additional arguments for the unlock command
args=()

while (( $# > 0 )); do
  case "$1" in
    -f | --force) args+=(-f)
      shift
      ;;
    -l | --list) list_only=true
      shift
      ;;
    -u | --user) user="$2"
      shift 2
      ;;
    -h | --help) usage
      exit 0
      ;;
    *) echo >&2 "option '$1' is not supported"
      usage
      exit 1
      ;;
  esac
done

[[ -n "$user" ]] || user="$(get_user)"

# List of all locked files (by the current user)
locked_files=()

# Split string on newline instead of space as per default.
# This allows to correctly get paths containing spaces.
IFS=$'\n'

# Iterate for all locked files
for file in $( \
  # List all locks
  git lfs locks |
  # Filter for current user locks
  grep -F "$user" |
  # Return the file path
  sed 's|[[:space:]]*[^[:space:]]*[[:space:]]*ID:.*$||')
do
  # Add file to list
  locked_files+=("$file")
done

# Reset the default word separator
unset IFS

# Just exit if no file is locked
(( "${#locked_files[@]}" > 0 )) || {
  echo >&2 "User '$user' owns no lock"
  exit 0
}

if $list_only; then
  printf "%s\n" "${locked_files[@]}"
  exit 0
fi

# Unlock the entire batch of files in parallel chunks
printf "%s\0" "${locked_files[@]}" |
xargs -0 -n $chunk_size -P $max_parallel_procs git lfs unlock "${args[@]}" -- || true
