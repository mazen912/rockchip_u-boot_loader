#ifndef _AFPTOOL_H_
#define _AFPTOOL_H_

#ifndef PART_NAME
	#define PART_NAME				32
#endif

#define RELATIVE_PATH				64

#ifndef MAX_PARTS
	#define MAX_PARTS				20
#endif

#ifndef MAX_MACHINE_MODEL
	#define MAX_MACHINE_MODEL		64
#endif

#ifndef MAX_MANUFACTURER
	#define MAX_MANUFACTURER		64
#endif

#define MAX_PACKAGE_FILES			16

typedef struct tagRKIMAGE_ITEM
{
	char name[PART_NAME];// ��������
	char file[RELATIVE_PATH];// ���·��������ȡ�ļ�ʱ�õ�
	unsigned int offset;// �ļ���Image�е�ƫ��
	unsigned int flash_offset;// ��д��Flash�е�λ��(��sectorΪ��λ)
	unsigned int usespace;// �ļ�ռ�ÿռ䣨��PAGE����)
	unsigned int size;// �ֽ�����ʵ���ļ���С
}RKIMAGE_ITEM;

typedef struct tagRKIMAGE_HDR
{
	unsigned int tag;
	unsigned int size;// �ļ���С������ĩβ��CRCУ����
	char machine_model[MAX_MACHINE_MODEL];
	char manufacturer[MAX_MANUFACTURER];
	int item_count;
	RKIMAGE_ITEM item[MAX_PACKAGE_FILES];
}RKIMAGE_HDR;

#define RKIMAGE_TAG				0x46414B52

#define PAGESIZE				2048

#define BOOTLOADER				"Rock28Boot(L).bin"
#define PARTNAME_BOOTLOADER		"bootloader"

#endif
