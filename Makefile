all:
	cd src && make
test:
	bash automation/test-launcher.sh
