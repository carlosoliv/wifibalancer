all: station controller

station:
	gcc station-manager.c -o station-manager
#	gcc station-notifier.c -o station-notifier
controller:
	gcc controller-manager.c -o controller-manager
	gcc controller-database.c -o controller-database
