#source file
SRC = data_access.cpp mysql_test.cpp access_attr.cpp

#Application name
APP = data_test 

#Application type BIN SO LIB
APP_TYPE = BIN

#Include
INCLUDE = -I/usr/include/mysql  -L/usr/local/lib64

#Link lib 
LINK_LIB = -L/usr/lib/x86_64-linux-gnu  -lmysqlclient_r -lpthread -lrt -lm -ldl 

#Install Path
INSTALL_PATH = 

#Bin Path
BIN_PATH = bin

#Obj path
OBJ_PATH = obj

#Compiler
CXX = g++

#Options
CFLAGS =  -g


#Obj and bin directory Judge
ifeq ($(strip $(BIN_PATH)),)
	BIN_PATH = .
endif

ifeq ($(strip $(OBJ_PATH)),)
	OBJ_PATH = .
endif

ifeq ($(strip $(INSTALL_PATH)),)
	INSTALL_PATH = .
endif

#Dest Splice
ifeq "$(strip $(APP_TYPE))" "SO"
	DEST=$(BIN_PATH)/lib$(APP).so
	LINKER = $(CXX)  $(CFLAGS)  -o $(DEST)
	CFLAGS+= -fPIC -shared
else
	ifeq "$(strip $(APP_TYPE))" "LIB"
		DEST = $(BIN_PATH)/lib$(APP).a
		LINKER = ar crs $(DEST)
	else
		ifeq "$(strip $(APP_TYPE))" "BIN"
			DEST = $(BIN_PATH)/$(APP)
			LINKER = $(CXX)  -o $(DEST) 
		endif
	endif
endif


VPATH += $(foreach n,$(SRC),$(dir $(n)):)
						
VPATH += $(BIN_PATH):$(OBJ_PATH)

#Object file
OBJ = $(addprefix $(OBJ_PATH)/, $(notdir $(SRC:.cpp=.o)))

#Deps file
DEPS = $(addprefix $(OBJ_PATH)/, $(notdir $(SRC:.cpp=.d)))


.PHONY:clean

install_all : all
	@if [ -n "${INSTALL_PATH}" ];then \
		mkdir -p ${INSTALL_PATH} && \
		cp -rf ${DEST}  ${INSTALL_PATH}/; \
	fi


all:init  $(DEST) 

init:
	@mkdir -p $(BIN_PATH)
	@mkdir -p $(OBJ_PATH)

$(DEST):$(OBJ)
	$(LINKER) $(CFLAGS)  $^ $(LINK_LIB)

$(OBJ):$(OBJ_PATH)/%.o:%.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) $< -c -o  $@ 											

$(DEPS):$(OBJ_PATH)/%.d:%.cpp
	@mkdir -p $(BIN_PATH);\
	mkdir -p $(OBJ_PATH);\
	set -e;rm -f  $@;\
	$(CXX) $(CFLAGS) -MM $(INCLUDE) $< > $@.$$$$;\
	sed 's, \($*\)\.o[ :]*,\1.o $@ : , g'< $@.$$$$ > $@;\
	rm -f $@.$$$$	   

include $(DEPS)

clean:
	-rm -f $(OBJ_PATH)/*.o  $(OBJ_PATH)/*.d $(DEST) \
	${INSTALL_PATH}/$(notdir $(DEST))
