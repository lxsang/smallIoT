# smallIoT

SmallIoT is a barebone VM (based on the Little Smalltalk VM) that run on the Raspberry Pi without any OS installed. The VM is loaded when the Pi boots, then it loads the Smalltalk image on the SD Card. The communication with the VM on the Pi is established via the UART line using a terminal application (e.g. picocom), in which user can directly type Smalltalk code to program and control the Pi in real-time.
