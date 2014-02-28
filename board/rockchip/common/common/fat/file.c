/********************************************************************************
*********************************************************************************
			COPYRIGHT (c)   2004 BY ROCK-CHIP FUZHOU
				--  ALL RIGHTS RESERVED  --

File Name:	    File.C
Author:		    XUESHAN LIN
Created:		1st JUL 2007
Modified:
Revision:		1.00
********************************************************************************
********************************************************************************/
#define     IN_FILE
#include    "../../armlinux/config.h"
#include    "fatInclude.h"		//FATͷ�ļ�


/*********************************************************************************************************
** ��������	:FileInit
** ��������	:��ʼ���ļ�ϵͳ
** �䡡��	:��
** �䡡��	:��
** ȫ�ֱ���	:��
** ����ģ��	:��
********************************************************************************************************/
void FileInit(void)
{
	uint16 i;

    CurDrive=0;     //Ĭ��ϵͳ��
	for (i=0; i<MAX_OPEN_FILES; i++)
	{
		FileInfo[i].Flags = 0;
	}
	CacheInit();

    //3���̳�ʼ��
	for (i=0; i<MAX_DRIVE; i++)
	{
		Demount(i);
		Mount(i);
	}
}


/*********************************************************************************************************
** ��������	:FileOpen
** ��������	:��ָ����ʽ���ļ�
** �䡡��	:DirClus:·����Ŀ¼��, DirFileName:�û�ʹ�õ��ļ���, Type:�򿪷�ʽ
** �䡡��	:Not_Open_FILEΪ���ܴ�,����Ϊ���ļ��ľ��
** ȫ�ֱ���: 
** ����ģ��: ��
********************************************************************************************************/
pFILE FileOpen(uint8 Drive, uint32 DirClus, char *DirFileName, char *Type)
{
	uint8 i;
	pFILE fp, Rt;
	FDT FileFDT;
    
	Rt=NULL;
#ifdef OS_FILE
	OBTAIN_FAT_SEM;
#endif
	StrUprCase(Type);
	StrUprCase(DirFileName);
	/* ���ҿ����ļ��Ǽ��� */
	for (i=0; i<MAX_OPEN_FILES; i++)
	{
		if (FileInfo[i].Flags == 0)
		{
			break;
		}
	}
    
	if (i < MAX_OPEN_FILES && DirClus < BAD_CLUS)
	{
		fp = &FileInfo[i];
		fp->DirClus=DirClus;
        fp->Drive=Drive;

		if (RETURN_OK==FindFDTInfo(&FileFDT, fp->Drive, fp->DirClus, DirFileName))
		{
		    
			if ((FileFDT.Attr & ATTR_DIRECTORY) == 0)	//���ļ�
			{
                ftl_memcpy(fp->Name, DirFileName, 11);
				fp->FileSize = FileFDT.FileSize;
				fp->FstClus = FileFDT.FstClusLO | ((uint32)FileFDT.FstClusHI << 16);
				fp->Clus = fp->FstClus;
				fp->Offset = 0;
				fp->RefClus = fp->Clus;
				fp->RefOffset = 0;
				fp->Flags = FILE_FLAGS_READ;
                Rt=fp;
			}
		}
	}	

#ifdef OS_FILE
	RELEASE_FAT_SEM;
#endif
	return (Rt);
}


/*********************************************************************************************************
** ��������	:FileClose
** ��������	:�ر�ָ���ļ�
** �䡡��	:�ļ����
** �䡡��	:RETURN_OK:�ɹ�
** �����ο�  fat.h�й��ڷ���ֵ��˵�� 
** ȫ�ֱ���	:
** ����ģ��	:��
********************************************************************************************************/
uint8 FileClose(pFILE fp)
{
	uint8 Rt;
    
	Rt = PARAMETER_ERR;
#ifdef OS_FILE
	OBTAIN_FAT_SEM;
#endif
	if (fp >= &FileInfo[0] && fp <= &FileInfo[MAX_OPEN_FILES-1])
	{
		Rt = RETURN_OK;
#ifndef BOOT_ONLY
		if ((fp->Flags & FILE_FLAGS_WRITE) == FILE_FLAGS_WRITE)
		{
            uint8 i;
        	FDT FileFDT;
            
			FLASH_PROTECT_OFF();
			Rt = FindFDTInfo(&FileFDT, fp->Drive, fp->DirClus, fp->Name);
			if (Rt == RETURN_OK)
			{
				FileFDT.FileSize = fp->FileSize;
				if (FileFDT.FstClusLO == 0 && FileFDT.FstClusHI == 0)	//���½��ļ������
				{
					FileFDT.FstClusLO = fp->FstClus & 0xffff;
					FileFDT.FstClusHI = (fp->FstClus >> 16) & 0xffff;
				}
				ChangeFDT(fp->Drive, fp->DirClus, &FileFDT);
#if 1
            	for (i=0; i<MAX_OPEN_FILES; i++)
            	{
            		if ((FileInfo[i].Flags & FILE_FLAGS_WRITE)==FILE_FLAGS_WRITE && fp!=&FileInfo[i])
            		{
            			break;
            		}
            	}
                if (i >= MAX_OPEN_FILES)
#endif                    
                    CacheWriteBackAll();
				//CopyFat(fp->Drive, 0);
				UpdataFreeMem(fp->Drive, -fp->FileSize);
                #ifdef IN_SYSTEM				
				ClearEncInfo();
                #endif
			}
		}
#endif
		fp->Flags = 0;
	}
#ifdef OS_FILE
	RELEASE_FAT_SEM;
#endif
	return (Rt);
}


