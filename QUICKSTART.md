# Instructions to run a shot after install


<pre>
export UUTS="acq1102_043 acq1102_044 acq1102_045 acq1102_046"
./user_apps/acq400/sync_role.py --fclk=2000000 --fin=1000000 --toprole=master,fptrg $UUTS
~/PROJECTS/HTSCOPE/cs-studio/phoebus/run_ht_scope_ui.sh --host=shuna
</pre>
