from  vendor.pybind11.pybind11.setup_helpers import *
from setuptools import setup,Extension




setup(
    name="SimuDracoPython",
    version="0.0.1",
    ext_modules=[Pybind11Extension("SimuDracoPython",["source/module.cpp"])]
)