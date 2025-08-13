project(
    'bytebuf',
    'c',
    version : '1.0',
    default_options : ['warning_level=3']
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

project_target = shared_library(
  meson.project_name(),
  project_source_files,
  install : true,
  c_args : build_args,
  gnu_symbol_visibility : 'hidden',
  include_directories : public_headers
)

project_dependency = declare_dependency(
  include_directories : public_headers,
  link_with : project_target
)

set_variable(
    meson.project_name() + '_dep',
    project_dependency
)

install_headers(
    project_headers,
    subdir: meson.project_name()
)

pkg_mod = import('pkgconfig')
pkg_mod.generate(
  name : meson.project_name(),
  filebase : meson.project_name(),
  description : project_description,
  subdirs : meson.project_name(),
  libraries : project_target
)

if not meson.is_subproject()
  test(
    'test basic operations',
    executable(
      'test_basic_operations',
      'tests/test_basic_operations.c',
      dependencies : [project_dependency]
    )
  )
endif