/*********************************************************************************************************
** ��������	:FileRead
** ��������	:��ȡ�ļ�
** �䡡��	:Buf:������ص�����ָ��
**        	 Size:1=byte, 2=halfword, 4=word
             count:��ȡ�ĸ���
			 fpָ�����ļ�����
** �䡡��	:ʵ�ʶ������ֽ���
** ȫ�ֱ���	:
** ����ģ��: ��
********************************************************************************************************/
uint32 FileRead(void *Buf, uint8 Size, uint32 count, pFILE fp)
{
	uint32 SecIndex;
	uint16 offsetInSec,offsetInClu;
	uint32 len;
	uint32 cnt;
    pDRIVE_INFO pInfo;
    uint8 tmp[512];
    uint8 *pBuf=Buf;
    uint16 BytePerClus;
    uint32 ReadContLba;
    uint16 ReadContSectors=0;
    uint8 *ReadContPtr;
    PRINT_I("FileRead offset=%x,Buf = %x count = %x\n" ,fp->Offset, Buf,count);
#ifdef OS_FILE
	OBTAIN_FAT_SEM;
#endif
	if (fp >= &FileInfo[0] && fp <= &FileInfo[MAX_OPEN_FILES-1] && fp->Flags != 0)
	{
        pInfo=GetDriveInfo(fp->Drive);
        count *= Size;
		if ((count + fp->Offset) > fp->FileSize)
			count = fp->FileSize  - fp->Offset;
		cnt=count;
        #if 0
		while (cnt > 0)	//���ļ�����
		{
			if (fp->Clus >= BAD_CLUS)		//���ӳ����ж�
				break;
		
			offsetInSec=fp->Offset &  (pInfo->BytsPerSec-1);
			offsetInClu=fp->Offset & ((1 << (pInfo->LogSecPerClus+9)) - 1);
			SecIndex=((fp->Clus - 2) << pInfo->LogSecPerClus) + pInfo->FirstDataSector+(offsetInClu >> 9);
			len=512-offsetInSec;	//������PAGE����
            if (OK != ReadSecs(fp->Drive, SecIndex, tmp, (len+511)>>9))
            {
                cnt=count;
                break;
            }
			if (len > cnt)
			{
				len=cnt;
			}
            ftl_memcpy(pBuf, tmp+offsetInSec, len);
			pBuf += len;

			cnt -= len;
			if ((offsetInClu+len) >= (1 << (pInfo->LogSecPerClus+9)))
			{
				SecIndex=FATGetNextClus(fp->Drive, fp->Clus, 1);
				if (SecIndex == BAD_CLUS)
				{
		            cnt=count;
                    break;
				}
				else
					fp->Clus = SecIndex;
			}

			fp->Offset += len;
			if (fp->Offset >= fp->FileSize)		//���ļ�����
			{
				cnt=fp->Offset-fp->FileSize;
				fp->Offset=fp->FileSize;
				break;
			}
		}
        #else
        BytePerClus = (1 << (pInfo->LogSecPerClus+9));
        while (cnt > 0)	//���ļ�����
		{
			if (fp->Clus >= BAD_CLUS)		//���ӳ����ж�
				break;

            offsetInSec=fp->Offset &  (pInfo->BytsPerSec-1);
            offsetInClu=fp->Offset & (BytePerClus - 1);
			SecIndex=((fp->Clus - 2) << pInfo->LogSecPerClus) + pInfo->FirstDataSector+(offsetInClu >> 9);

            if(offsetInSec || cnt < BytePerClus || offsetInClu)
            //if(offsetInSec || cnt < BytePerClus)
            {
                len=512-offsetInSec;	//������PAGE����
                if (OK != ReadSecs(fp->Drive, SecIndex, tmp, (len+511)>>9))
                {
                    cnt=count;
                    break;
                }
    			if (len > cnt)
    			{
    				len=cnt;
    			}
                ftl_memcpy(pBuf, tmp+offsetInSec, len);
                pBuf += len;
                cnt -= len;
                fp->Offset += len;
                
                if ((offsetInClu+len) >= BytePerClus)
    			{
    				SecIndex=FATGetNextClus(fp->Drive, fp->Clus, 1);
    				if (SecIndex == BAD_CLUS)
    				{
    		            cnt=count;
                        break;
    				}
    				else
    					fp->Clus = SecIndex;
    			}
                SecIndex = ((fp->Clus - 2) << pInfo->LogSecPerClus) + pInfo->FirstDataSector+(offsetInClu >> 9);
            }
            else
            {
                while(1)
                {
                    len = BytePerClus;
                    if(ReadContSectors == 0)
                    {
                        ReadContLba = SecIndex;
                        ReadContPtr = pBuf;
                    }
                    ReadContSectors += (len>>9);
                    pBuf += len;
                    cnt -= len;
                    if ((offsetInClu+len) >= BytePerClus)
        			{
        				SecIndex=FATGetNextClus(fp->Drive, fp->Clus, 1);
        				if (SecIndex == BAD_CLUS)
        				{
        		            cnt=count;
                            break;
        				}
        				else
        					fp->Clus = SecIndex;
        			}
                    
                    fp->Offset += len;
                    
                    SecIndex = ((fp->Clus - 2) << pInfo->LogSecPerClus) + pInfo->FirstDataSector+(offsetInClu >> 9);

                    if(cnt < BytePerClus || SecIndex != ReadContLba + ReadContSectors)
                    {
                        break;
                    }
                }

                if (OK != ReadSecs(fp->Drive, ReadContLba, ReadContPtr, ReadContSectors))
                {
                    cnt=count;
                    break;
                }
                ReadContSectors = 0;
            }
            
			if (fp->Offset >= fp->FileSize)		//���ļ�����
			{
				cnt=fp->Offset-fp->FileSize;
				fp->Offset=fp->FileSize;
				break;
			}
		}
        #endif
	}

#ifdef OS_FILE
	RELEASE_FAT_SEM;
#endif
    return (count-cnt);
}


/*********************************************************************************************************
** ��������	:FileEof
** ��������	:�ж��ļ��Ƿ񵽶�\д���ļ�β
** �䡡��	:��
** �䡡��	:0:��,1:��
** ȫ�ֱ���: 
** ����ģ��: ��
********************************************************************************************************/
bool FileEof(pFILE fp)
{
	bool Rt;

#ifdef OS_FILE
	OBTAIN_FAT_SEM;
#endif
	Rt=TRUE;
	if (fp >= &FileInfo[0] && fp <= &FileInfo[MAX_OPEN_FILES-1])
	{
		if (fp->Offset < fp->FileSize)
			Rt=FALSE;
	}
#ifdef OS_FILE
	RELEASE_FAT_SEM;
#endif
	return (Rt);
}


