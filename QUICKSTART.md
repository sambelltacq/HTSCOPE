# Instructions to run a shot after install


## Setup AFHBA404:
```
cd ~/PROJECTS/AFHBA404
sudo ./scripts/loadNIRQ
sudo ./scripts/mount-ramdisk
```
Helper:
```
export UUTS="acq1102_043 acq1102_044 acq1102_045 acq1102_046"
```

## Setup clocking Sync role uuts:

```
cd ~/PROJECT/acq400_hapi

#Sync role uuts
./user_apps/acq400/sync_role.py --fclk=2000000 --fin=1000000 --toprole=master,fptrg $UUTS
```

## Setup htscope
```
cd ~/PROJECTS/HTSCOPE/htscope1

#Create Symlinks
./scripts/create_symlinks.sh dt100

#Create st.cmd
./scripts/make_htscope_st.cmd.py --nchan=32 --data32=0 --ndata=100000 --host=shuna --user=dt100 $UUTS
```

## Run htscope:

```
#Start Ioc and hts
./init/start_servers
```

## Run ui
```
~/PROJECTS/HTSCOPE/cs-studio/phoebus/run_ht_scope_ui.sh --host=shuna
```




