from distutils.core import setup, Extension

personal_module = Extension('personal', 
                            sources=['personalmodule.c'])

setup(
    name='personal',
    version='0.1',
    author="M.Kojro",
    author_email="mateusz.kojro@edu.uni.lodz.pl",
    description='Personal Python extension',
    ext_modules=[personal_module]
)