/*********************************************************************************************************
** ��������	:FileSeek
** ��������	:�ƶ��ļ���\дλ��
** �䡡��	:offset:�ƶ�ƫ����
**        	 Whence:�ƶ�ģʽ
				 SEEK_SET:���ļ�ͷ����(����ƫ�ƶ�Ϊ+)
				 SEEK_CUR:�ӵ�ǰλ�ü���(��ǰΪ-, ����Ϊ+)
				 SEEK_END:���ļ�β����(��ǰ��Ϊ+, ����Ϊ-)
** �䡡��	:��
** ȫ�ֱ���	:
** ����ģ��	:��
********************************************************************************************************/
uint8 FileSeek(pFILE fp, int32 offset, uint8 Whence)
{
	uint8 Rt;
	uint32 OldClusCnt;
	uint32 NewClusCnt;
	uint32 Clus;
    pDRIVE_INFO pInfo;
    
	Rt = PARAMETER_ERR;
#ifdef OS_FILE
	OBTAIN_FAT_SEM;
#endif
	if (fp >= &FileInfo[0] && fp <= &FileInfo[MAX_OPEN_FILES-1])
	{
        pInfo=GetDriveInfo(fp->Drive);
		if (fp->Flags  != 0)                            		// ��Ӧ���ļ��Ƿ��Ѵ�
		{
			Rt = RETURN_OK;
			OldClusCnt = fp->Offset >> (pInfo->LogSecPerClus+9);
			switch (Whence)
			{
				case SEEK_END:					    /* ���ļ�β���� */
					fp->Offset = fp->FileSize - offset;
					offset = -offset;
					break;
				case SEEK_SET:
					fp->Offset = offset;			/* ���ļ�ͷ���� */
					break;
				case SEEK_CUR:                      /* �ӵ�ǰλ�ü��� */
					fp->Offset += offset;
					break;
				case SEEK_REF:                      /* �Ӳο�λ�ü��� */
					fp->Offset += offset;
					if (fp->Offset >= fp->RefOffset)
					{
						OldClusCnt = fp->RefOffset >> (pInfo->LogSecPerClus+9);
						fp->Clus = fp->RefClus;
					}
					break;
				default:
					Rt = PARAMETER_ERR;
					break;
			}
			if (Rt == RETURN_OK)
			{
				if (fp->Offset > fp->FileSize)
				{
					if (offset > 0)
						fp->Offset = fp->FileSize;
					else
						fp->Offset = 0;
				}
				/* �ı䵱ǰ�غ� */
				NewClusCnt = fp->Offset >> (pInfo->LogSecPerClus+9);
				if (NewClusCnt >= OldClusCnt)
				{
					NewClusCnt -= OldClusCnt;
					Clus = fp->Clus;
				}
				else
				{
					Clus = fp->FstClus;
				}
                OldClusCnt=FATGetNextClus(fp->Drive, Clus, NewClusCnt);
				if (OldClusCnt == BAD_CLUS)
					Rt=FAT_ERR;				
				else
					fp->Clus=OldClusCnt;
			}
		}
	}
#ifdef OS_FILE
	RELEASE_FAT_SEM;
#endif
	return (Rt);
}


/*********************************************************************************************************
** ��������	:FileExtNameMatch
** ��������	:�ļ���չ������
** �䡡��	:SrcExtNameԴ��չ��,Filter������
** �䡡��	:TRUEƥ��,FALSE��ƥ��
** ȫ�ֱ���	:��
** ����ģ��	:��
********************************************************************************************************/
bool FileExtNameMatch(char *SrcExtName, char *Filter)
{
	if (*Filter == '*')
		return (TRUE);

	StrUprCase(Filter);
	while (*Filter != '\0')
	{
		if (SrcExtName[0]==Filter[0] || Filter[0]=='?')
		{
			if (SrcExtName[1]==Filter[1] || Filter[1]=='?')
			{
				if (SrcExtName[2]==Filter[2] || Filter[2]=='?')
				{
					return (TRUE);
				}
			}
		}
		Filter += 3;
	}
	return (FALSE);
}



/*********************************************************************************************************
** ��������	:StrUprCase
** ��������	:Сд��ĸת��д��ĸ 
** �䡡��	:��Сд��ĸ��ϵ��ļ���
** �䡡��	:��д��ĸ�ļ���
** ȫ�ֱ���	:��
** ����ģ��	:��
********************************************************************************************************/
void StrUprCase(char *name)
{
#if 0
	while (*name != '\0')
	{
		if (*name >= 'a' && *name <= 'z')
			*name=*name-'a'+'A';
		name++;
	}
#endif	
}


/*********************************************************************************************************
** ��������	:FileOpenExt
** ��������	:��ָ����ʽ���ļ�
** �䡡��	:Path:·��, DirFileName:�û�ʹ�õ��ļ���, Type:�򿪷�ʽ
** �䡡��	:Not_Open_FILEΪ���ܴ�,����Ϊ���ļ��ľ��
** ȫ�ֱ���: 
** ����ģ��: ��
********************************************************************************************************/
pFILE FileOpenExt(char *Path, char *DirFileName, char *Type)
{
    uint8 Drive;
    uint32 DirClus;
    
    //if (Type[0]=='W' || Type[0]=='w')
    //    return (FileCreate(Path, DirFileName));
    Drive=GetDrive(Path);
	DirClus=GetDirClusIndex(Path);
    return (FileOpen(Drive, DirClus, DirFileName, Type));
}



