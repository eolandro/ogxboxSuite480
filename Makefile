XBE_TITLE = suite480p
GEN_XISO = $(XBE_TITLE).iso
SRCS = $(CURDIR)/main.c
NXDK_DIR = $(CURDIR)/../..
NXDK_SDL = y
include $(NXDK_DIR)/Makefile

TARGET += $(OUTPUT_DIR)/title.bmp
$(GEN_XISO): $(OUTPUT_DIR)/title.bmp
$(OUTPUT_DIR)/title.bmp: $(CURDIR)/title.bmp $(OUTPUT_DIR)
	$(VE)cp '$<' '$@'
	
TARGET += $(OUTPUT_DIR)/grid.bmp
$(GEN_XISO): $(OUTPUT_DIR)/grid.bmp
$(OUTPUT_DIR)/grid.bmp: $(CURDIR)/grid.bmp $(OUTPUT_DIR)
	$(VE)cp '$<' '$@'
	
TARGET += $(OUTPUT_DIR)/color.bmp
$(GEN_XISO): $(OUTPUT_DIR)/color.bmp
$(OUTPUT_DIR)/color.bmp: $(CURDIR)/color.bmp $(OUTPUT_DIR)
	$(VE)cp '$<' '$@'
	
TARGET += $(OUTPUT_DIR)/601701cb.bmp
$(GEN_XISO): $(OUTPUT_DIR)/601701cb.bmp
$(OUTPUT_DIR)/601701cb.bmp: $(CURDIR)/601701cb.bmp $(OUTPUT_DIR)
	$(VE)cp '$<' '$@'
	
TARGET += $(OUTPUT_DIR)/DejaVuSerif.ttf
$(GEN_XISO): $(OUTPUT_DIR)/DejaVuSerif.ttf
$(OUTPUT_DIR)/DejaVuSerif.ttf: $(CURDIR)/DejaVuSerif.ttf $(OUTPUT_DIR)
	$(VE)cp '$<' '$@'
