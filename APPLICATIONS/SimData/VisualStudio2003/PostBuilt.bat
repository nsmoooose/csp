copy /Y cSimData.py ..\SimData
cd ..
setup.py make_install_win
runtests.py