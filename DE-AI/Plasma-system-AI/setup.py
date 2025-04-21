from setuptools import setup, find_packages
import pathlib

# README.md içeriğini oku
here = pathlib.Path(__file__).parent
long_description = (here / "README.md").read_text(encoding="utf-8")

setup(
    name='Plasma-AI',
    version='1.0.0',
    author='Zaman Huseyinli',
    author_email='admin@azccriminal.space',
    description='DE-AI: An intelligent Linux desktop AI system that manages and optimizes your environment.',
    long_description=long_description,
    long_description_content_type='text/markdown',
    url='https://github.com/Zamanhuseyinli/Linux-AI',
    license='GPLv2',
    packages=find_packages(),
    include_package_data=True,
    install_requires=[
        'psutil',
        'tensorflow',
    ],
    entry_points={
        'console_scripts': [
            'plasma-start=plasma_system_ai.plasma_start:main',
        ],
    },
    classifiers=[
        'Programming Language :: Python :: 3',
        'License :: OSI Approved :: GNU General Public License v2 (GPLv2)',
        'Operating System :: POSIX :: Linux',
        'Intended Audience :: Developers',
        'Topic :: System :: Monitoring',
        'Topic :: System :: Operating System Kernels :: Linux',
        'Topic :: Software Development :: Libraries :: Application Frameworks',
    ],
    python_requires='>=3.7',
)
