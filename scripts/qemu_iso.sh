#!/bin/bash
export KERN_PATH=bin/andromeda.iso
export MEM_SIZE="64M"
export QEMU_FLAGS="-cdrom $KERN_PATH -monitor stdio -m $MEM_SIZE"

if ! command -v qemu-kvm; then                                                  
        if ! command -v kvm; then                                               
                if ! command -v qemu-system-i386; then                          
                        qemu $QEMU_FLAGS                                        
                else                                                            
                        qemu-system-i386 $QEMU_FLAGS                            
                fi;                                                             
        else                                                                    
                kvm $QEMU_FLAGS                                                 
        fi;                                                                     
else                                                                            
        qemu-kvm $QEMU_FLAGS                                                    
fi;
