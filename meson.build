project('rlpt', 'c',
  meson_version: '>=1.1')

subdir('src')

pkg = import('pkgconfig')
pkg.generate(
  libraries: rlpw,
  version: '9999',
  name: 'rlpw',
  description: 'Parallel worker in C',
  )

