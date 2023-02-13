# interboard communication

This firmware illustrate how 2 HydraUSB3 boards can be used together.

The following happens :
```
         --USB2->             ---HSPI-->  
Host PC            Top Board              Bottom Board (rot13)
         <-USB2--             <-SerDes--  
```

The host PC sends a message to the bottom board, the bottom board cypher the message (with rot13 encryption) and sends it back to host.

To send a message to software used is located in `host-controller/`.
