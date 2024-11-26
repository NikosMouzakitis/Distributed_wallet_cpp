# Distributed_wallet_cpp Distributed system, containing nodes, where wallets are connected to submit transactions.



# Registration of a new backbone node.
When a new backbone node registers, it sends a message to bootstrap node, which then informs all of the other already registered
nodes of the distributed system.

Bootstrap node runs on port :8080 on the local machine.

* A peer who want to register with the bootstrap node, is sending a message of the form "REGISTER (IP) (port)".


![img](https://github.com/NikosMouzakitis/Distributed_wallet_cpp/blob/main/img/addpeer.jpg)
