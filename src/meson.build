
sources = [
  'bgw-queue.c',
  'bgw-task.c',
  'bgw.c',
]

headers = [
  'bgw-queue.h',
  'bgw-sched.h',
  'bgw-task.h',
  'bgw-user.h',
  'bgw.h',
  ]

dep_rphiic = dependency('rphiic', required: true)

install_headers(headers, subdir: 'rphii/bgw')
rphiibgw = library('rphiibgw',
  sources,
  dependencies: [dep_rphiic],
  install: true,
  )

