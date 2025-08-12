
sources = [
  'rlpw/pw-queue.c',
  'rlpw/pw-task.c',
  'rlpw/pw.c',
]

headers = [
  'rlpw/pw-queue.h',
  'rlpw/pw-sched.h',
  'rlpw/pw-task.h',
  'rlpw/pw-user.h',
  'rlpw/pw.h',
  ]

rlc_dep = dependency('rlc', fallback : ['rlc', 'rlc_dep'], default_options: ['default_library=static'])

install_headers('rlpw.h')
install_headers(headers, subdir: 'rlpw')
librlpw = library('rlpw',
  sources,
  dependencies: [rlc_dep],
  install: true,
  )

pkg = import('pkgconfig')
pkg.generate(
  libraries: librlpw,
  version: '9999',
  name: 'rlpw',
  description: 'Parallel worker in C.',
  )

rlpw_dep = declare_dependency(
  link_with: librlpw,
  include_directories: '.',
  )

