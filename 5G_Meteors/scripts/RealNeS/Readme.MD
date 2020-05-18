# Prerequisites for running PeriodicCopy.py

PeriodicCopy.py executes a remote copy between the Simulator machine and the client machine 
using scp in periodic manner. This operation is executed quite often and for copying very few bytes ( < 80 bytes>.  

## Setting up Paswordless SSH login
* Create Authentication SSH-Keygen Keys on Simulator Machine
* Create .ssh Directory on client machine
* Upload Generated Public Keys to remote client Machine
* Set Permissions on client machine
* Login from Simulator to client machine


### Tunning SSH configuration to speed up connection

* Force SSH Connection Over IPV4:
use the AddressFamily (specifies the address family to use when connecting) directive in your ssh configuration 
file /etc/ssh/ssh_config (global configuration) or ~/.ssh/config (user specific file). 
The accepted values are “any”, “inet” for IPv4 only, or “inet6”.

* Disable DNS Lookup On Remote Machine
By default, sshd daemon looks up the remote host name, and also checks that the resolved host name for the remote IP address
maps back to the very same IP address. This can result into delays in connection establishment or session creation.
The UseDNS directive controls the above functionality; to disable it, search and uncomment it in the /etc/ssh/sshd_config file.
If it’s not set, add it with the value no.
```
UseDNS no
```
* Reuse SSH Connection
An ssh client program is used to establish connections to an sshd daemon accepting remote connections. 
You can reuse an already-established connection when creating a new ssh session and this can significantly speed up subsequent sessions.
You can enable this in your ~/.ssh/config file.
```
Host *
	ControlMaster auto
	ControlPath  ~/.ssh/sockets/%r@%h-%p
	ControlPersist 600
```

* Use Specific SSH Authentication Method
Another way of speeding up ssh connections is to use a given authentication method for all ssh connections, and here it is recommended
to configure ssh passwordless login.
Once that is done, use the PreferredAuthentications directive, within ssh_config files (global or user specific) above.
This directive defines the order in which the client should try authentication methods (you can specify a command 
separated list to use more than one method).
```
PreferredAuthentications=publickey 
```
