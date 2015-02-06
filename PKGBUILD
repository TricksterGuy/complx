pkgname=complx
pkgver=r43.11b2557
pkgrel=1
pkgdesc="the CS2110 LC-3 simulator"
arch=(i686 x86_64)
url="https://github.com/TricksterGuy/complx"
depends=('wxgtk')
makedepends=('git' 'webkitgtk2')
source=(complx::git+https://github.com/TricksterGuy/complx)
md5sums=('SKIP')
options=('makeflags')

pkgver() {
  cd "${srcdir}/${pkgname}"
  ( set -o pipefail
    git describe --long --tags 2>/dev/null | sed 's/\([^-]*-g\)/r\1/;s/-/./g' ||
    printf "r%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"
  )
}

build() {
  cd "${srcdir}/${pkgname}"
  make PREFIX=/usr
}

package() {
  cd "${srcdir}/${pkgname}"
  mkdir -p $pkgdir/usr
  make install PREFIX=$pkgdir/usr
}
