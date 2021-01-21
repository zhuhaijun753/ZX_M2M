#****************************************************************
# @FileName: Makefile
# @Engineer: TenYan
# @version   V1.0
# @Date:     2020-12-24
# ���ڱ��뵥�ļ���,���ļ�
# $@ ��ʾ�����е�Ŀ���ļ���
# $^ ��ʾ���е�����Ŀ��ļ���
# $< ��ʾ����Ŀ���еĵ�һ��Ŀ������
#***************************************************************
#���빤����
CROSS-COMPILE:=arm-none-linux-gnueabi-
CC = $(CROSS-COMPILE)gcc
LD = $(CROSS-COMPILE)ld

#����ѡ��
CFLAGS = -s -Wall -I${DIR_INC}
LDFLAGS = -ldl -lm -lpthread

#��������ļ���
TARGET = $(DIR_BIN)/helloworld

#�����Ŀ¼
DIR_INC = ./inc
DIR_OBJ = ./obj
DIR_BIN = ./bin
DIR_SRC = ./src

SRC = $(wildcard ${DIR_SRC}/*.c)
DIR = $(notdir $(SRC))

#�����е�.c�滻Ϊ.o
OBJ = $(patsubst %.c,${DIR_OBJ}/%.o,$(notdir ${SRC}))

all: $(TARGET)

#����.o�Ĺ���
$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

${DIR_OBJ}/%.o: ${DIR_SRC}/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -rf $(OBJ) $(TARGET)
