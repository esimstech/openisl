#! /bin/bash

ROOT_PATH=`pwd`

source $ROOT_PATH/00_env.sh
source $PYTHON_VENV_PATH/bin/activate
python3 -m pip install wheel
python3 -m pip install dist/openisl_python_api-1.0.0.zip