#ifndef BOOT_ONLY
/*********************************************************************************************************
** �������� :FindFile
** �������� :���ҵ�ǰĿ¼/ȫ��Ŀ¼��ָ�����͵ĵڼ����ļ�
**           FileNum:�ļ���,ExtName:��չ��, Attr:����(ATTR_DIRECTORYĿ¼, ATTR_VOLUME_ID���, 0�ļ�)
** �䡡��   :Rt:�ҵ����ļ�Ŀ¼����Ϣ
** ȫ�ֱ��� :
** ����ģ�� :��
** ˵  ��   :����չ��ָ��Ϊ"*"ʱ, Ҳ����ҵ�Ŀ¼
********************************************************************************************************/
uint8 FindFileSub(FDT *Rt, FIND_DATA* FindData, uint16 FileNum, char *ExtName, uint8 Attr)
{
    uint16 i, num;
    uint8 offset;
    LONG_FDT *LongFdt;
    uint32 index;
    uint16 Items;
    FDT tmp;

    index=FindData->Index;
    num=0;
    while (1)
    {
        if (RETURN_OK!=GetFDTInfo(Rt, FindData->Drive, FindData->Clus, index++))
            break;
        
        if (Rt->Name[0]==FILE_NOT_EXIST)                //��Ŀ¼��,���治�����ļ�
            break;
        
        if (Rt->Name[0]!=FILE_DELETED)
        {
            Items=0;
            while (Rt->Attr==ATTR_LFN_ENTRY)            //���ļ�����Ҫ�ҵ����ļ���
            {
                GetFDTInfo(Rt, FindData->Drive, FindData->Clus, index++);
                Items++;    
            }

            //if ((Rt->Attr & (ATTR_DIRECTORY | ATTR_VOLUME_ID)) == Attr)
            if ((Rt->Attr & Attr) == Attr)  
            {
                if (FileExtNameMatch(&Rt->Name[8], ExtName))
                {
                    if (++num == FileNum)
                    {
                        FindData->Index=index;
                        if (Items != 0) //��Ŀ¼��
                        {
                            index -= Items;
                            memset(LongFileName, '\0', MAX_FILENAME_LEN);
                            while (1)
                            {
                                GetFDTInfo(&tmp, FindData->Drive, FindData->Clus, index+Items-2);
                                LongFdt=(LONG_FDT*)&tmp;
                                offset=13 * ((LongFdt->Order & LFN_SEQ_MASK) - 1);
                                if ((LongFdt->Order & LFN_SEQ_MASK) <= MAX_LFN_ENTRIES)   
                                {/* ���ļ������Ŀ¼����*/
                                    for (i = 0; i < 5; i++)
                                        LongFileName[i+offset]=(LongFdt->Name1[i*2+1]<<8) | LongFdt->Name1[i*2];
                                    for (i = 0; i < 6; i++)
                                        LongFileName[i+5+offset]=(LongFdt->Name2[i*2+1]<<8) | LongFdt->Name2[i*2];
                                    for (i = 0; i < 2; i++)
                                        LongFileName[i+11+offset]=(LongFdt->Name3[i*2+1]<<8) | LongFdt->Name3[i*2];
                                }
                                if (--Items == 0)
                                    break;
                            }
                        }
                        else        //��Ŀ¼��
                        {
                            for (i=0; i<8; i++)
                            {
                                LongFileName[i] = Rt->Name[i];
                                if (LongFileName[i] == ' ')
                                    break;
                            }
                            if (Rt->Name[8] != ' ')
                            {
                                LongFileName[i++] = '.';                //׷����չ��
                                LongFileName[i++] = Rt->Name[8];
                                LongFileName[i++] = Rt->Name[9];
                                LongFileName[i++] = Rt->Name[10];
                            }
                            LongFileName[i] = '\0';                 //������־��unicode���NUL
                        }
                        return (RETURN_OK);
                    }
                }
            }
        }
    }
    return (NOT_FIND_FILE);
}



/*********************************************************************************************************
** �������� :FindFirst
** �������� :����ָ��Ŀ¼�ĵ�һ���ļ�
**           FindData:�ļ����ҽṹ,Path:·��, ExtName:��չ��
** �䡡��   :Rt:�ҵ����ļ�Ŀ¼����Ϣ
** ȫ�ֱ��� :
** ����ģ�� :��
** ˵  ��   :����չ��ָ��Ϊ"*"ʱ, Ҳ����ҵ�Ŀ¼
********************************************************************************************************/
uint8 FindFirst(FDT *Rt, FIND_DATA* FindData, char *Path, char *ExtName)
{
    FindData->Clus=GetDirClusIndex(Path);
    FindData->Drive=GetDrive(Path);
    FindData->Index=0;
    return (FindNext(Rt, FindData, ExtName));
}



/*********************************************************************************************************
** �������� :FindNext
** �������� :����ָ��Ŀ¼����һ���ļ�
**           FindData:�ļ����ҽṹ,ExtName:��չ��
** �䡡��   :Rt:�ҵ����ļ�Ŀ¼����Ϣ
** ȫ�ֱ��� :
** ����ģ�� :��
** ˵  ��   :����չ��ָ��Ϊ"*"ʱ, Ҳ����ҵ�Ŀ¼
********************************************************************************************************/
uint8 FindNext(FDT *Rt, FIND_DATA* FindData, char *ExtName)
{
    uint8 OsRt;

#ifdef OS_FILE
    OBTAIN_FAT_SEM;
#endif

    OsRt=PATH_NOT_FIND;
    if (FindData->Clus < BAD_CLUS)
    {
        OsRt=FindFileSub(Rt, FindData, 1, ExtName, 0);
    }

#ifdef OS_FILE
    RELEASE_FAT_SEM;
#endif
    return (OsRt);
}


/*********************************************************************************************************
** ��������	:FileModify
** ��������	:�޸��ļ�
** �䡡��	:buf:Ҫд������ָ��
**  		 Bytes:Ҫд���ֽ���С�ڵ���64KB
			 Handle:ָ�����ļ����
			 offset:�ļ�ƫ����
** �䡡��	:�����
** ȫ�ֱ���	:
** ����ģ��	:��
********************************************************************************************************/
uint8 FileModify(void* buf, uint32 offset, uint32 Bytes, pFILE fp)
{
	uint32 LBA;
	uint16 SecOffset;
	uint16 ClusOffset;
	uint8 Tmp[512];
	uint16 count;
	uint8 Rt;
    uint8 *pBuf=buf;
    pDRIVE_INFO pInfo;

	Rt = PARAMETER_ERR;
	if (fp >= &FileInfo[0] && fp <= &FileInfo[MAX_OPEN_FILES-1])
	{
		Rt = RETURN_OK;
		fp->Offset = offset;
        pInfo=GetDriveInfo(fp->Drive);
		while (Bytes > 0)
		{
			SecOffset = fp->Offset & 0x1ff;
			ClusOffset = fp->Offset & ((1 << (pInfo->LogSecPerClus+9)) - 1);
			LBA = ((fp->Clus - 2) << pInfo->LogSecPerClus) + pInfo->FirstDataSector+(ClusOffset >> 9);
			count=512-SecOffset;
            if (count > Bytes)
                count=Bytes;
            ftl_memcpy(&Tmp[SecOffset], pBuf, count);
            pBuf += count;
			Bytes -= count;
			fp->Offset += count;
			if (fp->Offset > fp->FileSize)
			{
				Rt = FILE_EOF;
				break;
			}
            WriteSecs(fp->Drive, LBA, Tmp, 1);
		}
	}
	return (Rt);
}


