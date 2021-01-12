# Add deb-src entries
sed -n '/^deb\s/s//deb-src /p' /etc/apt/sources.list > /etc/apt/sources.list.d/deb-src.list

apt-get update && eatmydata apt-get install --no-install-recommends -y \
     aptitude \
     devscripts \
     ccache \
     equivs \
     build-essential

eatmydata install-build-deps.sh .

# Generate ccache links
dpkg-reconfigure ccache
PATH="/usr/lib/ccache/:${PATH}"

# Reset ccache stats
ccache -z
