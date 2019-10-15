#!/bin/bash
#
# Create versioned gzipped tarball of current branch of git repo
# including all sub-modules.

set -e

url=$(git remote get-url origin)
project="${url##*/}"
project="${project%.git}"
#version="$(git describe --abbrev=0)$(git log -n 1 --pretty=format:"+%cd~git%h" --date=format:%Y%m%d master)"
version="$(git describe --abbrev=0)"
version="${version#v}"

# https://github.com/Kentzo/git-archive-all (install with e.g. `pipx install git-archive-all`)
git-archive-all --verbose --prefix "$project-$version/" $project-$version.tar
gzip "$project-$version.tar"
rm -rf "$project-$version.tar"
