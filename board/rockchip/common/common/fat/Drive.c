/********************************************************************************
*********************************************************************************
			COPYRIGHT (c)   2004 BY ROCK-CHIP FUZHOU
				--  ALL RIGHTS RESERVED  --

File Name:	    fat.C
Author:		    XUESHAN LIN
Created:		1st JUL 2007
Modified:
Revision:		1.00
********************************************************************************
********************************************************************************/
#define	    IN_DRIVE
#include    "../../armlinux/config.h"
#include    "fatInclude.h"		//FATͷ�ļ�

/*********************************************************************************************************
��������:����������
��ڲ���:�̷�
���ڲ���:��������Ϣ
���ú���:
˵    ��:ϵͳ��ΪDISK_SYS, FLASH��ΪDISK_FLASH, SD����ΪDISK_SD
***************************************************************************/
pDRIVE_INFO Mount(uint8 Drive)
{
    pDRIVE_INFO Rt;
    Rt = NULL;
    switch (Drive)
    {
        case DISK_SYS:
            Rt = DriveInfo+Drive;
            if (Rt->Valid == 0)
            {
                DriveInfo[DISK_SYS].Valid=0;
                //DriveInfo[DISK_FLASH].Valid=0;
            }
            Rt = DriveInfo+Drive;
            if (Rt->Valid == 0)
            {
                Rt->Valid=1;
                Rt->DirDeep=0;      //��ʼĿ¼���Ϊ0����Ŀ¼
                Rt->FreeClus=-1;    //��ÿ�ι��غ�������¼���ʣ������
                Rt->FdtRef.DirClus=-1;
                if (OK != CheckFileSystem(Drive))
                {
                    //StorageInit();//zyf ����
                    //if (OK != CheckFileSystem(Drive))
                    {
                        Rt = NULL;
                    }
                }
            }
            break;
        default:
            break;
    }
    return ((pDRIVE_INFO)Rt);
}


/*********************************************************************************************************
��������:ж��������
��ڲ���:�̷�
���ڲ���:��
���ú���:
˵    ��:ϵͳ��ΪDISK_SYS, FLASH��ΪDISK_FLASH, SD����ΪDISK_SD
***************************************************************************/
void Demount(uint8 Drive)
{
    if (Drive < MAX_DRIVE)
        DriveInfo[Drive].Valid=0;
}


/*********************************************************************************************************
��������:��ȡ��������Ϣ
��ڲ���:�̷�
���ڲ���:��������Ϣ
���ú���:
˵    ��:ϵͳ��FLASHΪ'C'
***************************************************************************/
pDRIVE_INFO GetDriveInfo(uint8 Drive)
{
    pDRIVE_INFO Rt;
    
    Rt = NULL;
    if (Drive < MAX_DRIVE)
    {
        if (DriveInfo[Drive].Valid != 0x00) //δʹ�õ�������
        {
            Rt = DriveInfo+Drive;
        }
    }
    return (Rt);
}


/*********************************************************************************************************
��������:��ȡ������
��ڲ���:Path
���ڲ���:�̷�
���ú���:
˵    ��:��һ���̷�Ϊ'C'
***************************************************************************/
uint8 GetDrive(char *Path)
{
    uint8 Drive;

	Drive=CurDrive;
	if (Path != NULL)
	{
    	if (Path[1] == ':')
    	{
            if (Path[0]>='c' && Path[0]<='z')
                Drive=Path[0]-'c';
            else if (Path[0]>='C' && Path[0]<='Z')
                Drive=Path[0]-'C';
            CurDrive = Drive;
    	}
	}
	return (Drive);
}


/*********************************************************************************************************
��������:��ȡ����������
��ڲ���:Drive
���ڲ���:�̷�
���ú���:
˵    ��:
***************************************************************************/
uint32 GetCapacity(uint8 Drive)
{
    return (256*2048);
    //return (FtlGetCapacity(Drive));
}


