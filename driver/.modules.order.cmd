cmd_/home/jjuser/working/term_project/EMB/driver/modules.order := {   echo /home/jjuser/working/term_project/EMB/driver/step_motor_driver.ko;   echo /home/jjuser/working/term_project/EMB/driver/u_sonic_driver.ko; :; } | awk '!x[$$0]++' - > /home/jjuser/working/term_project/EMB/driver/modules.order
