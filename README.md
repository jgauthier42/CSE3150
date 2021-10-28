# MAKEFILE TEMPLATE

## Description

This script automatically creates a Makefile for your project. Currently it only supports C/C++/C#, but I will extend it in the future to support other languages. The bulk of the logic which I coded is in ECEditor. 

## Usage

The script can be installed as follow:

```bash
git clone https://github.com/isthattyler/Makefile-Template.git
cd Makefile-Template
./install
```

After the installation process, you can use it right away. If you already have source files and your main file is named 'foo.cpp', creating a Makefile for it and run it is as simple as:

```bash
createmf foo.cpp
make
make clean
```

