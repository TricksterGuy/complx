pkgname=complx
pkgver=r44.6340dfc
pkgrel=1
pkgdesc="the CS2110 LC-3 simulator"
arch=(i686 x86_64)
url="https://github.com/TricksterGuy/complx"
depends=('wxgtk')
makedepends=('git' 'webkitgtk2')
options=(makeflags)

pkgver() {
  ( set -o pipefail
    git describe --long --tags 2>/dev/null | sed 's/\([^-]*-g\)/r\1/;s/-/./g' ||
    printf "r%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"
  )
}

build() {
  cd ..
  make PREFIX=/usr
}

package() {
  cd ..
  mkdir -p $pkgdir/usr
  make install PREFIX=$pkgdir/usr
}
