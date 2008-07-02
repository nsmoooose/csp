clear
echo ====================================================================
echo Fix the following dependencies. cspwf should not depend on cspsim.
find cspwf/ -name "*.h" -or -name "*.cpp"|xargs grep csp/cspsim/ --color
