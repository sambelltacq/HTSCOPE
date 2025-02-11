# Running HTSCOPE

Follow the installation instructions in README.md

* install Phoebus
* install EPICS BASE
* install AFHBA404
* git clone HTSCOPE to PROJECTS/HTSCOPE
* Built htscope1, the ioc

### Create a db definition

```
cd PROJECTS/HTSCOPE/htscope1
./scripts/make_htscope_st.cmd.py --user=dt100 --nchan=32 --data32=0 acq1102_010
```

### Run the servers

```
cd PROJECTS/HTSCOPE/htscope1
./init/start_servers
```

#### Optionally, run consoles on the servers

```
cd PROJECTS/HTSCOPE/htscope1
./scripts/epics-console
./scripts/htstream-console
```

### Run Phoebus

```
cd PROJECTS/HTSCOPE
./cs-studio/phoebus/run_ht_scope_ui.sh
```

