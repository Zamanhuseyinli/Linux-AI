from setuptools import setup, find_packages
from setuptools import setup
setup()


setup(
    name='proc-utils-AI',
    version='0.1.0',
    author='Polat Alemdar',
    description='AI-powered process and CPU utility tools',
    packages=find_packages(),
    install_requires=[
        'pyttsx3',
        'psutil',
        'PyYAML',
        'watchdog',
        'matplotlib',
        'seaborn',
        'numpy',
        'torch',
        'scikit-learn',
        'pandas'
    ],
    entry_points={
        'console_scripts': [
            'ai-cpufregd=cbinder-gbfs.ai-cpufregd:main',
            'ai-voicer=cbinder-gbfs.ai-voicer:main',
            'ai-analyzer=cbinder-gbfs.ai-analyzer:main',
            'cpu-mapping-visualizer=cbinder-gbfs.cpu-mapping-visualizer:main',
            'ai-lfs=cbinder-gbfs.ai-lfs:main',
            'train-ai-model=cbinder-gbfs.train_ai_model:main',
            'ai-affinity=cbinder-gbfs.ai-affinity:main',
            'simulate-io-stats=cbinder-gbfs.simulate_io_stats:main',
            'ai-greeting=cbinder-gbfs.ai-greeting:main'
        ],
    },
    python_requires='>=3.8',
)
