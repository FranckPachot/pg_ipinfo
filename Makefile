MODULES   = ipinfo             #list of shared-library objects
EXTENSION = ipinfo             #name of extension ($EXTENSION.control file)
DATA      = ipinfo--1.0.0.sql  ipinfo--1.0.0--1.0.1.sql


PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
override CFLAGS += -std=c99 # avoid `-Wdeclaration-after-statement` warnings
include $(PGXS)
