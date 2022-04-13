.PHONY:all
export SOC


SOC := hi3518ev300

all:prepare ddrinit rsa2048pem_gen rsa4096pem_gen u-boot-rsa.bin aeskey2reg
#all:prepare rsa2048pem_gen rsa4096pem_gen u-boot-rsa.bin aeskey2reg
prepare:
	gcc hash_modify.c -o HASH 
	gcc aeskey2reg.c -o AES 
ddrinit:
	pushd ddr_init;make;./mkddrinit.sh u-boot-original.bin;popd
	cp ./ddr_init/ddr_init_reg_info.bin ./

rsa2048pem_gen: prepare ddrinit
#rsa2048pem_gen: prepare 
	./rsa2048pem.sh

rsa4096pem_gen: prepare ddrinit
#rsa4096pem_gen: prepare
	./rsa4096pem.sh

u-boot-rsa.bin: prepare ddrinit rsa2048pem_gen rsa4096pem_gen
#u-boot-rsa.bin: prepare rsa2048pem_gen rsa4096pem_gen
	./create_secure_boot.sh

#aeskey2reg:
	./AES ./create_secure_boot.sh > aes_otp_cfg.txt
clean:
	pushd ddr_init;make clean;popd
	find . -name "*.bin" -exec rm -rf {} \;
distclean:clean
	rm rsa2048pem/*
	rm rsa4096pem/*
	rm -rf aes_otp_cfg.txt