/*********************************************************************************************************
** ��������	:FileWrite
** ��������	:д�ļ�
** �䡡��	:Buf:Ҫд������ָ��
**        	 Size:1=byte, 2=halfword, 4=word
             count:д��ĸ���
			 fpָ�����ļ�����
** �䡡��	:ʵ��д���ֽ���
** ȫ�ֱ���	:
** ����ģ��	:��
********************************************************************************************************/
uint32 FileWrite(void *Buf, uint8 Size, uint32 count, pFILE fp)
{
	uint32 NewClus;
	uint16 offsetInSec,offsetInClu;
	uint32 len;
	uint32 cnt;
    uint8 *pBuf=Buf;
    pDRIVE_INFO pInfo;

    count *= Size;
	if ((count % 512) != 0)
		return (0);
#ifdef OS_FILE
	OBTAIN_FAT_SEM;
#endif
	if (fp >= &FileInfo[0] && fp <= &FileInfo[MAX_OPEN_FILES-1] && (fp->Flags & FILE_FLAGS_WRITE) != 0)
	{
        cnt=count;
        pInfo=GetDriveInfo(fp->Drive);
		FLASH_PROTECT_OFF();
		while (cnt > 0)
		{
			offsetInSec=fp->Offset &  (pInfo->BytsPerSec-1);
			offsetInClu= fp->Offset & ((1 << (pInfo->LogSecPerClus+9)) - 1);

			if (fp->Offset > fp->FileSize)	//�ļ�ָ�볬���ļ�����
				break;
			
			if (offsetInClu == 0)			//Ҫ����һ����
			{
				if (fp->Offset < fp->FileSize)
				{
					if (fp->Offset == 0)		//��д�״�
						fp->Clus = fp->FstClus;
					else
						fp->Clus = FATGetNextClus(fp->Drive, fp->Clus, 1);
				}
				else
				{
					NewClus= FATAddClus(fp->Drive, fp->Clus);
					if (NewClus >= BAD_CLUS)			//���̿ռ���
					{
						break;
					}
					fp->Clus = NewClus;
					if (fp->FstClus == EMPTY_CLUS)		//��δ�����
					{
						fp->FstClus = fp->Clus;
                        #ifdef IN_SYSTEM
						{
							RCV_INFO EncRcvInfo;
							
							EncRcvInfo.Flag=0x0f;
							EncRcvInfo.Drive=fp->Drive;
							EncRcvInfo.DirClus=fp->DirClus;
							EncRcvInfo.FstClus=fp->FstClus;
                            ftl_memcpy(EncRcvInfo.FileName, fp->Name, 11);
							SaveEncInfo(&EncRcvInfo);
							FLASH_PROTECT_OFF();
						}
                        #endif
					}
				}
			}
			fp->Sec=((fp->Clus - 2) << pInfo->LogSecPerClus)+pInfo->FirstDataSector+(offsetInClu >> 9);
			len=pInfo->BytsPerSec-offsetInSec;		//��������������
			if (len > cnt)
				len=cnt;
            if (OK != WriteSecs(fp->Drive, fp->Sec, pBuf, 1))
                break;
			cnt -= len;
			pBuf += len;
			fp->Offset += len;
			if (fp->Offset > fp->FileSize)
				fp->FileSize = fp->Offset;
		}
		FLASH_PROTECT_ON();
	}
#ifdef OS_FILE
	RELEASE_FAT_SEM;
#endif
    return (count-cnt);
}


/*********************************************************************************************************
** ��������	:FindOpenFile
** ��������	:�����Ѵ򿪵�ָ���ļ����ļ����
** �䡡��	:FileName:�ڲ��ļ���
** �䡡��	:�ļ����
** ȫ�ֱ���	:
** ����ģ��: ��
********************************************************************************************************/
uint8 FindOpenFile(uint32 DirClus, char *FileName)
{
	pFILE fp;
	uint8 i, j;
	
	fp=FileInfo;
	for (j=0; j<MAX_OPEN_FILES; j++)
	{
		if (fp->Flags != 0)
		{
    		if (fp->DirClus == DirClus)
    		{
        		for (i=0; i<11; i++)
        		{
        			if (fp->Name[i] != FileName[i])
        				break;
        		}
        		if (i == 11)
        			break;
    		}
		}
		fp++;
	}
	return (j);
}


/*********************************************************************************************************
** ��������	:
** ��������	:ɾ���ļ�
** �䡡��	:Path:·��, DirFileName:�û�ʹ�õ��ļ���
** �䡡��	:RETURN_OK���ɹ�
** �����ο�  fat.h�й��ڷ���ֵ��˵��
** ȫ�ֱ���	:
** ����ģ��	:
********************************************************************************************************/
uint8 FileDelete(char *Path, char *DirFileName)
{
	uint32 ClusIndex1;
	uint32 DirClus;
	uint8 Rt;
	FDT temp;
    uint8 Drive;

#ifdef OS_FILE
	OBTAIN_FAT_SEM;
#endif
	StrUprCase(Path);
	StrUprCase(DirFileName);
	DirClus=GetDirClusIndex(Path);	//��ȡ·�����ڵĴغ�
	Drive=GetDrive(Path);
	Rt = PATH_NOT_FIND;
	if (DirClus != BAD_CLUS)			//ȷ��·������
	{
		Rt = NOT_FIND_FILE;
		if (RETURN_OK == FindFDTInfo(&temp, Drive, DirClus, DirFileName))		//�ҵ�Ŀ¼��
		{
			if ((temp.Attr & ATTR_DIRECTORY) == 0)  		// ���ļ���ɾ��
			{
				Rt = FILE_LOCK;
				if (FindOpenFile(DirClus, DirFileName) >= MAX_OPEN_FILES)	//�ļ�û�д򿪲�ɾ��
				{
					ClusIndex1 = temp.FstClusLO + ((uint32)(temp.FstClusHI) << 16);
					FATDelClusChain(Drive, ClusIndex1);
					Rt = DelFDT(Drive, DirClus, DirFileName);
					UpdataFreeMem(Drive, temp.FileSize);
				}
			}
		}
	}
#ifdef OS_FILE
	RELEASE_FAT_SEM;
#endif
	return (Rt);
}


