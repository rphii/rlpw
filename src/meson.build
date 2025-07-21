
sources = [
  'pw-queue.c',
  'pw-task.c',
  'pw.c',
]

headers = [
  'pw-queue.h',
  'pw-sched.h',
  'pw-task.h',
  'pw-user.h',
  'pw.h',
  ]

dep_rlc = dependency('rlc', required: true)

install_headers(headers, subdir: 'rl')
rlpw = library('rlpw',
  sources,
  dependencies: [dep_rlc],
  install: true,
  )

