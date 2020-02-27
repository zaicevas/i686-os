if grub-file --is-x86-multiboot2 ../bin/kernel.elf; then
  echo multiboot2 confirmed
else
  echo kernel.elf is not multiboot2 compliant
fi