/*********************************************************************************************************
** ��������	:FileCreate
** ��������	:�����ļ�
** �䡡��	:Path:·��, DirFileName:�û�ʹ�õ��ļ���
** �䡡��	:RETURN_OK���ɹ�
** �����ο�  fat.h�й��ڷ���ֵ��˵��
** ȫ�ֱ���	:
** ����ģ��	:
********************************************************************************************************/
pFILE FileCreate(char *Path, char *DirFileName)
{
	pFILE fp, Rt;
	FDT temp;
	uint8 i;
    uint8 Drive;

	Rt=NULL;
#ifdef OS_FILE
	OBTAIN_FAT_SEM;
#endif
	StrUprCase(Path);
	StrUprCase(DirFileName);
    Drive=GetDrive(Path);
	/* ���ҿ����ļ��Ǽ��� */
	for (i=0; i<MAX_OPEN_FILES; i++)
	{
		if (FileInfo[i].Flags == 0)
		{
			break;
		}
	}
    
	if (i < MAX_OPEN_FILES)
	{
		fp = &FileInfo[i];		//ָ���ļ����
		memset((uint8*)&temp, 0x00, sizeof(FDT));
		ftl_memcpy((uint8*)temp.Name, DirFileName, 11);
		temp.Attr = ATTR_ARCHIVE;		//�浵
		fp->DirClus=GetDirClusIndex(Path);
		if (fp->DirClus < BAD_CLUS)
		{
            fp->Drive=GetDrive(Path);
			if (AddFDT(fp->Drive, fp->DirClus, &temp) == RETURN_OK)      //�����ļ�
			{
				/* �����ļ���Ϣ */
		        ftl_memcpy(fp->Name, (uint8*)temp.Name, 11);
				fp->Flags = FILE_FLAGS_READ | FILE_FLAGS_WRITE;
				fp->FileSize = 0;
				fp->FstClus = 0;
				fp->Clus = 0;
				fp->Offset = 0;
                Rt=fp;
			}
		}
	}
#ifdef OS_FILE
	RELEASE_FAT_SEM;
#endif
	return (Rt);
}


/*********************************************************************************************************
** ��������	:FindFirst
** ��������	:����ָ��Ŀ¼�ĵ�һ���ļ�
**				 FindData:�ļ����ҽṹ,Path:·��
** �䡡��	:Rt:�ҵ����ļ�Ŀ¼����Ϣ
** ȫ�ֱ���	:
** ����ģ��	:��
** ˵  ��	:����չ��ָ��Ϊ"*"ʱ, Ҳ����ҵ�Ŀ¼
********************************************************************************************************/
uint8 FindDirFirst(FDT *Rt, FIND_DATA* FindData, char *Path)
{
	FindData->Clus=GetDirClusIndex(Path);
    FindData->Drive=GetDrive(Path);
	FindData->Index=0;
	return (FindDirNext(Rt, FindData));
}



/*********************************************************************************************************
** ��������	:FindNext
** ��������	:����ָ��Ŀ¼����һ���ļ�
**			 FindData:�ļ����ҽṹ
** �䡡��	:Rt:�ҵ����ļ�Ŀ¼����Ϣ
** ȫ�ֱ���	:
** ����ģ��	:��
** ˵  ��	:����չ��ָ��Ϊ"*"ʱ, Ҳ����ҵ�Ŀ¼
********************************************************************************************************/
uint8 FindDirNext(FDT *Rt, FIND_DATA* FindData)
{
	uint8 OsRt;

#ifdef OS_FILE
	OBTAIN_FAT_SEM;
#endif

	OsRt=PATH_NOT_FIND;
	if (FindData->Clus < BAD_CLUS)
	{
    	OsRt=FindFileSub(Rt, FindData, 1, "*", ATTR_DIRECTORY);
	}

#ifdef OS_FILE
	RELEASE_FAT_SEM;
#endif
	return (OsRt);
}


/*********************************************************************************************************
** ��������	:GetLongFileName
** ��������	:��ȡ���ļ���
** �䡡��	:��
** �䡡��	:lfn:��UNICODE����ĳ��ļ���16bit
** ȫ�ֱ���	:
** ����ģ��	:��
********************************************************************************************************/
void GetLongFileName(char *lfn)
{
	uint16 i=0;

	do
	{
		*lfn++ = LongFileName[i];
	}while (LongFileName[i++] != '\0');
}


/*********************************************************************************************************
** ��������	:
** ��������	:��ȡ��ǰĿ¼�µ��ļ�����
** �䡡��	:ExtName:�ļ���չ��
** �䡡��	:�ļ�����
** ȫ�ֱ���	:
** ����ģ��	:��
********************************************************************************************************/
uint16 GetTotalFiles(char *Path, char *ExtName)
{
	uint32 offset;
	uint16 TotalFiles;
	FDT temp;
	uint32 DirClus;
    uint8 Drive;

	offset=0; TotalFiles=0;
	DirClus=GetDirClusIndex(Path);
    Drive=GetDrive(Path);
	if (DirClus != BAD_CLUS)
	{
		while (1)
		{
			if (RETURN_OK!=GetFDTInfo(&temp, Drive, DirClus, offset++))
				break;

			if (temp.Name[0]==FILE_NOT_EXIST)
				break;

			if (temp.Name[0]!=FILE_DELETED)
			{
				if ((temp.Attr & (ATTR_DIRECTORY | ATTR_VOLUME_ID)) == 0)	//���ļ�
				{
					while (temp.Attr==ATTR_LFN_ENTRY)		//���ļ���
					{
						GetFDTInfo(&temp, Drive, DirClus, offset++);
					}

					if (FileExtNameMatch(&temp.Name[8], ExtName))
						TotalFiles++;
				}
			}
		}
	}
	return (TotalFiles);
}


