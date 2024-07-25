from distutils.core import setup
from distutils.extension import Extension
import os
import platform

openmm_dir = '@OPENMM_DIR@'
openmmxtb_header_dir = '@OPENMMXTB_HEADER_DIR@'
openmmxtb_library_dir = '@OPENMMXTB_LIBRARY_DIR@'

# setup extra compile and link arguments on Mac
extra_compile_args=['-std=c++11']
extra_link_args = []

if platform.system() == 'Darwin':
    extra_compile_args += ['-stdlib=libc++', '-mmacosx-version-min=10.7']
    extra_link_args += ['-stdlib=libc++', '-mmacosx-version-min=10.7', '-Wl', '-rpath', openmm_dir+'/lib']

extension = Extension(name='openmmxtb._openmmxtb',
                      sources=['XtbPluginWrapper.cpp'],
                      libraries=['OpenMM', 'OpenMMXTB'],
                      include_dirs=[os.path.join(openmm_dir, 'include'), openmmxtb_header_dir],
                      library_dirs=[os.path.join(openmm_dir, 'lib'), openmmxtb_library_dir],
                      extra_compile_args=extra_compile_args,
                      extra_link_args=extra_link_args
                     )

setup(name='OpenMMXTB',
      version='0.1',
      packages=['openmmxtb'],
      ext_modules=[extension],
      package_data = {"openmmxtb" : ['forcefields/xtb/*.xml']},
      entry_points={
          'openmm.forcefielddir' : ['forcefields = openmmxtb:_get_forcefield_dir']
      },
     )
