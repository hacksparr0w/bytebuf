project(
    'bytebuf',
    'c',
    license: 'MIT',
    version : '1.0',
    default_options : [
      'c_std=c99',
      'warning_level=3'
    ]
)

project_description = 'A pure C implementation of a high-performance, zero-copy byte buffer'

project_headers = [
  'include/bytebuf.h'
]

project_source_files = [
  'src/bytebuf.c'
]

build_args = [

]

public_headers = include_directories('include')

build_args += [
  '-DPROJECT_NAME=' + meson.project_name(),
  '-DPROJECT_VERSION=' + meson.project_version()
]

if target_machine.system() == 'windows' or target_machine.system() == 'cygwin'
  build_args += '-DBYTEBUF_PUBLIC=__declspec(dllexport)'
else
  build_args += '-DBYTEBUF_PUBLIC=__attribute__((visibility("default")))'
endif

project_target = shared_library(
  meson.project_name(),
  project_source_files,
  install : true,
  c_args : build_args,
  include_directories : public_headers
)

bytebuf_dep = declare_dependency(
  include_directories : public_headers,
  link_with : project_target
)

install_headers(project_headers)

pkg_mod = import('pkgconfig')
pkg_mod.generate(
  name : meson.project_name(),
  filebase : meson.project_name(),
  description : project_description,
  subdirs : meson.project_name(),
  libraries : project_target
)

if not meson.is_subproject()
  unity_subproject = subproject('unity')
  unity_dep = unity_subproject.get_variable('unity_dep')
  unity_gen_runner = unity_subproject.get_variable('gen_test_runner')

  test(
    'test bytebuf',
    executable(
      'test_bytebuf',
      sources: [
        'tests/test_bytebuf.c',
        unity_gen_runner.process('tests/test_bytebuf.c')
      ],
      dependencies : [bytebuf_dep, unity_dep]
    )
  )
endif