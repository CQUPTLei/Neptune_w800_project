sinclude $(TOPDIR)/config.mk

ifdef CONFIG_AUTO_OTA_UPDATE
obj-y += ota_update/ota_local_info.o
obj-y += ota_update/ota_package_info.o
obj-y += ota_update/ota_update.o
obj-y += test/ota_test.o
obj-y += verify/update_hash.o
obj-y += verify/update_public_key.o
obj-y += verify/update_rsa.o
obj-y += verify/verify_sign.o
endif

ifdef CONFIG_AUTO_UPDATE_ADAPTATION
obj-y += auto_update_adaptation.o
else
obj-y += auto_update.o
endif