/*********************************************************************************************************
** ��������	:RecoveryEncInfo
** ��������	:¼���쳣�ж�ʱ�ϵ�ָ�
** �䡡��	:�ļ��ָ���Ϣ(�ļ���, �ļ��״�, Ŀ¼��)
** �䡡��	:�ָ��ɹ�ָʾ(OK--�ɹ�, ERROR--ʧ��)
** ȫ�ֱ���	:
** ����ģ��	:
** ˵   ��  :RecoveryInfo�ṹ����¼�������״غ��FLASH
********************************************************************************************************/
uint8 RecoveryEncInfo(RCV_INFO* RecoveryInfo)
{
	FDT tmp;
	uint32 FileSize;
	uint32 ThisClus, NextClus;
    pDRIVE_INFO pInfo;

	if (RecoveryInfo->Flag == 0x0f)
	{
		if (FindFDTInfo(&tmp, RecoveryInfo->Drive, RecoveryInfo->DirClus, RecoveryInfo->FileName) == RETURN_OK)
		{
            pInfo=GetDriveInfo(RecoveryInfo->Drive);
            //��FAT��������������ȷ���ļ���С
            for (FileSize=0, ThisClus=RecoveryInfo->FstClus; ; FileSize++)
			{
                NextClus=FATGetNextClus(RecoveryInfo->Drive, ThisClus, 1);
                if (NextClus>=BAD_CLUS || NextClus<=EMPTY_CLUS_1 || NextClus<=ThisClus)
                    break;
                ThisClus=NextClus;
			}
			FileSize <<= pInfo->LogSecPerClus+9;
			tmp.FstClusLO=(uint16)(RecoveryInfo->FstClus & 0xffff);
			tmp.FstClusHI=(uint16)(RecoveryInfo->FstClus >> 16);
			tmp.FileSize=FileSize;
			ChangeFDT(RecoveryInfo->Drive, RecoveryInfo->DirClus, &tmp);
			UpdataFreeMem(RecoveryInfo->Drive, -FileSize);
			#if 0
			if (RecoveryInfo->FileName[8]=='W' && RecoveryInfo->FileName[9]=='A' && RecoveryInfo->FileName[10]=='V')
			{
	            uint32 LBA;
	            uint8 buf[512];
                
				LBA = ((RecoveryInfo->FstClus - 2) << pInfo->LogSecPerClus) + pInfo->FirstDataSector;
                ReadSecs(RecoveryInfo->Drive, LBA, buf, 1);
				buf[7]=(uint8)(FileSize >> 24) & 0xff;
				buf[6]=(uint8)(FileSize >> 16) & 0xff;
				buf[5]=(uint8)(FileSize >> 8) & 0xff;
				buf[4]=(uint8)FileSize & 0xff;

				FileSize -= 512;
				buf[511]=(uint8)(FileSize >> 24) & 0xff;
				buf[510]=(uint8)(FileSize >> 16) & 0xff;
				buf[509]=(uint8)(FileSize >> 8) & 0xff;
				buf[508]=(uint8)FileSize & 0xff;
                WriteSecs(RecoveryInfo->Drive, LBA, buf, 1);
			}
			#endif
            #ifdef IN_SYSTEM				
			ClearEncInfo();
            #endif
		}
	}
	return (OK);
}



/*********************************************************************************************************
** ��������	:FindFile
** ��������	:���ҵ�ǰĿ¼/ȫ��Ŀ¼��ָ�����͵ĵڼ����ļ�
**			 FileNum:�ļ���,Path:·��,ExtName:��չ��
** �䡡��	:Rt:�ҵ����ļ�Ŀ¼����Ϣ
** ȫ�ֱ���	:
** ����ģ��	:��
** ˵  ��	:����չ��ָ��Ϊ"*"ʱ, Ҳ����ҵ�Ŀ¼
********************************************************************************************************/
uint8 FindFile(FDT *Rt, uint16 FileNum, char *Path, char *ExtName)
{
	uint8 OsRt;
    uint16 FileTotal;
    FIND_DATA FindTmp;
    pDRIVE_INFO pInfo;

#ifdef OS_FILE
	OBTAIN_FAT_SEM;
#endif

	OsRt=PATH_NOT_FIND;
	FindTmp.Clus=GetDirClusIndex(Path);
	if (FindTmp.Clus != BAD_CLUS)
	{
        FindTmp.Drive=GetDrive(Path);
        pInfo=GetDriveInfo(FindTmp.Drive);
    	//modify by lxs @2007.03.20, ·���е��ַ��п��ܺ���'\'(�緱�����)
    	if ((Path[0] == '\\'  && Path[1] == '\0') || (Path[1] == ':' && Path[2] == '\\' && Path[3] == '\0'))
    	{
    		GotoRootDir(FindTmp.Drive, ExtName);
            while (1)
    		{
                if(ExtName[0] == '*')
                    FileTotal = pInfo->DirInfo[pInfo->DirDeep].TotalFile + pInfo->DirInfo[pInfo->DirDeep].TotalSubDir;
                else
                    FileTotal = pInfo->DirInfo[pInfo->DirDeep].TotalFile;
                if (FileNum <= FileTotal)
                    break;
    			FileNum -= FileTotal;
    			GotoNextDir(FindTmp.Drive, ExtName);
    			if (pInfo->DirDeep == 0)
    			{
    				goto FindErr;
    			}
    		}
    		FindTmp.Clus=pInfo->PathClus;
    	}
        FindTmp.Index=0;
    	OsRt=FindFileSub(Rt, &FindTmp, FileNum, ExtName, 0);
	}

FindErr:
#ifdef OS_FILE
	RELEASE_FAT_SEM;
#endif
	return (OsRt);
}


/*********************************************************************************************************
** ��������	:FindFile
** ��������	:���ҵ�ǰĿ¼/ȫ��Ŀ¼��ָ�����͵ĵڼ����ļ�
**			 FileNum:�ļ���,Path:·��,ExtName:��չ��
** �䡡��	:Rt:�ҵ����ļ�Ŀ¼����Ϣ
** ȫ�ֱ���	:
** ����ģ��	:��
** ˵  ��	:����չ��ָ��Ϊ"*"ʱ, Ҳ����ҵ�Ŀ¼
********************************************************************************************************/
uint8 FindFileBrowser(FDT *Rt, uint16 FileNum, char *Path, char *ExtName)
{
	uint8 OsRt;
    FIND_DATA FindTmp;

#ifdef OS_FILE
	OBTAIN_FAT_SEM;
#endif

	OsRt=PATH_NOT_FIND;
	FindTmp.Clus=GetDirClusIndex(Path);
	if (FindTmp.Clus != BAD_CLUS)
	{
    	FindTmp.Index=0;
        FindTmp.Drive=GetDrive(Path);
    	OsRt=FindFileSub(Rt, &FindTmp, FileNum, ExtName, 0);
	}

#ifdef OS_FILE
	RELEASE_FAT_SEM;
#endif
	return (OsRt);
}


