# BUILD EPICS, fixed version with modules, OMIT linux-arm
# The result is suitable for HTSCOPE
mkdir PROJECTS
git clone --recurse-submodules https://github.com/D-TACQ/ACQ400_ESW_TOP.git
cd ACQ400_ESW_TOP/EPICS7/
cd base
cat README.ACQ400
sudo apt-get install re2c libtirpc-dev rpcsvc-proto

execute
```
cd ..
if [ -d /usr/local/epics ]; then     echo DANGER /usr/local/epics already exists, quit; exit 1; fi
sudo ln -s ${PWD} /usr/local/epics
ln -s acq400_epics_base base
cd base;
```

# Build Base
```
source setup.env
# comment out CROSS_COMPILER_TARGET_ARCHS for no linux-arm
vi configure/CONFIG_SITE
make 
make install
```
# Build Modules
```
cd ../modules/
# comment out CROSS_COMPILER_TARGET_ARCHS for no linux-arm
vi seq/configure/CONFIG_SITE 
for dir in *;do (cd $dir; make); done
```


