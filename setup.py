from distutils.core import setup, Extension


setup(name='jwinkler_c', version='1.0',
      ext_modules=[Extension('jwinkler_c', ['algos/jwinkler.c'])]
)