/*********************************************************************************************************
** ��������	:
** ��������	:��ȫ���ļ���ָ��õ���ǰĿ¼�ļ���ָ��
** �䡡��	:ȫ���ļ�ָ��FileNum, �ļ�����ExtName
** �䡡��	:��ǰĿ¼���ļ���ָ��, ����0����
** ȫ�ֱ���	:
** ����ģ��	:
** ˵    �� :���øú������Զ������Ŀ¼
********************************************************************************************************/
uint16 GetCurFileNum(uint8 Drive, uint16 FileNum, char *ExtName)
{
    pDRIVE_INFO pInfo=GetDriveInfo(Drive);

#ifdef OS_FILE
	OBTAIN_FAT_SEM;
#endif
	GotoRootDir(Drive, ExtName);
	while (FileNum > pInfo->DirInfo[pInfo->DirDeep].TotalFile)
	{
		FileNum -= pInfo->DirInfo[pInfo->DirDeep].TotalFile;
		GotoNextDir(Drive, ExtName);
		if (pInfo->DirDeep == 0)
		{
			FileNum=0;
			break;
		}
	}
#ifdef OS_FILE
	RELEASE_FAT_SEM;
#endif
	return (FileNum);
}


/*********************************************************************************************************
** ��������	:
** ��������	:�ӵ�ǰĿ¼�ļ���ָ��õ�ȫ���ļ���ָ��
** �䡡��	:��ǰĿ¼���ļ���ָ��FileNum, ��ǰĿ¼·��Path, �ļ�����ExtName
** �䡡��	:ȫ���ļ�ָ��, ����0����
** ȫ�ֱ���	:
** ����ģ��	:
** ˵   ��  :���øú������Զ������Ŀ¼
********************************************************************************************************/
uint16 GetGlobeFileNum(uint16 FileNum, char *Path, char *ExtName)
{
	uint32 DirClus;
    uint8 Drive;
    pDRIVE_INFO pInfo;

#ifdef OS_FILE
	OBTAIN_FAT_SEM;
#endif
	DirClus=GetDirClusIndex(Path);
	if (DirClus == BAD_CLUS)
	{
		FileNum=0;
	}
	else
	{
        Drive=GetDrive(Path);
        pInfo=GetDriveInfo(Drive);
		GotoRootDir(Drive, ExtName);
		while (DirClus != pInfo->PathClus)		//ֱ���ҵ���ǰĿ¼
		{
			FileNum += pInfo->DirInfo[pInfo->DirDeep].TotalFile;
			GotoNextDir(Drive, ExtName);
			if (pInfo->DirDeep == 0)
			{
				FileNum=0;
				break;
			}
		}
	}
#ifdef OS_FILE
	RELEASE_FAT_SEM;
#endif
	return (FileNum);
}


/*********************************************************************************************************
** ��������	:FileRefSet
** ��������	:�����ļ�����λ�õ�
** �䡡��	:Handle:�ļ����
** �䡡��	:��
** ȫ�ֱ���	:��
** ����ģ��	:
** ˵  ��	:
********************************************************************************************************/
uint8 FileRefSet(pFILE fp)
{
	uint8 Rt;
    pDRIVE_INFO pInfo;
    
#ifdef OS_FILE
	OBTAIN_FAT_SEM;
#endif
	Rt = PARAMETER_ERR;
	if (fp >= &FileInfo[0] && fp <= &FileInfo[MAX_OPEN_FILES-1])
	{
        pInfo=GetDriveInfo(fp->Drive);
		if ((fp->Offset <= fp->FileSize) &&  (fp->Clus <= pInfo->TotClus+2))
		{
			Rt = RETURN_OK;
			fp->RefClus=fp->Clus;
			fp->RefOffset=fp->Offset;
		}
	}
#ifdef OS_FILE
	RELEASE_FAT_SEM;
#endif
	return (Rt);
}


/*********************************************************************************************************
** ��������	:FileRefReset
** ��������	:�ָ��ļ�����λ�õ�
** �䡡��	:Handle:�ļ����
** �䡡��	:��
** ȫ�ֱ���	:��
** ����ģ��	:
** ˵  ��	:
********************************************************************************************************/
uint8 FileRefReset(pFILE fp)
{
	uint8 Rt;
    pDRIVE_INFO pInfo;
    
#ifdef OS_FILE
	OBTAIN_FAT_SEM;
#endif
	Rt = PARAMETER_ERR;
	if (fp >= &FileInfo[0] && fp <= &FileInfo[MAX_OPEN_FILES-1])
	{
        pInfo=GetDriveInfo(fp->Drive);
		if ((fp->Offset <= fp->FileSize) &&  (fp->Clus <= pInfo->TotClus+2))
		{
			Rt = RETURN_OK;
			fp->Clus=fp->RefClus;
			fp->Offset=fp->RefOffset;
		}
	}
#ifdef OS_FILE
	RELEASE_FAT_SEM;
#endif
	return (Rt);
}



/*********************************************************************************************************
** ��������	:VolumeCreate
** ��������	:�������̾��
** �䡡��	:DirFileName:�����������8.3��ʽ
** �䡡��	:
** �����ο�  
** ȫ�ֱ���	:
** ����ģ��	:
********************************************************************************************************/
void VolumeCreate(uint8 Drive, char *DirFileName)
{
	FDT temp, temp1;
	uint8 i;
	uint8 Result;
	uint32 index;
    pDRIVE_INFO pInfo=GetDriveInfo(Drive);

    memset((uint8*)&temp, 0x00, 32);
    memset((uint8*)temp.Name, ' ', 11);
	for (i = 0; i < 11; i++)
	{	
		if (DirFileName[i] == '\0')
			break;
		temp.Name[i] = DirFileName[i];
	}
	temp.Attr = ATTR_VOLUME_ID | ATTR_ARCHIVE;
	index=0;
	while (1)
	{
		Result = GetFDTInfo(&temp1, Drive, pInfo->RootClus, index);
		if (Result == FDT_OVER || Result != RETURN_OK)
		{
			break;
		}

		if (temp1.Name[0] == FILE_NOT_EXIST)
		{
			SetFDTInfo(Drive, pInfo->RootClus, index, &temp);
			break;
		}
		
		if ((temp.Attr & (ATTR_DIRECTORY | ATTR_VOLUME_ID)) == ATTR_VOLUME_ID)
		{
			for (i=0; i<11; i++)
			{
				if (temp1.Name[i] != temp.Name[i])
				{
					SetFDTInfo(Drive, pInfo->RootClus, index, &temp);
					break;
				}
			}
			break;
		}
		index++;
	}
}
#endif

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/

