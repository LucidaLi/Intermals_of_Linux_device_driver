1. navigate into ./app folder and build it with command: gcc main.c -o main
2. the application 'main' in the ./app folder will be used as '/sbin/hotplug', so we need to copy it to /sbin folder with below command:
   #cp ./main /sbin/hotplug
3. navigate into the /proc/sys/kernel folder, execute the below command to make the hotplug application can be trigered:
   #echo "/sbin/hotplug" > hotplug
   make sure the hotplug file has the string with 'cat'command: #cat /proc/sys/kernel/hotplug, it should output the "/sbin/hotplug"
4. the syslog info will be routed to '/var/log/syslog' file, or '/var/log/messages' in some systems...

This illustration demonstrates how the user space to monitor events happened in kernel space...
   