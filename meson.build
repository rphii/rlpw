project('bgw', 'c',
  meson_version: '>=1.1')

subdir('src')

pkg = import('pkgconfig')
pkg.generate(
  libraries: rphiibgw,
  version: '9999',
  name: 'rphiibgw',
  description: 'Multi-threaded background worker in C.',
  )

