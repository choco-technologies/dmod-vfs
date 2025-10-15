# ##############################################################################
#  Makefile for the dmod_vfs module
#
#     This Makefile is used to build the dmod_vfs module library
#
#   DONT EDIT THIS FILE - it is automatically generated. 
#   Edit the scripts/Makefile-lib.in file instead.
#       
# ##############################################################################
ifeq ($(DMOD_DIR),)
    DMOD_DIR = build/_deps/dmod-src
endif

#
#  Name of the module
#
DMOD_LIB_NAME=libdmod_vfs.a
DMOD_SOURCES=src/dmod_vfs.c
DMOD_INC_DIRS = /home/pkubiak/Projects/choco-technologies/public/dmod-vfs\
		/home/pkubiak/Projects/choco-technologies/public/dmod-vfs/inc
DMOD_LIBS = dmod
DMOD_GEN_HEADERS_IN = 
DMOD_DEFINITIONS = 

# -----------------------------------------------------------------------------
# 	Initialization of paths
# -----------------------------------------------------------------------------
include $(DMOD_DIR)/paths.mk

# -----------------------------------------------------------------------------
# 	Including the template for the library
# -----------------------------------------------------------------------------
DMOD_LIB_OBJS_DIR = $(DMOD_OBJS_DIR)/dmod_vfs
include $(DMOD_SLIB_FILE_PATH)
