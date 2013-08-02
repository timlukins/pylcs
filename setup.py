from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext
 
setup(
  name='pylcs',
  description='Python Learning Classifier System',
  ext_modules=[
    Extension("xcs", ["xcs.pyx", "LCS_XCS.cpp"],language="c++",)
  ],
  cmdclass = {'build_ext': build_ext}
)
